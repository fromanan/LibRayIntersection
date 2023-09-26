#include "StdAfx.h"
#include "Triangle.h"
#include <cassert>

constexpr double TINY = 1e-10;          // A small value to avoid round-off errors

CTriangle::CTriangle()
{
    m_numVertices = 0;
    m_numNormals = 0;
    m_numTVertices = 0;
}

CTriangle::~CTriangle() = default;

//
// Name :         CTriangle::ComputeT()
// Description :  Compute the t value for a ray and the plane of this polygon
//
double CTriangle::ComputeT(const CRayp &ray)
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

bool CTriangle::SurfaceTest(const CGrVector &intersect)
{
    CGrVector b = GetBarycentricCoordinate(intersect);
    return b[0] >= 0 && b[1] >= 0 && b[2] >= 0;
}

CGrVector CTriangle::GetBarycentricCoordinate(const CGrVector &p) const
{
    CGrVector b(0, 0, 0);

    const CGrVector &p1 = m_vertices[0];
    const CGrVector &p2 = m_vertices[1];
    const CGrVector &p3 = m_vertices[2];

    const double det = (p1.X() - p3.X()) * (p2.Y() - p3.Y()) - (p2.X() - p3.X()) * (p1.Y() - p3.Y());
    if (det == 0)
        return b;

    b[0] = ( (p2.Y() - p3.Y()) * (p.X() - p3.X()) - (p2.X() - p3.X()) * (p.Y() - p3.Y()) ) / det;
    b[1] = ( -(p1.Y() - p3.Y()) * (p.X() - p3.X()) + (p1.X() - p3.X()) * (p.Y() - p3.Y())) / det;
    b[2] = 1 - b[0] - b[1];
    return b;
}

bool CTriangle::TriangleEnd()
{
    // We must have at 3 vertices.
    if (m_numVertices != 3)
        return false;

    // Ensure we have three normals as it should be
    if (m_numNormals == 0)
        return false;

    if (m_numNormals < 3)
    {
        m_normals[1] = m_normals[0];
        m_normals[2] = m_normals[0];
    }

    // We need a surface normal for intersection testing
    const CGrVector ab = m_vertices[1] - m_vertices[0];
    const CGrVector ac = m_vertices[2] - m_vertices[0];

    const CGrVector cross = Cross(ab, ac);

    //
    // Handle triangles with co-linear edge vertices
    //

    const double length = cross.Length3();
    if (length < 1e-9)
    {
        return false;
    }

    m_normal = cross / length;

    // Compute d
    m_d = -Dot3(m_vertices[0], m_normal);

    // Ensure we have enough texture coordinates
    if (m_numTVertices == 0)
    {
        for (int i=0;  i<3; i++)
            m_tvertices[m_numTVertices++] = CGrVector(0, 0, 0);
    }
    else
    {
        while(m_numTVertices < 3)
        {
            m_tvertices[m_numTVertices] = m_tvertices[m_numTVertices-1];
            m_numTVertices++;
        }
    }

    //
    // Protection from negative u,v values
    //

    double min = 0;
    for (auto& m_tvertice : m_tvertices)
    {
        if (m_tvertice.X() < min)
            min = m_tvertice.X();
        if (m_tvertice.Y() < min)
            min = m_tvertice.Y();
    }

    if (min < 0)
    {
        const int add = static_cast<int>(-min) + 1;
        for (auto& m_tvertice : m_tvertices)
        {
            m_tvertice.X() += add;
            m_tvertice.Y() += add;
        }
    }

    // Compute bounding box
    CBoundingBox box;
    box.Set(m_vertices[0]);
    box.Include(m_vertices[1]);
    box.Include(m_vertices[2]);
    SetBoundingBox(box);

    return true;
}

void CTriangle::IntersectInfo(const CGrVector &intersect, CGrVector &p_normal, CGrVector &p_texcoord) const
{
    CGrVector b = GetBarycentricCoordinate(intersect);

    p_normal = m_normals[0] * b[0] + m_normals[1] * b[1] + m_normals[2] * b[2];
    p_normal.Normalize3();
    
    p_texcoord = m_tvertices[0] * b[0] + m_tvertices[1] * b[1] + m_tvertices[2] * b[2];
}
