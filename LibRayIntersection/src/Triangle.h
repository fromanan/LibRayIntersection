#pragma once

#include "IntersectionObject.h"

class CTriangle : public CIntersectionObject
{
public:
    CTriangle();
    ~CTriangle() override;

    CRayIntersection::ObjectType Type() const override {return CRayIntersection::Triangle;}

    void AddVertex(const CGrVector &v) override {if (m_numVertices < 3) m_vertices[m_numVertices++] = v;}
    void AddNormal(const CGrVector &n) override {if (m_numNormals < 3) m_normals[m_numNormals++] = n;}
    void AddTexVertex(const CGrVector &t) override {if (m_numTVertices < 3) m_tvertices[m_numTVertices++] = t;}

    void IntersectInfo(const CGrVector &intersect, CGrVector &p_normal, CGrVector &p_texcoord) const override;

    double ComputeT(const CRayp &ray) override;
    bool SurfaceTest(const CGrVector &intersect) override;

    bool TriangleEnd();

    const CGrVector &GetVertex(const int i) {return m_vertices[i];}
    const CBoundingBox &GetBoundingBox() const override {return mBBox;}

private:
    CGrVector GetBarycentricCoordinate(const CGrVector &p) const;

    CGrVector  m_vertices[3];
    CGrVector  m_normals[3];
    CGrVector  m_tvertices[3];

    CBoundingBox mBBox;

    int m_numVertices;
    int m_numNormals;
    int m_numTVertices;

    CGrVector  m_normal;
    double     m_d;

};
