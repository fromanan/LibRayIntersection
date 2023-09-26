//
// Name :         RayIntersectionD.cpp
// Description :  Implementation of CRayIntersectionD class.
//                This is an implementation of a Kd tree with the surface area heuristic.
// Author :       Charles B. Owen
// Version :      1.01  4-03-01 Thread safe
//                1.02  3-12-07 Replaced old walking block method with Kd tree.
//                              Notice:  No longer thread safe!!!
//                1.03  4-05-07 Changes to make the system tolerate of bad input
//

#include "stdafx.h"
#include <cassert>
#include <algorithm>
#include <fstream>

#include "RayIntersectionD.h"
#include "Rayp.h"

using namespace std;

// The surface area computation algorithm.  
inline double AreaCompute(const CGrVector& p)
{
    return p.X() * p.Y() + p.X() * p.Z() + p.Y() * p.Z();
}

constexpr double TINY = 1e-10; // A small value to avoid roundoff errors

// 
// To use:
//
// 1.  Call Initialize() to clear the system.
// 2.  Call Material() to set a pointer to the current material property
// 3.  Add polygons to the system:
//     A.  Call PolygonBegin()
//     B.  Call Vertex() to add vertices for the polygon
//         Call Texture() to specify a texture for the polygon
//         Call Normal() to specify a normal for the polygon
//         Call TexVertex() to specify a vertex for the polygon
//     C.  Call PolygonEnd()
// 4.  Call LoadingComplete()
// 5.  Call Intersect() to test for intersections
// 6.  Call IntersectInfo() to get intersection information for rendering
//

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRayIntersectionD::CRayIntersectionD()
{
    // Algorithm parameterization defaults
    m_intersectionCost = 100; // 8;
    m_traverseCost = 1;
    m_maxDepth = 100;
    m_minLeaf = 3;

    // Kd Tree version
    m_root = nullptr; // Root is initially empty

    Clear(); // This will clear everything else
}

CRayIntersectionD::~CRayIntersectionD()
{
    // Delete any allocated KdTree
    delete m_root;
}

//
// Name :         CRayIntersectionD::SaveStats()
// Description :  Function to spit out some statistics to a file named stats.txt
//
void CRayIntersectionD::SaveStats() const
{
    ofstream str("stats.txt");
    str << "Polygons:  " << m_polys.size() << endl;
    str << "Triangles:  " << m_triangles.size() << endl;
    str << "Tree Nodes:  " << m_statNodes << endl;
    str << "Tree Depth:  " << m_statMaxDepth << endl;
    str << "Intersection Tests:  " << m_statTests << endl;
    str << "Object Tests:  " << m_statObjTests << endl;
    str << "Surface Tests:  " << m_statSurfaceTests << endl;
    str << "Average:  " << static_cast<double>(m_statSurfaceTests) / m_statTests << endl;
    str << "One child:  " << m_statOneChild << endl;
}

void CRayIntersectionD::Clear()
{
    delete m_root;
    m_root = nullptr;
    m_polys.clear();
    m_triangles.clear();
    m_mark = 1;
    m_loading = CRayIntersection::None;
    m_loadingObject = nullptr;
    m_sceneBB.SetEmpty();

    // Zero the stats
    m_statNodes = 0;
    m_statTests = 0;
    m_statMaxDepth = 0;
    m_statObjTests = 0;
    m_statSurfaceTests = 0;
    m_statOneChild = 0;
}

/////////////////////////////////////////////////////////////////////
//
// We build a complete list of all polygons for the entire scene first, then
// build the intersection data structure from it.
//
/////////////////////////////////////////////////////////////////////

//
// Name :         CRayIntersectionD::Initialize()
// Description :  Initialize the system to begin receiving polygons.
//
void CRayIntersectionD::Initialize()
{
    Clear();
}

//
// Name :         CRayIntersectionD::PolygonBegin()
// Description :  Begin the creation of a new polygon in the intersection system.
//                This is the first step in the polygon creation process.
//
void CRayIntersectionD::PolygonBegin()
{
    m_loading = CRayIntersection::Polygon;
    m_polys.emplace_back();
    m_loadingObject = &m_polys.back();
}

