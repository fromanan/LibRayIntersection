#include "StdAfx.h"
#include "IntersectionObject.h"

CIntersectionObject::CIntersectionObject()
{
    m_texture = nullptr;  
    m_material = nullptr;

    m_markVisited = 0;  // Indicates if we computed T in a pass (visited at all)
    m_markTested = 0;   // Indicates if we did an intersection test in a pass
    m_t = 0;            // t computed for the current ray
}

CIntersectionObject::~CIntersectionObject() = default;