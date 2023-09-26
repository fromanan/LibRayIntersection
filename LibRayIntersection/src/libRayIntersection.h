// libRayIntersection.h : main header file for the libRayIntersection DLL
//

#pragma once

#include "stdafx.h"

#include "resource.h"		// main symbols

// ClibRayIntersectionApp
// See libRayIntersection.cpp for the implementation of this class
//
class ClibRayIntersectionApp : public CWinApp
{
public:
	ClibRayIntersectionApp();

// Overrides
public:
	BOOL InitInstance() override;
	DECLARE_MESSAGE_MAP()
};