void CRayIntersectionD::TriangleBegin()
{
    m_loading = CRayIntersection::Triangle;
    m_triangles.emplace_back();
    m_loadingObject = &m_triangles.back();
}

//
// Name :         CRayIntersectionD::Texture()
// Description :  Assign a texture to the current polygon we are creating.
//
void CRayIntersectionD::Texture(ITexture* p_texture) const
{
    assert(m_loadingObject != nullptr);
    m_loadingObject->SetTexture(p_texture);
}

//
// Name :         CRayIntersectionD::Material()
// Description :  Set the material for the current polgon we are creating.
//
void CRayIntersectionD::Material(IMaterial* p_material) const
{
    assert(m_loadingObject != nullptr);
    m_loadingObject->SetMaterial(p_material);
}

//
// Name :         CRayIntersectionD::Normal()
// Description :  Add a normal to the current polygon we are creating.
//
void CRayIntersectionD::Normal(const CGrVector& p_normal) const
{
    assert(m_loadingObject != nullptr);
    m_loadingObject->AddNormal(p_normal);
}

//
// Name :         CRayIntersectionD::TexVertex()
// Description :  Add a texture vertex to the current polygon we are creating.
//
void CRayIntersectionD::TexVertex(const CGrVector& p_tvertex) const
{
    assert(m_loadingObject != nullptr);
    m_loadingObject->AddTexVertex(p_tvertex);
}

//
// Name :         CRayIntersectionD::Vertex()
// Description :  Add a vertex to the current polygon we are creating.
//
void CRayIntersectionD::Vertex(const CGrVector& p_vertex) const
{
    assert(m_loadingObject != nullptr);
    m_loadingObject->AddVertex(p_vertex);
}

void CRayIntersectionD::TriangleEnd()
{
    if (m_loading != CRayIntersection::Triangle)
        return;

    if (CTriangle& t = m_triangles.back(); !t.TriangleEnd())
    {
        // Bad triangle, remove it
        m_triangles.pop_back();
        return;
    }
}

//
// Name :         CRayIntersectionD::PolygonEnd()
// Description :  Indicate the end of a polygon creation.
//
void CRayIntersectionD::PolygonEnd()
{
    if (m_polys.empty())
        return;

    CPolygon& p = m_polys.back();
    if (!p.PolygonEnd())
    {
        // Bad polygon, remove it
        m_polys.pop_back();
        return;
    }

    // This is a test if the polygon is planer (or very near to it).
    // The intersection test does not work right for non-planer polygons.  
    if (vector<CGrVector>& vertices = p.GetVertices(); vertices.size() == 3)
    {
        // This is a triangle. Treat it as such.
        CPolygon poly = m_polys.back();
        m_polys.pop_back();

        auto a = poly.GetVertices().begin(); // First vertex
        auto b = a;
        ++b; // Second
        auto c = b;
        ++c; // Third

        auto an = poly.m_normals.begin();
        auto bn = an;
        if (bn != poly.m_normals.end()) ++bn;
        auto cn = bn;
        if (cn != poly.m_normals.end()) ++cn;
        auto at = poly.m_tvertices.begin();
        auto bt = at;
        if (bt != poly.m_tvertices.end()) ++bt;
        auto ct = bt;
        if (ct != poly.m_tvertices.end()) ++ct;

        // a, b, c is a triangle
        TriangleBegin();
        Material(poly.GetMaterial());
        Texture(poly.GetTexture());

        if (at != poly.m_tvertices.end())
            TexVertex(*at);
        Normal(*an);
        Vertex(*a);

        if (bt != poly.m_tvertices.end())
            TexVertex(*bt);
        if (bn != poly.m_normals.end())
            Normal(*bn);
        Vertex(*b);

        if (ct != poly.m_tvertices.end())
            TexVertex(*ct);
        if (cn != poly.m_normals.end())
            Normal(*cn);
        Vertex(*c);

        TriangleEnd();
    }
    else if (vertices.size() > 3)
    {
        for (auto a = vertices.begin(); a != vertices.end(); ++a)
        {
            if (double r = Dot3(p.GetNormal(), *a) + p.GetD(); r < -0.01 || r > 0.01)
            {
                // Non-planer polygon.  Convert to a triangle fan.
                CPolygon poly = m_polys.back();
                m_polys.pop_back();

                // Convert this to a triangle fan
                a = poly.GetVertices().begin(); // First vertex
                auto b = a;
                ++b;
                auto c = b;
                ++c;

                auto an = poly.m_normals.begin();
                auto bn = an;
                if (bn != poly.m_normals.end()) ++bn;
                auto cn = bn;
                if (cn != poly.m_normals.end()) ++cn;
                auto at = poly.m_tvertices.begin();
                auto bt = at;
                if (bt != poly.m_tvertices.end()) ++bt;
                auto ct = bt;
                if (ct != poly.m_tvertices.end()) ++ct;


                for (; c != poly.GetVertices().end(); ++b, ++c)
                {
                    // a, b, c is a triangle
                    TriangleBegin();
                    Material(poly.GetMaterial());
                    Texture(poly.GetTexture());

                    if (at != poly.m_tvertices.end())
                        TexVertex(*at);
                    Normal(*an);
                    Vertex(*a);

                    if (bt != poly.m_tvertices.end())
                    {
                        TexVertex(*bt);
                        ++bt;
                    }
                    if (bn != poly.m_normals.end())
                    {
                        Normal(*bn);
                        ++bn;
                    }
                    Vertex(*b);

                    if (ct != poly.m_tvertices.end())
                    {
                        TexVertex(*ct);
                        ++ct;
                    }
                    if (cn != poly.m_normals.end())
                    {
                        Normal(*cn);
                        ++cn;
                    }
                    Vertex(*c);

                    TriangleEnd();
                }

                return;
            }
        }
    }
}

