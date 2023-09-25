//
// Name :         OpenGLRenderer.h
// Description :  Header for COpenGLRenderer
//                Plug-in renderer fro OpenGL
//                See OpenGLRenderer.cpp
// Author :       Charles B. Owen
//

#pragma once

#include "grafx.h"

#include "GrRenderer.h"

class COpenGLRenderer : public CGrRenderer  
{
public:
	COpenGLRenderer();
	virtual ~COpenGLRenderer();

    virtual bool RendererStart();
    virtual bool RendererEnd();
    virtual void RendererEndPolygon();
    virtual void RendererColor(double *c);
    virtual void RendererMaterial(CGrMaterial *p_material);
    virtual void RendererTranslate(double x, double y, double z);
    virtual void RendererTransform(const CGrTransform *p_transform);
    virtual void RendererRotate(double a, double x, double y, double z);
    virtual void RendererPopMatrix();
    virtual void RendererPushMatrix();

};
