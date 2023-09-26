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
    ~CRayp();

    CRayp(const CRay &r);

    const CGrVector &Origin() const {return m_o;}
    double Origin(const int d) const {return m_o[d];}
    const CGrVector &Direction() const {return m_d;}
    double Direction(const int d) const {return m_d[d];}
    CRayp &operator=(const CRay &r) {m_o = r.Origin(); m_d = r.Direction(); return *this;}
    CGrVector PointOnRay(const double t) const {return m_o + m_d * t;}

    // Bounding box intersection support
    const CGrVector &InvDirection() const {return m_invDirection;}

private:
    CRayp() = delete;

    CGrVector    m_o;
    CGrVector    m_d;

    // Box intersection support
    CGrVector   m_invDirection;
};
