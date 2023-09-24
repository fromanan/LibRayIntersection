#pragma once

#include "graphics/RayIntersection.h"
#include "BoundingBox.h"
#include "Rayp.h"

class CIntersectionObject : public CRayIntersection::Object
{
public:
    CIntersectionObject(void);
    virtual ~CIntersectionObject(void);

    virtual void AddVertex(const CGrVector &v) = 0;
    virtual void AddNormal(const CGrVector &n) = 0;
    virtual void AddTexVertex(const CGrVector &t) = 0;
  
    const CBoundingBox &GetBoundingBox() const {return mBBox;}

    virtual double ComputeT(const CRayp &ray) = 0;   
    virtual bool SurfaceTest(const CGrVector &intersect) = 0;

    virtual void IntersectInfo(const CGrVector &intersect,  
                   CGrVector &p_normal, CGrVector &p_texcoord) const = 0;

    void SetTexture(ITexture *texture) {m_texture = texture;}
    ITexture *GetTexture() const {return m_texture;}
    void SetMaterial(IMaterial *material) {m_material = material;}
    IMaterial *GetMaterial() const {return m_material;}

    bool WasTested(int mark) {return m_markTested == mark;}
    void SetTested(int mark) {m_markTested = mark;}
    bool WasVisited(int mark) {return m_markVisited == mark;}
    void SetVisited(int mark) {m_markVisited = mark;}

    double GetT() {return m_t;}

protected:
    void SetBoundingBox(const CBoundingBox &box) {mBBox = box;}
    void SetT(double t) {m_t = t;}

private:
    // Associated values
    ITexture            *m_texture;
    IMaterial           *m_material;

    // Intersection assistance
    int                 m_markVisited;  // Indicates if we computed T in a pass (visited at all)
    int                 m_markTested;   // Indicates if we did an intersection test in a pass
    double              m_t;            // t computed for the current ray

    CBoundingBox        mBBox;          // Bounding box for object
};
