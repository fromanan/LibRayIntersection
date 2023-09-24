#include "StdAfx.h"
#include "Rayp.h"

CRayp::CRayp(const CRay &r)
{
    m_o = r.Origin(); 
    m_d = r.Direction();

    // Preparation for modified Smit's bounding box intersection test
    // This works even if the direction component is zero because of
    // the characteristics of the IEEE number system.
    m_invDirection.Set(1 / m_d.X(), 1 / m_d.Y(), 1 / m_d.Z());
}

CRayp::~CRayp(void)
{
}