/////////////////////////////////////////////////////////////////////
//
// Intersection Testing
//
/////////////////////////////////////////////////////////////////////

//
// Name :         CRayIntersectionD::Intersect()  
// Description :  This is the actual ray intersection test.  
// Parameters :   p_ray - The ray we are testing against the scene.
//                p_maxt - Maximum range to search.
//                p_ignore - Optional point to some object we'll ignore
//                 when testing for intersections.
//                p_nearest - Location to put a pointer to the object
//                 we intersected with.
//                p_t - Location to put the distance to the object.
// Returns :      true for insertion or false if none.
//
bool CRayIntersectionD::Intersect(const CRay& p_ray, const double p_maxt, const CRayIntersection::Object* p_ignore,
                                  const CRayIntersection::Object*& p_nearest, double& p_t, CGrVector& p_intersect)
{
    // Create a copy of the ray that has support for faster intersection testing
    CRayp ray(p_ray);

    m_statTests++; // Count the number of tests
    NewMark(); // New mark for this test

    double tNear = TINY; // Start of the ray, a small value
    double tFar = p_maxt; // End of the ray

    // Clip the ray test range to just include the bounding box for the scene
    const CBoundingBox& sceneBB = m_root->m_bbox; // Root bounding box is entire scene
    for (int d = 0; d < 3; d++) // Loop over the three dimensions
    {
        // What is the value of this dimension at the near and far points of the ray?
        double rFm = ray.Origin(d) + tNear * ray.Direction(d);
        double rTo = ray.Origin(d) + tFar * ray.Direction(d);
        if (rTo < rFm)
        {
            // Ray going in negative direction
            if (rFm > sceneBB.Max(d))
                tNear = (sceneBB.Max(d) - ray.Origin(d)) / ray.Direction(d);
            if (rTo < sceneBB.Min(d))
                tFar = (sceneBB.Min(d) - ray.Origin(d)) / ray.Direction(d);
        }
        else if (rFm < rTo)
        {
            // Ray going in positive direction
            if (rFm < sceneBB.Min(d))
                tNear = (sceneBB.Min(d) - ray.Origin(d)) / ray.Direction(d);
            if (rTo > sceneBB.Max(d))
                tFar = (sceneBB.Max(d) - ray.Origin(d)) / ray.Direction(d);
        }
        else
        {
            // Case of ray direction = 0 for this dimension.  
            // This ray will not hit at all unless it is inside the bounding box
            if (rFm < sceneBB.Min(d) || rFm > sceneBB.Max(d))
                return false; // No intersection...
        }

        if (tNear > tFar)
            return false; // Clipped to nothing, we don't intersect at all
    }

    // There can be problems due to roundoff error so that we would miss
    // an intersection that is exactly at tFar or tNear.  These offsets 
    // allow for that roundoff error.
    tNear -= TINY;
    tFar += TINY;

    if (tFar > p_maxt)
        tFar = p_maxt; // But don't let it go beyond our maximum we're looking for...

    // Keeping track of the nearest polygon found so far
    double nearestT = tFar;
    const CIntersectionObject* nearestP = nullptr;

    // Items we'll put into our stack
    struct StackItem
    {
        StackItem(const CKdNode* n, const double tn, const double tf) : node(n), tNear(tn), tFar(tf) {}

        const CKdNode* node;
        double tNear;
        double tFar;
    };

    // The tree traversal stack
    std::vector<StackItem> stack;
    stack.reserve(10); // Reserving space makes this faster
    // stack.push_back(StackItem(m_root, tNear, tFar));

    //
    // The traversal loop
    //

    bool pop = false;
    const CKdNode* pTree = m_root;
    double pTreeNear = tNear;
    double pTreeFar = tFar;

    while (true)
    {
        if (pop)
        {
            // If the stack is empty, we are done
            if (stack.empty())
                break;

            // Pop the last item off of the stack
            const StackItem& back = stack.back();
            pTree = back.node;
            pTreeNear = back.tNear;
            pTreeFar = back.tFar;

            stack.pop_back();
        }

        // Ignore any nodes that are now farther away than the nearest we know so far.
        if (pTreeNear >= nearestT)
            break;

        pop = true;

        if (pTree->m_left == nullptr && pTree->m_right == nullptr)
        {
            //
            // This is a leaf node.
            // Iterate over all members of this node.
            //

            const CKdNode::Member* m = pTree->m_members.data();
            for (int ip = pTree->m_members.size(); ip > 0; ip--, m++)
            {
                CIntersectionObject* p = m->m_object;

                // Has this member been tested?  We don't need to test again.
                if (p->WasTested(m_mark))
                    continue;

                // Is this a member we ignore?
                if (p == p_ignore)
                {
                    p->SetTested(m_mark);
                    continue;
                }

                // There are two levels of examining a member: 
                // Visit:  set compute the distance and check if it is in the range we
                // are examining.  We only compute this once.
                // Test:  actual interior test of the member

                //
                // Visit 
                //

                double t;
                if (p->WasVisited(m_mark))
                {
                    // Already visited before, t is already computed.
                    t = p->GetT(); // Recover the saved version
                    // Is this farther away than our current 
                    // nearest item? If so, we ignore it.
                    if (t >= nearestT)
                    {
                        continue;
                    }
                }
                else
                {
                    p->SetVisited(m_mark); // Not visited before, mark as visited

                    m_statObjTests++;
                    t = p->ComputeT(ray); // Compute the t value
                    if (t < tNear || t >= nearestT)
                    {
                        p->SetTested(m_mark); // No reason to test again
                        continue; // This member is either too near or 
                        // we've already found a closer one.
                    }
                }

                // If this going to be visited again, wait until then
                if (t > pTreeFar)
                    continue;

                // We know the distance to the plane, so let's test the member
                // to see if the interior point is inside the member.
                p->SetTested(m_mark);

                // What's the intersection point on the plane?
                CGrVector intersect = ray.PointOnRay(t);

                // Interior test for this point
                m_statSurfaceTests++; // Count number of actual member surface tests
                if (!p->SurfaceTest(intersect))
                    continue; // Not on the surface

                // We have a new candidate for nearest member intersection
                nearestT = t;
                nearestP = p;
            }
        }
        else
        {
            // An interior node.  
            assert(pTree->m_members.empty());

            int dim = pTree->m_splitDim; // What is the dimension for the split point?
            double splitPoint = pTree->m_splitPoint;

            // What are the from and to points in the dimension of the split point?
            double rFm = ray.Origin(dim) + ray.Direction(dim) * pTreeNear;
            double rTo = ray.Origin(dim) + ray.Direction(dim) * pTreeFar;

            const CKdNode* left = pTree->m_left;
            const CKdNode* right = pTree->m_right;

            if (left == nullptr)
            {
                // Only a right subtree exists
                if (rFm < splitPoint && rTo < splitPoint)
                {
                    // Ray completely contained in the left side
                    // We would not be going down this side since it does not exist
                    continue;
                }

                if ((rFm > splitPoint && rTo > splitPoint) || rFm == rTo)
                {
                    // Ray completely contained in the right side
                    pTree = right;
                    pop = false;
                    continue;
                }

                // What is the t value at the split point?
                double tAtSplit = (splitPoint - ray.Origin(dim)) / ray.Direction(dim);

                // We must be straddling the two subtrees
                if (rFm < rTo)
                {
                    // Going from lesser to greater. Traverse the left tree first, but it
                    // does not exist, so we then move to the right tree.
                    pTreeNear = tAtSplit;
                    pTree = right;
                    pop = false;
                    continue;
                }
                else
                {
                    // Going from greater to lesser.  Traverse the right tree first
                    pTreeFar = tAtSplit;
                    pTree = right;
                    pop = false;
                    continue;
                }
            }
            else if (right == nullptr)
            {
                // Right is null, so only a left tree exists

                if ((rFm < splitPoint && rTo < splitPoint) || rFm == rTo)
                {
                    // Ray completely contained in the left side
                    pTree = left;
                    pop = false;
                    continue;
                }

                if (rFm > splitPoint && rTo > splitPoint)
                {
                    // Ray completely contained in the right side
                    continue;
                }

                // What is the t value at the split point?
                double tAtSplit = (splitPoint - ray.Origin(dim)) / ray.Direction(dim);

                // We must be straddling the two subtrees
                if (rFm < rTo)
                {
                    // Going to greater to lesser. Traverse the left tree first
                    pTreeFar = tAtSplit;
                    pTree = left;
                    pop = false;
                    continue;
                }
                else
                {
                    // Going from lesser to greater.  Traverse the right tree first
                    pTreeNear = tAtSplit;
                    pTree = left;
                    pop = false;
                    continue;
                }
            }
            else
            {
                // We have both children
                // Easy cases first:  Only traverse one child...
                if (rFm < splitPoint && rTo < splitPoint)
                {
                    // Only traverse the left tree
                    // Because we didn't hit the split point, the t range remains the same
                    pTree = left;
                    pop = false;

                    continue;
                }

                if (rFm > splitPoint && rTo > splitPoint)
                {
                    // Only traverse the right tree
                    pTree = right;
                    pop = false;

                    continue;
                }

                if (rFm == rTo)
                {
                    // We must be going right down the split.  Either side may have 
                    // plane parallel to this one, so do both of them.

                    //if (left->m_bbox.IntersectTest(ray, pTreeNear, pTreeFar))
                    stack.emplace_back(left, pTreeNear, pTreeFar);

                    //if (right->m_bbox.IntersectTest(ray, pTreeNear, pTreeFar))
                    {
                        pTree = right;
                        pop = false;
                    }

                    continue;
                }

                // What is the t value at the split point?
                double tAtSplit = (splitPoint - ray.Origin(dim)) / ray.Direction(dim);

                // We must be straddling the two subtrees
                if (rFm < rTo)
                {
                    // Going from lesser to greater.  Traverse left tree first, so top of stack
                    // then the right tree, but only if not too far away.

                    if (tAtSplit < nearestT)
                    {
                        //if (right->m_bbox.IntersectTest(ray, tAtSplit, pTreeFar))
                        stack.emplace_back(right, tAtSplit, pTreeFar);
                    }

                    //  if (left->m_bbox.IntersectTest(ray, pTreeNear, tAtSplit))
                    {
                        pTreeFar = tAtSplit;
                        pTree = left;
                        pop = false;
                    }
                }
                else
                {
                    if (tAtSplit < nearestT)
                    {
                        //if (left->m_bbox.IntersectTest(ray, tAtSplit, pTreeFar))
                        stack.emplace_back(left, tAtSplit, pTreeFar);
                    }

                    //   if (right->m_bbox.IntersectTest(ray, pTreeNear, tAtSplit))
                    {
                        pTreeFar = tAtSplit;
                        pTree = right;
                        pop = false;
                    }
                }
            }
        }
    }

#if 0
            //
            // ORIGINAL VERSION
            //

            // Easy cases first:  Only traverse one child...
            if (rFm < splitPoint && rTo < splitPoint)
            {
                if (left != nullptr)
                {
                    // Only traverse the left tree
                    // Because we didn't hit the split point, the t range remains the same
                    pTree = left;
                    pop = false;
                }

                continue;
            }

            const CKdNode *right = pTree->m_right;

            if (rFm > splitPoint && rTo > splitPoint)
            {
                if (right != nullptr)
                {
                    // Only traverse the right tree
                    pTree = right;
                    pop = false;
                }

                continue;
            }


            if (rFm == rTo)
            {
                if (left == nullptr)
                {
                    pTree = right;
                    pop = false;
                    continue;
                }
                else if (right == nullptr)
                {
                    pTree = left;
                    pop = false;
                    continue;
                }

                // We must be going right down the split.  Either side may have 
                // plane parallel to this one, so do both of them.

                //if (left->m_bbox.IntersectTest(ray, pTreeNear, pTreeFar))
                    stack.push_back(StackItem(left, pTreeNear, pTreeFar));

                //if (right->m_bbox.IntersectTest(ray, pTreeNear, pTreeFar))
                {
                    pTree = right;
                    pop = false;
                }

                continue;
            }

                
            // What is the t value at the split point?
            double tAtSplit = (splitPoint - ray.Origin(dim)) / ray.Direction(dim);

            // We must be straddling the two subtrees
            if (rFm < rTo)
            {
                // Going from lesser to greater.  Traverse left tree first, so top of stack
                // then the right tree, but only if not too far away.

                if (left == nullptr)
                {
                    pTreeNear = tAtSplit;
                    pTree = right;
                    pop = false;
                }
                else if (right == nullptr)
                {
                    if (tAtSplit < nearestT)
                    {
                        pTreeFar = tAtSplit;
                        pTree = left;
                        pop = false;
                    }
                }
                else
                {
                    if (tAtSplit < nearestT)
                    {
                        //if (right->m_bbox.IntersectTest(ray, tAtSplit, pTreeFar))
                            stack.push_back(StackItem(right, tAtSplit, pTreeFar));
                    }

                  //  if (left->m_bbox.IntersectTest(ray, pTreeNear, tAtSplit))
                    {
                        pTreeFar = tAtSplit;
                        pTree = left;
                        pop = false;
                    }
                }

            }
            else
            {
                if (left == nullptr)
                {
                    pTreeFar = tAtSplit;
                    pTree = right;
                    pop = false;
                }
                else if (right == nullptr)
                {
                    if (tAtSplit < nearestT)
                    {
                        pTreeNear = tAtSplit;
                        pTree = left;
                        pop = false;
                    }
                }
                else
                {
                    if (tAtSplit < nearestT)
                    {
                        //if (left->m_bbox.IntersectTest(ray, tAtSplit, pTreeFar))
                            stack.push_back(StackItem(left, tAtSplit, pTreeFar));
                    }

                 //   if (right->m_bbox.IntersectTest(ray, pTreeNear, tAtSplit))
                    {
                        pTreeFar = tAtSplit;
                        pTree = right;
                        pop = false;
                    }
                }

            }

#endif
    
    if (nearestP != nullptr)
    {
        p_nearest = nearestP;
        p_t = nearestT;
        p_intersect = ray.PointOnRay(nearestT);
        return true; // We have a hit
    }

    return false; // No hit
}

