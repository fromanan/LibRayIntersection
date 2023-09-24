#pragma once

#include "IntersectionObject.h"

class CTriangle : public CIntersectionObject
{
public:
    CTriangle(void);
    virtual ~CTriangle(void);

    virtual CRayIntersection::ObjectType Type() const {return CRayIntersection::Triangle;}

    virtual void AddVertex(const CGrVector &v) {if(m_numVertices < 3) m_vertices[m_numVertices++] = v;}
    virtual void AddNormal(const CGrVector &n) {if(m_numNormals < 3) m_normals[m_numNormals++] = n;}
    virtual void AddTexVertex(const CGrVector &t) {if(m_numTVertices < 3) m_tvertices[m_numTVertices++] = t;}

    virtual void IntersectInfo(const CGrVector &intersect,  
                   CGrVector &p_normal, CGrVector &p_texcoord) const;

    virtual double ComputeT(const CRayp &ray);
    virtual bool SurfaceTest(const CGrVector &intersect);

    bool TriangleEnd();

    const CGrVector &GetVertex(int i) {return m_vertices[i];}

    const CBoundingBox &GetBoundingBox() const {return mBBox;}

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
