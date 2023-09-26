//
// Name :         GrObject.h
// Description :  Scene graph library basic components.  
// Author :       Charles B. Owen
// Version :       2-18-01 1.01 Revisions to make CGrPtr work in vectors
//

#pragma once

#include "grafx.h"

#include "GrPoint.h"
#include "GrTransform.h"
#include <list>

// This allows for forward references
class CGrTexture;
class CGrRenderer;

// class CGrObject
// Superclass for all graphics objects
class CGrObject  
{
public:
    CGrObject() {m_refs = 0;}
    virtual ~CGrObject();

    virtual void glRender() = 0;
    virtual void Render(CGrRenderer *p_renderer) = 0;

    void IncRef() {m_refs++;}
    void DecRef() {m_refs--;  if (m_refs == 0) {delete this;}}
    int  RefCnt() const {return m_refs;}

private:
    int   m_refs;
};

// class CGrPtr
// Class that is a pointer to a graphics object
template <class T> class CGrPtr
{
public:
    CGrPtr() {m_ptr = nullptr;}
    CGrPtr(T *p_ptr) {m_ptr = p_ptr;  if (m_ptr) m_ptr->IncRef();}
    CGrPtr(const CGrPtr &p_ptr) {m_ptr=p_ptr.m_ptr;  if (m_ptr) m_ptr->IncRef();}
    ~CGrPtr() {Clear();}

    void Clear() {if (m_ptr) {m_ptr->DecRef(); m_ptr = nullptr;}}
    T *operator=(T *t) {if (t) t->IncRef();  Clear();  m_ptr = t;  return m_ptr;}
    T *operator=(CGrPtr &t) {if (t.m_ptr) t.m_ptr->IncRef();  Clear();  m_ptr = t.m_ptr;  return m_ptr;}
    operator T *() const {return m_ptr;}
    T *operator->() const {return m_ptr;}

private:
    T *m_ptr;
};

// class CGrPolygon
// Class for a single polygon
class CGrPolygon : public CGrObject
{
public:
    CGrPolygon();
    CGrPolygon(double *a, double *b, double *c, double *d=nullptr);
    ~CGrPolygon() override;

    void glRender() override;
    void Render(CGrRenderer *p_renderer) override;

    void AddVertex3d(double x, double y, double z) {m_vertices.emplace_back(x, y, z);}
    void AddVertex3dv(double *p) {m_vertices.emplace_back(p[0], p[1], p[2]);}
    void AddNormal3d(double x, double y, double z);
    void AddNormal3dv(double *p);
    void AddTexVertex3d(double x, double y, double z, double s, double t);

    void AddTex2d(double s, double t) {m_tvertices.emplace_back(s, t, 0);}

    void AddVertices3(const double *a, const double *b, const double *c, bool p_computenormal=false);
    void AddVertices4(const double *a, const double *b, const double *c, const double *d, bool p_computenormal=false);

    void RectXY(double x, double y, double z, double dx, double dy);
    void RectYZ(double x, double y, double z, double dy, double dz);
    void RectZX(double x, double y, double z, double dz, double dx);

    void Texture(CGrTexture *p_texture);

    void ComputeNormal();
    void ClearNormals() {m_normals.clear();}

    // Access functions
    std::list<CGrPoint> Normals() const {return m_normals;}

private:
    // A polygon is a list of vertices
    std::list<CGrPoint> m_vertices;     // The polygon vertices
    std::list<CGrPoint> m_tvertices;    // The texture vertices
    std::list<CGrPoint> m_normals;      // Vertex normals

    // Do we have an associated texture?
    CGrPtr<CGrTexture>  m_texture;
};

// class CGrColor
// Class for setting the color
class CGrColor : public CGrObject
{
public:
    CGrColor() {c[0]=c[1]=c[2] = 0.;  c[3] = 1.;}
    CGrColor(const double r, const double g, const double b) {c[0]=r; c[1]=g; c[2]=b;  c[3] = 1.;}
    CGrColor(const double r, const double g, const double b, CGrObject *p_child) {c[0]=r; c[1]=g; c[2]=b;  c[3] = 1.;  m_child=p_child;}
    ~CGrColor() override;

    void glRender() override;
    void Render(CGrRenderer *p_renderer) override;

    void Child(CGrObject *p_child) {m_child = p_child;}

private:
    double c[4];
    CGrPtr<CGrObject> m_child;
};

// class CGrComposite
// Class for a composite object
class CGrComposite : public CGrObject
{
public:
    CGrComposite() = default;
    ~CGrComposite() override;

    void glRender() override;
    void Render(CGrRenderer *p_renderer) override;

    void Child(CGrObject *p_child) {m_children.push_back(static_cast<CGrPtr<CGrObject>>(p_child));}

    void AddMappedRect(CGrTexture *p_texture, double x1, double y1, double x2, double y2, 
        double xd, double yd, double so, double to);

    // Automatic functions to fill this composite with polygons
    void Box(double x, double y, double z, double dx, double dy, double dz, CGrTexture *p_texture=nullptr);
    void SlantBox(double x, double y, double z, double dx, double dy, double dz, double lift);
    void Poly3(const CGrPoint &a, const CGrPoint &b, const CGrPoint &c, CGrTexture *p_texture=nullptr);
    void Poly4(const CGrPoint &a, const CGrPoint &b, const CGrPoint &c, const CGrPoint &d, CGrTexture *p_texture=nullptr);

    void Pyramid(double x, double y, double z, double dx, double dy, double dz, CGrTexture* p_texture=nullptr);

    void Sphere(double x, double y, double z, double r, CGrTexture* p_texture);