//
// Name :         CRayIntersectionD::IntersectInfo()
// Description :  Given a polygon that we have intersected with, return
//                intersection information.  
// Input Parms :  p_ray - The ray we are intersecting with.
//                p_object - Pointer to the intersting object (from CRayIntersectionD::Intersect)
//                p_t - Distance to the intersection point.
// Output Parms : p_normal - The polygon normal at the intersection.
//                p_material - Pointer to the material for this polygon
//                p_texture - Pointer to the texture for this polygon (or nullptr)
//                p_texcoord - The texture coordinate at the intersection
//
void CRayIntersectionD::IntersectInfo(const CRay& p_ray, const CRayIntersection::Object* p_object, double p_t,
                                      CGrVector& p_normal, IMaterial*& p_material,
                                      ITexture*& p_texture, CGrVector& p_texcoord)
{
    // Compute intersection point
    const CGrVector intersect = p_ray.Origin() + p_ray.Direction() * p_t;

    // Make it an object
    const auto* obj = dynamic_cast<const CIntersectionObject*>(p_object);
    p_texture = obj->GetTexture();
    p_material = obj->GetMaterial();
    obj->IntersectInfo(intersect, p_normal, p_texcoord);
}


//
// Name :         CRayIntersectionD::LoadingComplete()
// Description :  Indicate that everything we might want to load
//                into the ray intersection structure is loaded.
//
void CRayIntersectionD::LoadingComplete()
{
    assert(m_polys.size() + m_triangles.size() > 0);

    // Determine the extents in each dimension
    DetermineExtents();

    // Kd Tree Version
    KdTreeBuild();
}

