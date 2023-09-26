#pragma once

#include "IntersectionObject.h"

class CPolygon : public CIntersectionObject
{
public:
    CPolygon() {}
    ~CPolygon() override;

    CRayIntersection::ObjectType Type() const override {return CRayIntersection::Polygon;}

    void AddVertex(const CGrVector &v) override {m_vertices.push_back(v);}
    void AddNormal(const CGrVector &n) override {m_normals.push_back(n);}
    void AddTexVertex(const CGrVector &t) override {m_tvertices.push_back(t);}
    bool PolygonEnd();

    double ComputeT(const CRayp &ray) override;
    bool SurfaceTest(const CGrVector &intersect) override;

    void IntersectInfo(const CGrVector &intersect, CGrVector &p_normal, CGrVector &p_texcoord) const override;

    std::vector<CGrVector> &GetVertices() {return m_vertices;}
    const CGrVector &GetVertex(const int v) {return m_vertices[v];}
    int GetNumVertices() const { return static_cast<int>(m_vertices.size()); }

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