    void SphereFace(int p_recurse, CGrPtr<CGrPolygon>& poly, CGrTexture* p_texture, double p_radius, double x, double y, double z,double* a, double* b, double* c);

private:
    std::list<CGrPtr<CGrObject> > m_children;
};

// class CGrTranslate
// Class for a translation object
class CGrTranslate : public CGrObject
{
public:
    CGrTranslate() {m_x=m_y=m_z = 0.;}
    CGrTranslate(const double x, const double y, const double z) {m_x=x;  m_y=y;  m_z=z;}
    CGrTranslate(const double x, const double y, const double z, CGrObject *p_child) {m_x=x;  m_y=y;  m_z=z;  m_child=p_child;}
    ~CGrTranslate() override;

    void X(const double x) {m_x = x;}
    void Y(const double y) {m_y = y;}
    void Z(const double z) {m_z = z;}
    void Translate(const double x, const double y, const double z) {m_x = x; m_y = y; m_z = z;}
    void Translate(const CGrPoint& p) {m_x = p.X();  m_y = p.Y();  m_z = p.Z();}

    void glRender() override;
    void Render(CGrRenderer *p_renderer) override;

    void Child(CGrObject *p_child) {m_child = p_child;}

private:
    CGrPtr<CGrObject> m_child;
    double m_x, m_y, m_z;
};

// class CGrSgTransform
// Class for a generic transform object
class CGrSgTransform : public CGrObject, public CGrTransform
{
public:
    CGrSgTransform() = default;
    ~CGrSgTransform() override;

    void glRender() override;
    void Render(CGrRenderer *p_renderer) override;

    void Child(CGrObject *p_child) {m_child = p_child;}
    void Transform(const CGrTransform &p_tran) {CGrTransform::operator=(p_tran);}

private:
    CGrPtr<CGrObject> m_child;
};

// class CGrRotate
// Class for a rotation object
class CGrRotate : public CGrObject
{
public:
    CGrRotate() {m_angle = 0;  m_x=1.0; m_y=m_z = 0.;}
    CGrRotate(const double a, const double x, const double y, const double z) {m_angle=a;  m_x=x;  m_y=y;  m_z=z;}
    CGrRotate(const double a, const double x, const double y, const double z, CGrObject *p_child) {m_angle=a; m_x=x;  m_y=y;  m_z=z;  m_child=p_child;}
    ~CGrRotate() override;

    void Angle(const double a) {m_angle = a;}

    void glRender() override;
    void Render(CGrRenderer *p_renderer) override;
    void Child(CGrObject *p_child) {m_child = p_child;}

private:
    CGrPtr<CGrObject> m_child;
    double m_angle;
    double m_x, m_y, m_z;
};

// class CGrMaterial
// Class for a material object
class CGrMaterial : public CGrObject
{
public:
    void Clear();
    enum Standards {allblack, texture};

    CGrMaterial();
    CGrMaterial(CGrObject *obj) {Child(obj);}
    CGrMaterial(float dr, float dg, float db, float da=1.f);
    CGrMaterial(float dr, float dg, float db, CGrObject *p_child);
    CGrMaterial(float dr, float dg, float db, float sr, float sg, float sb);
    CGrMaterial(float dr, float dg, float db, float sr, float sg, float sb, CGrObject *p_child);
    CGrMaterial(const Standards s) {Standard(s);}
    CGrMaterial(const Standards s, CGrObject *p_child) {Standard(s);  m_child = p_child;}
    ~CGrMaterial() override;

    void glMaterial() const;

    void glRender() override;
    void Render(CGrRenderer *p_renderer) override;
    void Child(CGrObject *p_child) {m_child = p_child;}

    void Standard(Standards s);
    void AmbientDiffuseSpecularShininess(const float *a, const float *d, const float *s, float sh);
    void Emissive(const float *e);
    void Diffuse(const float r, const float g, const float b, const float a=1.f) 
    {m_diffuse[0] = r;  m_diffuse[1] = g;  m_diffuse[2] = b;  m_diffuse[3] = a;}
    void Specular(const float r, const float g, const float b, const float a=1.f) 
    {m_specular[0] = r;  m_specular[1] = g;  m_specular[2] = b;  m_specular[3] = a;}
    void SpecularOther(const float r, const float g, const float b, const float a=1.f) 
    {m_specularother[0] = r;  m_specularother[1] = g;  m_specularother[2] = b;  m_specularother[3] = a;}
    void Ambient(const float r, const float g, const float b, const float a=1.f) 
    {m_ambient[0] = r;  m_ambient[1] = g;  m_ambient[2] = b;  m_ambient[3] = a;}
    void Emission(const float r, const float g, const float b, const float a=1.f) 
    {m_emission[0] = r;  m_emission[1] = g;  m_emission[2] = b;  m_emission[3] = a;}
    void Shininess(const float s) 
    {m_shininess = s;}
    void AmbientAndDiffuse(const float r, const float g, const float b, const float a=1.f) 
    {m_diffuse[0] = r;  m_diffuse[1] = g;  m_diffuse[2] = b;  m_diffuse[3] = a;
    for (int c=0;  c<4;  c++) {m_ambient[c] = m_diffuse[c];}}

    float Ambient(const int i) const {return m_ambient[i];}
    const float *Ambient() const {return m_ambient;}
    float Diffuse(const int i) const {return m_diffuse[i];}
    const float *Diffuse() const {return m_diffuse;}
    float Specular(const int i) const {return m_specular[i];}
    const float *Specular() const {return m_specular;}
    float Shininess() const {return m_shininess;}
    float SpecularOther(const int i) const {return m_specularother[i];}

private:
    CGrPtr<CGrObject> m_child;

    float m_diffuse[4];
    float m_specular[4];
    float m_specularother[4];
    float m_ambient[4];
    float m_emission[4];
    float m_shininess;
};
