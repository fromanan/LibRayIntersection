//
// Name :         RayIntersectionD.h
// Description :  Header for CRayIntersectionD
//                Polygon ray intersection support class.
// Author :       Charles B. Owen
//                For further information, see RayIntersection.cpp
//

#pragma once

#include <list>
#include <vector>

#include "graphics/RayIntersection.h"
#include "Polygon.h"
#include "Triangle.h"
#include "BoundingBox.h"
#include "KdNode.h"

class CRayIntersectionD  
{
public:
	CRayIntersectionD();
	virtual ~CRayIntersectionD();

    void Clear();
	void Initialize();
	void LoadingComplete();

    // Polygon insertion
	void PolygonBegin();
	void PolygonEnd();

    // Triangle insertion
	void TriangleBegin();
	void TriangleEnd();

    // Generic insertion routines
	void Material(IMaterial *p_material);
	void Vertex(const CGrVector &p_vertex);
	void TexVertex(const CGrVector &p_tvertex);
	void Normal(const CGrVector &p_normal);
	void Texture(ITexture *p_texture);

    double SetIntersectionCost(double c) {m_intersectionCost = c;  return c;}
    double GetIntersectionCost() const {return m_intersectionCost;}
    double SetTraverseCost(double c) {m_traverseCost = c;  return c;}
    double GetTraverseCost() const {return m_traverseCost;}
    int SetMaxDepth(int m) {m_maxDepth = m;  return m;}
    int GetMaxDepth() const {return m_maxDepth;}
    int SetMinLeaf(int m) {m_minLeaf = m;  return m;}
    int GetMinLeaf() const {return m_minLeaf;}
   
   // Intersection testing
   bool Intersect(const CRay &p_ray, double p_maxt, const CRayIntersection::Object *p_ignore, 
       const CRayIntersection::Object *&p_object, double &p_t, CGrVector &p_intersect);
   void IntersectInfo(const CRay &p_ray, const CRayIntersection::Object *p_object, double p_t, 
                      CGrVector &p_normal, IMaterial *&p_material, 
                      ITexture *&p_texture, CGrVector &p_texcoord) const; 

    void SaveStats();

    int GetMaxDepth() {return m_maxDepth;}
    int GetMinLeaf() {return m_minLeaf;}
    double GetIntersectionCost() {return m_intersectionCost;}
    double GetTraverseCost() {return m_traverseCost;}
    void StatIncNodes(int c=1) {m_statNodes += c;}
    void NewDepth(int d) {if(d > m_statMaxDepth) m_statMaxDepth = d;}

    int GetMark() {return m_mark;}
    int NewMark() {m_mark++;  return m_mark;}

private:
    void KdTreeBuild();
	void DetermineExtents();
    void Traverse(CKdNode *node);

    CRayIntersection::ObjectType m_loading; // Type of object we are loading
    CIntersectionObject *m_loadingObject;   // Object we are loading
    std::list<CPolygon>  m_polys;           // List of all polygons
    std::list<CTriangle> m_triangles;       // List of all triangles

    int                 m_mark;

    // Some basic parameters
    double              m_intersectionCost; // Cost to compute an intersection
    double              m_traverseCost;     // Cost to traverse a child node
    int                 m_maxDepth;         // Maximum allowed tree depth
    int                 m_minLeaf;          // Leaves below this will not split

    // Statistics gathering
    int                 m_statNodes;
    int                 m_statTests;
    int                 m_statObjTests;
    int                 m_statSurfaceTests;
    int                 m_statMaxDepth;
    int                 m_statOneChild;

    // The scene bounding box
    CBoundingBox        m_sceneBB;

    // Kd Tree root node
    CKdNode            *m_root;

};
