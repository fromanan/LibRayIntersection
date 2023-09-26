#include "StdAfx.h"
#include "Polygon.h"

using namespace std;

constexpr double TINY = 1e-10; // A small value to avoid round-off errors

CPolygon::~CPolygon() = default;

bool CPolygon::PolygonEnd()
{
    // We must have at least 3 vertices.
    if (m_vertices.size() < 3)
        return false;

    //
    // Test for and remove any coincident vertices
    //
    
    bool valid;
    do
    {
        valid = true; // Until we know otherwise

        // We have to remove any associated normal and texture vertex as well.
        CGrVector last = m_vertices.back();
        auto normal = m_normals.begin();
        auto tvert = m_tvertices.begin();
        for (auto cur = m_vertices.begin(); cur != m_vertices.end();)
        {
            if (last.X() == cur->X() && last.Y() == cur->Y() && last.Z() == cur->Z())
            {
                // Duplicate vertex.  Remove it.
                if (m_normals.size() > 1 && normal != m_normals.end())
                    m_normals.erase(normal);
                if (m_tvertices.size() > 1 && tvert != m_tvertices.end())
                    m_tvertices.erase(tvert);

                m_vertices.erase(cur);

                // Because the erase invalidates the iterators, we'll just start over again
                // from the beginning of the list.
                valid = false;
                break;
            }
            else
            {
                if (normal != m_normals.end())
                    ++normal;
                if (tvert != m_tvertices.end())
                    ++tvert;

                last = *cur;
                ++cur;
            }
        }
    }
    while (m_vertices.size() > 1 && !valid);

    // The following error checks make sure we 
    // have valid polygon configurations.  This avoids
    // having to error check in later code.

    // We must have at least 3 vertices.
    // We may have lost some earlier
    if (m_vertices.size() < 3)
        return false;

    // Other error checks
    // We allow one normal or a normal for every vertex.  
    // No other options are valid..
    if (!(m_normals.size() == 1 || m_normals.size() == m_vertices.size()))
        return false;

    // If we have a texture or if we have provided any texture
    // vertices, we must have a texture vertex for every vertex
    if ((GetTexture() != nullptr || !m_tvertices.empty()) && m_tvertices.size() != m_vertices.size())
        return false;

    // We need a surface normal for intersection testing 
    // independent of the polygon normals.
    auto a = m_vertices.begin();
    auto b = a;
    ++b;
    auto c = b;
    ++c;

    const CGrVector cross = Cross(*c - *b, *a - *b);

    //
    // Handle polygons with co-linear edge vertices
    //

    const double length = cross.Length3();
    if (length < 1e-9)
        return false;

    m_normal = cross / length;

    // Compute d
    m_d = -Dot3(*a, m_normal);

    // My polygon intersection test requires a set of edge normals.
    // These are vectors that are normal to the edge and to the
    // surface normal and point into the interior of the polygon.
    a = m_vertices.begin();
    b = a;
    ++b;

    for (; a != m_vertices.end(); ++a)
    {
        m_enormals.push_back(Normalize3(Cross(m_normal, *b - *a)));

        ++b;
        if (b == m_vertices.end())
            b = m_vertices.begin();
    }

    // It helps in later code if the first vertex is also
    // the last vertex.  Then we can treat the list of 
    // vertices as a list of edges.
    m_vertices.push_back(*m_vertices.begin());

    // Same for the normals if there are more than one
    if (m_normals.size() > 1)
        m_normals.push_back(*m_normals.begin());

    // Same for texture vertices
    if (!m_tvertices.empty())
        m_tvertices.push_back(*m_tvertices.begin());

    // Compute bounding box
    CBoundingBox box;
    a = m_vertices.begin();
    box.Set(*a);
    ++a;

    for (; a != m_vertices.end(); ++a)
    {
        box.Include(*a);
    }

    SetBoundingBox(box);

    return true;
}


//
// Name :         CPolygon::ComputeT()
// Description :  Compute the t value for a ray and the plane of this polygon
//
double CPolygon::ComputeT(const CRayp& ray)
{
    // What's the t value here?  Intersection test with the member plane...
    const double bottom = Dot3(m_normal, ray.Direction());
    if (bottom >= -TINY && bottom <= TINY)
    {
        SetT(-1);
        return -1;
    }

    const double t = -(Dot3(m_normal, ray.Origin()) + m_d) / bottom;
    SetT(t);

    return t;
}

bool CPolygon::SurfaceTest(const CGrVector& intersect)
{
    // Interior test for this point
    vector<CGrVector>::const_iterator n = m_enormals.begin();
    vector<CGrVector>::const_iterator v = m_vertices.begin();

    for (; n != m_enormals.end(); ++n, ++v)
    {
        if (Dot3(*n, intersect - *v) < 0)
            break;
    }

    if (n != m_enormals.end())
        return false; // Not interior

    return true;
}


