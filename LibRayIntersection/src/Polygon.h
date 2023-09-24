#pragma once

#include "IntersectionObject.h"

class CPolygon : public CIntersectionObject
{
public:
    CPolygon() {}
    virtual ~CPolygon(void);

    virtual CRayIntersection::ObjectType Type() const {return CRayIntersection::Polygon;}

    virtual void AddVertex(const CGrVector &v) {m_vertices.push_back(v);}
    virtual void AddNormal(const CGrVector &n) {m_normals.push_back(n);}
    virtual void AddTexVertex(const CGrVector &t) {m_tvertices.push_back(t);}
    bool PolygonEnd();

    virtual double ComputeT(const CRayp &ray);
    virtual bool SurfaceTest(const CGrVector &intersect);

    virtual void IntersectInfo(const CGrVector &intersect,  
                       CGrVector &p_normal, CGrVector &p_texcoord) const;

    std::vector<CGrVector> &GetVertices() {return m_vertices;}
    const CGrVector &GetVertex(int v) {return m_vertices[v];}
    int GetNumVertices() {return (int)m_vertices.size();}

    double GetD() const {return m_d;}
    const CGrVector &GetNormal() const {return m_normal;}

    std::vector<CGrVector>  m_normals;
    std::vector<CGrVector>  m_tvertices;

private:
    CGrVector  m_normal;
    std::vector<CGrVector>  m_vertices;

    std::vector<CGrVector> m_enormals;     // Edge normals

    double              m_d;            // The D value for ax+by+cz+d=0;

};
