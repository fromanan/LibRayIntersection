#pragma once

#include "graphics/RayIntersection.h"

//
// class CRayp
// This is a local copy of the ray class with support for
// faster bounding box intersection testing.
//

class CRayp
{
public:
    ~CRayp(void);

    CRayp(const CRay &r);

    const CGrVector &Origin() const {return m_o;}
    const double Origin(int d) const {return m_o[d];}
    const CGrVector &Direction() const {return m_d;}
    const double Direction(int d) const {return m_d[d];}
    CRayp &operator=(const CRay &r) {m_o = r.Origin(); m_d = r.Direction(); return *this;}
    CGrVector PointOnRay(double t) const {return m_o + m_d * t;}

    // Bounding box intersection support
    const CGrVector &InvDirection() const {return m_invDirection;}

private:
    CRayp();

    CGrVector    m_o;
    CGrVector    m_d;

    // Box intersection support
    CGrVector   m_invDirection;
};
