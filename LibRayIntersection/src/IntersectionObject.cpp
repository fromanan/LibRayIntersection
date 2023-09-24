#include "StdAfx.h"
#include "IntersectionObject.h"

CIntersectionObject::CIntersectionObject(void)
{
    m_texture = NULL;  
    m_material = NULL;


    m_markVisited = 0;  // Indicates if we computed T in a pass (visited at all)
    m_markTested = 0;   // Indicates if we did an intersection test in a pass
    m_t = 0;            // t computed for the current ray
}

CIntersectionObject::~CIntersectionObject(void)
{
}
