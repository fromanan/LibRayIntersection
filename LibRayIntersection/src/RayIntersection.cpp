//
// Name :         RayIntersection.cpp
// Description :  Implementation of CRayIntersection class.
// Author :       Charles B. Owen
//

#include "stdafx.h"
#include <cassert>
#include "graphics/RayIntersection.h"

#include "RayIntersectionD.h"

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

CRayIntersection::CRayIntersection()
{
    ri = new CRayIntersectionD();
}

CRayIntersection::~CRayIntersection()
{
    delete ri;
}

void CRayIntersection::Initialize() const {ri->Initialize();}
void CRayIntersection::LoadingComplete() const {ri->LoadingComplete();}

// Polygon insertion
void CRayIntersection::PolygonBegin() const {ri->PolygonBegin();}
void CRayIntersection::PolygonEnd() const {ri->PolygonEnd();}

// Triangle insertion
void CRayIntersection::TriangleBegin() const {ri->TriangleBegin();}
void CRayIntersection::TriangleEnd() const {ri->TriangleEnd();}

// Generic insertion routines
void CRayIntersection::Material(IMaterial *p_material) const {ri->Material(p_material);}
void CRayIntersection::Vertex(const CGrVector &p_vertex) const {ri->Vertex(p_vertex);}
void CRayIntersection::TexVertex(const CGrVector &p_tvertex) const {ri->TexVertex(p_tvertex);}
void CRayIntersection::Normal(const CGrVector &p_normal) const {ri->Normal(p_normal);}
void CRayIntersection::Texture(ITexture *p_texture) const {ri->Texture(p_texture);}

// Parameter routines
double CRayIntersection::SetIntersectionCost(double c) {return ri->SetIntersectionCost(c);}
double CRayIntersection::GetIntersectionCost() const {return ri->GetIntersectionCost();}
double CRayIntersection::SetTraverseCost(double c) {return ri->SetTraverseCost(c);}
double CRayIntersection::GetTraverseCost() const {return ri->GetTraverseCost();}
int CRayIntersection::SetMaxDepth(int m) {return ri->SetMaxDepth(m);}
int CRayIntersection::GetMaxDepth() const {return ri->GetMaxDepth();}
int CRayIntersection::SetMinLeaf(int m) {return ri->SetMinLeaf(m);}
int CRayIntersection::GetMinLeaf() const {return ri->GetMinLeaf();}

bool CRayIntersection::Intersect(const CRay &p_ray, double p_maxt, const Object *p_ignore, 
                                 const Object *&p_object, double &p_t, CGrVector &p_intersect)
{
    return ri->Intersect(p_ray, p_maxt, p_ignore, p_object, p_t, p_intersect);
}

void CRayIntersection::IntersectInfo(const CRay &p_ray, const Object *p_object, double p_t, 
                  CGrVector &p_normal, IMaterial *&p_material, 
                  ITexture *&p_texture, CGrVector &p_texcoord) const
{
    ri->IntersectInfo(p_ray, p_object, p_t, p_normal, p_material, p_texture, p_texcoord);
}

void CRayIntersection::SaveStats() {ri->SaveStats();}
