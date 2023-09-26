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
	~COpenGLRenderer() override;

	bool RendererStart() override;
	bool RendererEnd() override;
	void RendererEndPolygon() override;
	void RendererColor(double *c) override;
	void RendererMaterial(CGrMaterial *p_material) override;
	void RendererTranslate(double x, double y, double z) override;
	void RendererTransform(const CGrTransform *p_transform) override;
	void RendererRotate(double a, double x, double y, double z) override;
	void RendererPopMatrix() override;
	void RendererPushMatrix() override;

};