void CPolygon::IntersectInfo(const CGrVector& intersect, CGrVector& p_normal, CGrVector& p_texcoord) const
{
    //
    // Now we need to determine the interpolated values.  We use the
    // rule here that polygons are convex, so that simplifies things 
    // a bit.  But, we need to get the best numeric resolution, so we 
    // determine the largest component of the polygon normal and
    // project onto the appropriate plane that will maximize the polgon area.
    //

    enum Max { X, Y, Z };

    Max m;
    if (fabs(m_normal.X()) > fabs(m_normal.Y()))
    {
        // Either X or Z
        if (fabs(m_normal.X()) > fabs(m_normal.Z()))
            m = X;
        else
            m = Z;
    }
    else
    {
        // Either Y or Z
        if (fabs(m_normal.Y()) > fabs(m_normal.Z()))
            m = Y;
        else
            m = Z;
    }

    // We're looking of the two edges that overlap
    // the intersection in one dimension.

    // We'll need 4 vertices.  They are:
    // a, b - Left edge
    // c, d - Right edge
    int a = 0;
    int b = 1;
    int c;
    int d;
    const int cnt = static_cast<int>(m_vertices.size());

    // f is the percent from a to b
    // g is the percent from c to d
    // h is the percent from ab side to cd side
    double f, g, h;

    if (m == X)
    {
        // Projecting onto the YZ plane
        // Look for the overlap in the Y dimension
        for (; b < cnt; a++, b++)
        {
            if ((m_vertices[a].Y() >= intersect.Y() && m_vertices[b].Y() < intersect.Y()) ||
                (m_vertices[b].Y() >= intersect.Y() && m_vertices[a].Y() < intersect.Y()))
            {
                break;
            }
        }

        if (b == cnt)
            throw 1;

        c = b;
        d = c;
        if (d < cnt)
            d++;

        for (; d < cnt; c++, d++)
        {
            if ((m_vertices[c].Y() >= intersect.Y() && m_vertices[d].Y() < intersect.Y()) ||
                (m_vertices[d].Y() >= intersect.Y() && m_vertices[c].Y() < intersect.Y()))
            {
                break;
            }
        }

        if (d == cnt)
            throw 1;

        f = (intersect.Y() - m_vertices[a].Y()) /
            (m_vertices[b].Y() - m_vertices[a].Y());
        g = (intersect.Y() - m_vertices[c].Y()) /
            (m_vertices[d].Y() - m_vertices[c].Y());

        const double lz = m_vertices[a].Z() + f * (m_vertices[b].Z() - m_vertices[a].Z());
        const double rz = m_vertices[c].Z() + g * (m_vertices[d].Z() - m_vertices[c].Z());

        h = rz - lz == 0 ? 0 : (intersect.Z() - lz) / (rz - lz);
    }
    else if (m == Y)
    {
        // Project onto XZ plane
        // Look for the overlap in the X dimension
        for (; b < cnt; a++, b++)
        {
            if ((m_vertices[a].X() >= intersect.X() && m_vertices[b].X() < intersect.X()) ||
                (m_vertices[b].X() >= intersect.X() && m_vertices[a].X() < intersect.X()))
            {
                break;
            }
        }

        if (b == cnt)
            throw 1;

        c = b;
        d = c;
        if (d < cnt)
            d++;

        for (; d < cnt; c++, d++)
        {
            if ((m_vertices[c].X() >= intersect.X() && m_vertices[d].X() < intersect.X()) ||
                (m_vertices[d].X() >= intersect.X() && m_vertices[c].X() < intersect.X()))
            {
                break;
            }
        }

        if (d == cnt)
            throw 1;

        f = (intersect.X() - m_vertices[a].X()) /
            (m_vertices[b].X() - m_vertices[a].X());
        g = (intersect.X() - m_vertices[c].X()) /
            (m_vertices[d].X() - m_vertices[c].X());

        const double lz = m_vertices[a].Z() + f * (m_vertices[b].Z() - m_vertices[a].Z());
        const double rz = m_vertices[c].Z() + g * (m_vertices[d].Z() - m_vertices[c].Z());

        h = rz - lz == 0 ? 0 : (intersect.Z() - lz) / (rz - lz);
    }
    else if (m == Z)
    {
        // Project onto XY plane
        // Look for the overlap in the X dimension
        for (; b < cnt; a++, b++)
        {
            if ((m_vertices[a].X() >= intersect.X() && m_vertices[b].X() < intersect.X()) ||
                (m_vertices[b].X() >= intersect.X() && m_vertices[a].X() < intersect.X()))
            {
                break;
            }
        }

        c = b;
        d = c;
        if (d < cnt)
            d++;

        if (b == cnt)
            throw 1;

        for (; d < cnt; c++, d++)
        {
            if ((m_vertices[c].X() >= intersect.X() && m_vertices[d].X() < intersect.X()) ||
                (m_vertices[d].X() >= intersect.X() && m_vertices[c].X() < intersect.X()))
            {
                break;
            }
        }

        if (d == cnt)
            throw 1;

        f = (intersect.X() - m_vertices[a].X()) /
            (m_vertices[b].X() - m_vertices[a].X());
        g = (intersect.X() - m_vertices[c].X()) /
            (m_vertices[d].X() - m_vertices[c].X());

        const double lz = m_vertices[a].Y() + f * (m_vertices[b].Y() - m_vertices[a].Y());
        const double rz = m_vertices[c].Y() + g * (m_vertices[d].Y() - m_vertices[c].Y());

        h = rz - lz == 0 ? 0 : (intersect.Y() - lz) / (rz - lz);
    }

    // Convert values to what is multiplied by each part
    const double am = 1 - f - h + f * h;
    const double bm = f - f * h;
    const double cm = h - g * h;
    const double dm = g * h;

    // Set the normal
    if (m_normals.size() == 1)
        p_normal = m_normals[0];
    else
    {
        p_normal = m_normals[a] * am + m_normals[b] * bm +
            m_normals[c] * cm + m_normals[d] * dm;
        p_normal.Normalize3();
    }

    if (!m_tvertices.empty())
    {
        CGrVector ta = m_tvertices[a];
        CGrVector tb = m_tvertices[b];
        CGrVector tc = m_tvertices[c];
        CGrVector td = m_tvertices[d];

        p_texcoord = m_tvertices[a] * am + m_tvertices[b] * bm +
            m_tvertices[c] * cm + m_tvertices[d] * dm;
    }
}