//
// Name :         CRayIntersectionD::KdTreeBuild()
// Description :  Build the Kd tree after we have loaded all of the polygons.
//
void CRayIntersectionD::KdTreeBuild()
{
    delete m_root;

    m_root = new CKdNode(this); // Create the root node
    m_root->SetBoundingBox(m_sceneBB); // Initial box is the scene bounding box
    m_statNodes++; // Counts the nodes
    m_statMaxDepth = 1; // For the root node only tree

    //
    // Build a tree of nodes all at the same level
    // Iterate over all polygons and triangles
    //

    for (auto poly = m_polys.begin(); poly != m_polys.end(); ++poly)
    {
        CPolygon* p = &*poly;
        if (p->GetNumVertices() < 4)
            continue;

        m_root->Add(p);
    }

    // And the triangles
    for (auto tri = m_triangles.begin(); tri != m_triangles.end(); ++tri)
    {
        CTriangle* t = &*tri;
        m_root->Add(t);
    }

    // Shrink the bounding box around the members
    //  m_root->ShrinkBoundingBox();

    // We have a complete Kd tree at this point. 
    // Split into children.
    m_root->Subdivide();

    // For statistics purposes
    Traverse(m_root);
}

void CRayIntersectionD::Traverse(const CKdNode* node)
{
    if (node->m_left == nullptr && node->m_right == nullptr)
        return;

    if (node->m_left == nullptr || node->m_right == nullptr)
        m_statOneChild++;

    if (node->m_left)
        Traverse(node->m_left);

    if (node->m_right)
        Traverse(node->m_right);
}

//
// Name :         CRayIntersectionD::DetermineExtents()
// Description :  We need to know the range of the scene, so determine a
//                bounding box for the entire scene.
//
void CRayIntersectionD::DetermineExtents()
{
    // Initially, just fill min and max with an arbitrary vertex.
    auto poly = m_polys.begin();
    auto tri = m_triangles.begin();

    // Set the bounding box to include just one first point
    if (poly != m_polys.end())
    {
        m_sceneBB.Set(poly->GetVertex(0));
    }
    else if (tri != m_triangles.end())
    {
        m_sceneBB.Set(tri->GetVertex(0));
    }

    // Iterate over all polygons and vertices.
    for (; poly != m_polys.end(); ++poly)
    {
        for (int i = 0; i < poly->GetNumVertices(); i++)
        {
            // Add the new point
            m_sceneBB.Include(poly->GetVertex(i));
        }
    }

    // iterate over the triangles
    for (; tri != m_triangles.end(); ++tri)
    {
        m_sceneBB.Include(tri->GetVertex(0));
        m_sceneBB.Include(tri->GetVertex(1));
        m_sceneBB.Include(tri->GetVertex(2));
    }
}
