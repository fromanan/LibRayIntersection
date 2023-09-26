// GrObject.cpp: implementation of the CGrObject class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "GrObject.h"

#include "GrTexture.h"
#include "GrRenderer.h"

//////////////////////////////////////////////////////////////////////
// CGrObject:  Generic superclass
//////////////////////////////////////////////////////////////////////

CGrObject::~CGrObject() = default;

//////////////////////////////////////////////////////////////////////
// CGrPolygon:  Polygon class
//////////////////////////////////////////////////////////////////////

CGrPolygon::CGrPolygon() = default;

void CGrPolygon::Texture(CGrTexture* p_texture)
{
    m_texture = p_texture;
}

CGrPolygon::CGrPolygon(double* a, double* b, double* c, double* d)
{
    m_vertices.emplace_back(a[0], a[1], a[2]);
    m_vertices.emplace_back(b[0], b[1], b[2]);
    m_vertices.emplace_back(c[0], c[1], c[2]);
    if (d)
        m_vertices.emplace_back(d[0], d[1], d[2]);
}

CGrPolygon::~CGrPolygon() = default;

void CGrPolygon::AddVertices3(const double* a, const double* b, const double* c, bool p_computenormal)
{
    m_vertices.emplace_back(a[0], a[1], a[2]);
    m_vertices.emplace_back(b[0], b[1], b[2]);
    m_vertices.emplace_back(c[0], c[1], c[2]);
    if (p_computenormal)
        ComputeNormal();
}

void CGrPolygon::AddVertices4(const double* a, const double* b, const double* c, const double* d, bool p_computenormal)
{
    m_vertices.emplace_back(a[0], a[1], a[2]);
    m_vertices.emplace_back(b[0], b[1], b[2]);
    m_vertices.emplace_back(c[0], c[1], c[2]);
    m_vertices.emplace_back(d[0], d[1], d[2]);
    if (p_computenormal)
        ComputeNormal();
}


//
// Name :         CGrPolygon::ComputeNormal()
// Description :  For flat polygons, the polygon normal can be computed
//                automatically.  That's what this function does.
// Notice :       Clears any existing normals list.
//

void CGrPolygon::ComputeNormal()
{
    m_normals.clear();

    CGrPoint normal(0, 0, 0, 0); // Zero the normal we are computing
    for (auto coord = m_vertices.begin(); coord != m_vertices.end(); ++coord)
    {
        auto coordnext = coord;
        ++coordnext;

        if (coordnext == m_vertices.end())
            coordnext = m_vertices.begin();

        // Now, coord and coordnext are two sequential vertex 
        // pointers.
        CGrPoint& v1 = *coord;
        CGrPoint& v2 = *coordnext;

        normal[0] -= (v1[2] + v2[2]) * (v2[1] - v1[1]);
        normal[1] -= (v1[0] + v2[0]) * (v2[2] - v1[2]);
        normal[2] -= (v1[1] + v2[1]) * (v2[0] - v1[0]);
    }

    // Normalize
    if (const double len = sqrt(normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]); len != 0.0)
    {
        normal[0] /= len;
        normal[1] /= len;
        normal[2] /= len;
    }

    // Put into the list of normals
    m_normals.push_back(normal);
}


//
// Name :         CGrPolygon::AddTexVertex3d()
// Description :  Add a vertex with an associated texture.
//

void CGrPolygon::AddTexVertex3d(double x, double y, double z, double s, double t)
{
    m_vertices.emplace_back(x, y, z);
    m_tvertices.emplace_back(s, t, 0);
}

void CGrPolygon::AddNormal3d(double x, double y, double z)
{
    m_normals.emplace_back(x, y, z, 0);
    m_normals.back().Normalize3();
}

void CGrPolygon::AddNormal3dv(double* p)
{
    m_normals.emplace_back(p[0], p[1], p[2], 0);
    m_normals.back().Normalize3();
}


//
// Name :         CGrPolygon::glRender()
// Description :  Render this polygon.  Note that this will allow
//                optional texture mapping if you want to use it
//                and it can use a single normal or a bunch.
//

void CGrPolygon::glRender()
{
    if (m_texture)
    {
        glEnable(GL_TEXTURE_2D);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glBindTexture(GL_TEXTURE_2D, m_texture->TexName());
    }

    auto normals = m_normals.begin();
    auto tvertices = m_tvertices.begin();

    glBegin(GL_POLYGON);
    for (auto i = m_vertices.begin(); i != m_vertices.end(); ++i)
    {
        if (normals != m_normals.end())
        {
            normals->glNormal();
            ++normals;
        }

        if (tvertices != m_tvertices.end())
        {
            tvertices->glTexVertex();
            ++tvertices;
        }

        i->glVertex();
    }

    glEnd();

    if (m_texture)
    {
        glDisable(GL_TEXTURE_2D);
    }
}


void CGrPolygon::Render(CGrRenderer* p_renderer)
{
    p_renderer->RendererBeginPolygon();

    if (m_texture)
        p_renderer->RendererTexture(m_texture);

    auto normals = m_normals.begin();
    auto tvertices = m_tvertices.begin();

    for (auto i = m_vertices.begin(); i != m_vertices.end(); i++)
    {
        if (normals != m_normals.end())
        {
            p_renderer->RendererNormal(*normals);
            ++normals;
        }

        if (tvertices != m_tvertices.end())
        {
            p_renderer->RendererTexVertex(*tvertices);
            ++tvertices;
        }

        p_renderer->RendererVertex(*i);
    }

    p_renderer->RendererEndPolygon();
}


//
// Name :         CGrPolygon::RectXY()
// Description :  Helper function to create a rectangle
//                parallel to the XY plane.
//                To use, provide the lower left or upper
//                right corner, when
//                looking from the visible side and the 
//                different values in the x and y directions.   
//
void CGrPolygon::RectXY(const double x, const double y, const double z, const double dx, const double dy)
{
    AddVertex3d(x, y, z);
    AddVertex3d(x + dx, y, z);
    AddVertex3d(x + dx, y + dy, z);
    AddVertex3d(x, y + dy, z);
    AddNormal3d(0, 0, dx > 0 == dy > 0 ? 1. : -1.);
}

//
// Name :         CGrPolygon::RectYZ()
// Description :  Helper function to create a rectangle
//                parallel to the YZ plane.  
//
void CGrPolygon::RectYZ(const double x, const double y, const double z, const double dy, const double dz)
{
    AddVertex3d(x, y, z);
    AddVertex3d(x, y, z + dz);
    AddVertex3d(x, y + dy, z + dz);
    AddVertex3d(x, y + dy, z);
    AddNormal3d(dy > 0 == dz > 0 ? -1. : 1., 0., 0.);
}


//
// Name :         CGrPolygon::RectZX()
// Description :  Helper function to create a rectangle
//                parallel to the XY plane.  
//
void CGrPolygon::RectZX(const double x, const double y, const double z, const double dz, const double dx)
{
    AddVertex3d(x, y, z);
    AddVertex3d(x + dx, y, z);
    AddVertex3d(x + dx, y, z + dz);
    AddVertex3d(x, y, z + dz);
    AddNormal3d(0, dx > 0 == dz > 0 ? -1. : 1., 0.);
}

//////////////////////////////////////////////////////////////////////
// CGrColor:  Color class
//////////////////////////////////////////////////////////////////////

CGrColor::~CGrColor() = default;

void CGrColor::glRender()
{
    glColor4dv(c);
    if (m_child)
        m_child->glRender();
}

void CGrColor::Render(CGrRenderer* p_renderer)
{
    p_renderer->RendererColor(c);
    if (m_child)
        m_child->Render(p_renderer);
}

//////////////////////////////////////////////////////////////////////
// CGrComposite:  Composite object class.
//////////////////////////////////////////////////////////////////////

CGrComposite::~CGrComposite() = default;

void CGrComposite::glRender()
{
    for (auto i = m_children.begin(); i != m_children.end(); ++i)
        (*i)->glRender();
}

void CGrComposite::Render(CGrRenderer* p_renderer)
{
    for (auto i = m_children.begin(); i != m_children.end(); ++i)
        (*i)->Render(p_renderer);
}

//////////////////////////////////////////////////////////////////////
// CGrTranslate:  Translate
//////////////////////////////////////////////////////////////////////

CGrTranslate::~CGrTranslate() = default;

void CGrTranslate::glRender()
{
    if (m_child)
    {
        glPushMatrix();
        glTranslated(m_x, m_y, m_z);
        m_child->glRender();
        glPopMatrix();
    }
}


void CGrTranslate::Render(CGrRenderer* p_renderer)
{
    if (m_child)
    {
        p_renderer->RendererPushMatrix();
        p_renderer->RendererTranslate(m_x, m_y, m_z);
        m_child->Render(p_renderer);
        p_renderer->RendererPopMatrix();
    }
}

//////////////////////////////////////////////////////////////////////
// CGrSgTransform  Generic transformations
//////////////////////////////////////////////////////////////////////

CGrSgTransform::~CGrSgTransform() = default;

void CGrSgTransform::glRender()
{
    if (m_child)
    {
        glPushMatrix();
        CGrTransform::glMultMatrix();
        m_child->glRender();
        glPopMatrix();
    }
}


void CGrSgTransform::Render(CGrRenderer* p_renderer)
{
    if (m_child)
    {
        p_renderer->RendererPushMatrix();
        p_renderer->RendererTransform(this);
        m_child->Render(p_renderer);
        p_renderer->RendererPopMatrix();
    }
}


//////////////////////////////////////////////////////////////////////
// CGrRotate:  Rotate
//////////////////////////////////////////////////////////////////////

CGrRotate::~CGrRotate() = default;

void CGrRotate::glRender()
{
    if (m_child)
    {
        glPushMatrix();
        glRotated(m_angle, m_x, m_y, m_z);
        m_child->glRender();
        glPopMatrix();
    }
}

void CGrRotate::Render(CGrRenderer* p_renderer)
{
    if (m_child)
    {
        p_renderer->RendererPushMatrix();
        p_renderer->RendererRotate(m_angle, m_x, m_y, m_z);
        m_child->Render(p_renderer);
        p_renderer->RendererPopMatrix();
    }
}

//////////////////////////////////////////////////////////////////////
// CGrMaterial:  Sets material properties.
//////////////////////////////////////////////////////////////////////

CGrMaterial::CGrMaterial()
{
    Clear();
}

CGrMaterial::CGrMaterial(const float dr, const float dg, const float db, const float da)
{
    Clear();
    m_diffuse[0] = dr;
    m_diffuse[1] = dg;
    m_diffuse[2] = db;
    m_diffuse[3] = da;
}

CGrMaterial::CGrMaterial(const float dr, const float dg, const float db, CGrObject* p_child)
{
    Clear();
    m_diffuse[0] = dr;
    m_diffuse[1] = dg;
    m_diffuse[2] = db;
    m_diffuse[3] = 1.f;

    Child(p_child);
}

CGrMaterial::CGrMaterial(const float dr, const float dg, const float db, const float sr, const float sg, const float sb)
{
    Clear();

    m_diffuse[0] = dr;
    m_diffuse[1] = dg;
    m_diffuse[2] = db;
    m_diffuse[3] = 1.f;
    m_specular[0] = sr;
    m_specular[1] = sg;
    m_specular[2] = sb;
    m_specular[3] = 1.f;
}

CGrMaterial::CGrMaterial(const float dr, const float dg, const float db, const float sr, const float sg, const float sb,
    CGrObject* p_child)
{
    Clear();

    m_diffuse[0] = dr;
    m_diffuse[1] = dg;
    m_diffuse[2] = db;
    m_diffuse[3] = 1.f;
    m_specular[0] = sr;
    m_specular[1] = sg;
    m_specular[2] = sb;
    m_specular[3] = 1.f;
    Child(p_child);
}

CGrMaterial::~CGrMaterial() = default;

void CGrMaterial::Clear()
{
    for (int c = 0; c < 4; c++)
    {
        m_diffuse[c] = 0.f;
        m_specular[c] = 0.f;
        m_specularother[c] = 0.f;
        m_ambient[c] = 0.f;
        m_emission[c] = 0.f;
    }
    m_shininess = 1.f;
}

void CGrMaterial::AmbientDiffuseSpecularShininess(const float* a, const float* d, const float* s, const float sh)
{
    for (int i = 0; i < 4; i++)
    {
        m_ambient[i] = a[i];
        m_diffuse[i] = d[i];
        m_specular[i] = s[i];
    }

    m_shininess = sh;
}

void CGrMaterial::Emissive(const float* e)
{
    for (int i = 0; i < 4; i++)
        m_emission[i] = e[i];
}

void CGrMaterial::glRender()
{
    if (m_child)
    {
        glMaterialfv(GL_FRONT, GL_DIFFUSE, m_diffuse);

        glMaterialfv(GL_FRONT, GL_SPECULAR, m_specular);

        glMaterialfv(GL_FRONT, GL_AMBIENT, m_ambient);

        glMaterialfv(GL_FRONT, GL_EMISSION, m_emission);

        glMaterialfv(GL_FRONT, GL_SHININESS, &m_shininess);

        m_child->glRender();
    }
}

void CGrMaterial::glMaterial() const
{
    glMaterialfv(GL_FRONT, GL_DIFFUSE, m_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, m_specular);
    glMaterialfv(GL_FRONT, GL_AMBIENT, m_ambient);
    glMaterialfv(GL_FRONT, GL_EMISSION, m_emission);
    glMaterialfv(GL_FRONT, GL_SHININESS, &m_shininess);
}

void CGrMaterial::Render(CGrRenderer* p_renderer)
{
    if (m_child)
    {
        p_renderer->RendererMaterial(this);

        m_child->Render(p_renderer);
    }
}

//
// Name :         CGrMaterial::Standard()
// Description :  There are some combinations that are so 
//                common I consider this standards.  I provide
//                an enum to set these directly.
//
void CGrMaterial::Standard(const Standards s)
{
    int c;
    Clear();

    switch (s)
    {
    case allblack:
        for (c = 0; c < 3; c++)
        {
            m_diffuse[c] = 0.f;
            m_specular[c] = 0.f;
            m_ambient[c] = 0.f;
            m_emission[c] = 0.f;
        }

        m_diffuse[3] = 1.f;
        m_specular[3] = 1.f;
        m_ambient[3] = 1.f;
        m_emission[3] = 1.f;
        m_shininess = 0.f;
        break;

    case texture:
        // Standard for texture is white diffuse
        for (c = 0; c < 3; c++)
        {
            m_diffuse[c] = 1.f;
            m_specular[c] = 0.f;
            m_ambient[c] = 1.f;
            m_emission[c] = 0.f;
        }

        m_diffuse[3] = 1.f;
        m_specular[3] = 1.f;
        m_ambient[3] = 1.f;
        m_emission[3] = 1.f;
        m_shininess = 0.f;
        break;
    }
}

//
// Name :         CGrComposite::AddMappedRect()
// Description :  This function is a helper function that
//                adds a texture-mapped rectangle to the 
//                composite.
// Parameters :   p_texture - Pointer to texture to use
//                x1, y1 - Lower left corner (in x/y plane)
//                x2, y2 - Upper left corner
//                xd, yd - Texture division factors
//                so, to - S and T texture offsets
// Note :         An x,y point maps to s,t values like this:
//                s = x / xd + so
//                t = y / yd + to
//
void CGrComposite::AddMappedRect(CGrTexture* p_texture, const double x1, const double y1,
                                 const double x2, const double y2, const double xd, const double yd,
                                 const double so, const double to)
{
    const CGrPtr poly = new CGrPolygon;

    poly->Texture(p_texture);
    poly->AddNormal3d(0, 0, 1);
    poly->AddTexVertex3d(x1, y1, 0, x1 / xd + so, y1 / yd + to);
    poly->AddTexVertex3d(x2, y1, 0, x2 / xd + so, y1 / yd + to);
    poly->AddTexVertex3d(x2, y2, 0, x2 / xd + so, y2 / yd + to);
    poly->AddTexVertex3d(x1, y2, 0, x1 / xd + so, y2 / yd + to);

    // Add to this composite.
    Child(poly);
}

//
// Name :         CGrComposite::Box()
// Description :  This is that always useful box creation function. Note that 
//                I created it as a member of CGrComposite so it will just
//                add the box to the composite class.
//                Box creates an upright box.  This is a box with normals on all
//                sides parallel to the coordinate axis.  
// Parameters :   x, y, z - The coordinates of tbe back, lower left corner of the box.
//                dx, dy, dz - The size of the box.
//                p_texture - An option texture to apply to the box.
// Example :      Box(5, 3, 2, 10, 5, 7)
//                This creates an untextured box with the lower, back left corner
//                at 5, 3, 2.  The box extends to x=15, y=8, and z=9.
//                This box will not have a texture applied to it.
//
void CGrComposite::Box(const double x, const double y, const double z, const double dx, const double dy,
    const double dz, CGrTexture* p_texture)
{
    // Front
    CGrPtr poly = new CGrPolygon;
    poly->AddNormal3d(0, 0, 1);
    poly->AddVertex3d(x, y, z + dz);
    poly->AddVertex3d(x + dx, y, z + dz);
    poly->AddVertex3d(x + dx, y + dy, z + dz);
    poly->AddVertex3d(x, y + dy, z + dz);
    if (p_texture)
    {
        poly->Texture(p_texture);
        poly->AddTex2d(0, 0);
        poly->AddTex2d(1, 0);
        poly->AddTex2d(1, 1);
        poly->AddTex2d(0, 1);
    }
    Child(poly);

    // Right
    poly = new CGrPolygon;
    poly->AddNormal3d(1, 0, 0);
    poly->AddVertex3d(x + dx, y, z + dz);
    poly->AddVertex3d(x + dx, y, z);
    poly->AddVertex3d(x + dx, y + dy, z);
    poly->AddVertex3d(x + dx, y + dy, z + dz);
    if (p_texture)
    {
        poly->Texture(p_texture);
        poly->AddTex2d(0, 0);
        poly->AddTex2d(1, 0);
        poly->AddTex2d(1, 1);
        poly->AddTex2d(0, 1);
    }
    Child(poly);

    // Back
    poly = new CGrPolygon;
    poly->AddNormal3d(0, 0, -1);
    poly->AddVertex3d(x + dx, y, z);
    poly->AddVertex3d(x, y, z);
    poly->AddVertex3d(x, y + dy, z);
    poly->AddVertex3d(x + dx, y + dy, z);
    if (p_texture)
    {
        poly->Texture(p_texture);
        poly->AddTex2d(0, 0);
        poly->AddTex2d(1, 0);
        poly->AddTex2d(1, 1);
        poly->AddTex2d(0, 1);
    }
    Child(poly);

    // Left
    poly = new CGrPolygon;
    poly->AddNormal3d(-1, 0, 0);
    poly->AddVertex3d(x, y, z);
    poly->AddVertex3d(x, y, z + dz);
    poly->AddVertex3d(x, y + dy, z + dz);
    poly->AddVertex3d(x, y + dy, z);
    if (p_texture)
    {
        poly->Texture(p_texture);
        poly->AddTex2d(0, 0);
        poly->AddTex2d(1, 0);
        poly->AddTex2d(1, 1);
        poly->AddTex2d(0, 1);
    }
    Child(poly);

    // Top
    poly = new CGrPolygon;
    poly->AddNormal3d(0, 1, 0);
    poly->AddVertex3d(x, y + dy, z + dz);
    poly->AddVertex3d(x + dx, y + dy, z + dz);
    poly->AddVertex3d(x + dx, y + dy, z);
    poly->AddVertex3d(x, y + dy, z);
    if (p_texture)
    {
        poly->Texture(p_texture);
        poly->AddTex2d(0, 0);
        poly->AddTex2d(1, 0);
        poly->AddTex2d(1, 1);
        poly->AddTex2d(0, 1);
    }
    Child(poly);

    // Bottom
    poly = new CGrPolygon;
    poly->AddNormal3d(0, -1, 0);
    poly->AddVertex3d(x, y, z);
    poly->AddVertex3d(x + dx, y, z);
    poly->AddVertex3d(x + dx, y, z + dz);
    poly->AddVertex3d(x, y, z + dz);
    if (p_texture)
    {
        poly->Texture(p_texture);
        poly->AddTex2d(0, 0);
        poly->AddTex2d(1, 0);
        poly->AddTex2d(1, 1);
        poly->AddTex2d(0, 1);
    }
    Child(poly);
}

//
// Name :         CGrComposite::SlantBox()
// Description :  I found this to be rather handy.  It's a function to create a box
//                where the far right side is lifted, but the ends remain vertical.
//
void CGrComposite::SlantBox(const double x, const double y, const double z, const double dx, const double dy,
    const double dz, const double lift)
{
    // Front
    CGrPtr poly = new CGrPolygon;
    poly->AddNormal3d(0, 0, 1);
    poly->AddVertex3d(x, y, z + dz);
    poly->AddVertex3d(x + dx, y + lift, z + dz);
    poly->AddVertex3d(x + dx, y + lift + dy, z + dz);
    poly->AddVertex3d(x, y + dy, z + dz);
    Child(poly);

    // Right
    poly = new CGrPolygon;
    poly->AddNormal3d(1, 0, 0);
    poly->AddVertex3d(x + dx, y + lift, z + dz);
    poly->AddVertex3d(x + dx, y + lift, z);
    poly->AddVertex3d(x + dx, y + lift + dy, z);
    poly->AddVertex3d(x + dx, y + lift + dy, z + dz);
    Child(poly);

    // Back
    poly = new CGrPolygon;
    poly->AddNormal3d(0, 0, -1);
    poly->AddVertex3d(x + dx, y + lift, z);
    poly->AddVertex3d(x, y, z);
    poly->AddVertex3d(x, y + dy, z);
    poly->AddVertex3d(x + dx, y + lift + dy, z);
    Child(poly);

    // Left
    poly = new CGrPolygon;
    poly->AddNormal3d(-1, 0, 0);
    poly->AddVertex3d(x, y, z);
    poly->AddVertex3d(x, y, z + dz);
    poly->AddVertex3d(x, y + dy, z + dz);
    poly->AddVertex3d(x, y + dy, z);
    Child(poly);

    // Top
    poly = new CGrPolygon;
    poly->AddVertex3d(x, y + dy, z + dz);
    poly->AddVertex3d(x + dx, y + lift + dy, z + dz);
    poly->AddVertex3d(x + dx, y + lift + dy, z);
    poly->AddVertex3d(x, y + dy, z);
    poly->ComputeNormal();
    Child(poly);


    // Bottom
    poly = new CGrPolygon;
    poly->AddVertex3d(x, y, z);
    poly->AddVertex3d(x + dx, y + lift, z);
    poly->AddVertex3d(x + dx, y + lift, z + dz);
    poly->AddVertex3d(x, y, z + dz);
    poly->ComputeNormal();
    Child(poly);
}

void CGrComposite::Poly3(const CGrPoint& a, const CGrPoint& b, const CGrPoint& c, CGrTexture* p_texture)
{
    const CGrPtr poly = new CGrPolygon;
    poly->AddVertices3(a, b, c, true);
    Child(poly);

    if (p_texture)
    {
        poly->Texture(p_texture);
        poly->AddTex2d(0, 0);
        poly->AddTex2d(1, 0);
        poly->AddTex2d(0, 1);
    }
}

void CGrComposite::Poly4(const CGrPoint& a, const CGrPoint& b, const CGrPoint& c, const CGrPoint& d,
    CGrTexture* p_texture)
{
    const CGrPtr poly = new CGrPolygon;
    poly->AddVertices4(a, b, c, d, true);
    Child(poly);

    if (p_texture)
    {
        poly->Texture(p_texture);
        poly->AddTex2d(0, 0);
        poly->AddTex2d(1, 0);
        poly->AddTex2d(1, 1);
        poly->AddTex2d(0, 1);
    }
}

void CGrComposite::Pyramid(const double x, const double y, const double z, const double dx, const double dy,
    const double dz, CGrTexture* p_texture)
{
    // Front
    CGrPtr poly = new CGrPolygon;
    poly->AddNormal3d(0, 0, 1);
    poly->AddVertex3d(x, y, z + dz);
    poly->AddVertex3d(x + dx, y, z + dz);
    poly->AddVertex3d(x + (dx / 2), y + dy, z + (dz / 2));
    //poly->AddVertex3d(x, y + dy, z + dz);
    if (p_texture)
    {
        poly->Texture(p_texture);
        poly->AddTex2d(0, 0);
        poly->AddTex2d(1, 0);
        poly->AddTex2d(1, 1);
        poly->AddTex2d(0, 1);
    }
    Child(poly);

    // Right
    poly = new CGrPolygon;
    poly->AddNormal3d(1, 0, 0);
    poly->AddVertex3d(x + dx, y, z + dz);
    poly->AddVertex3d(x + dx, y, z);
    //poly->AddVertex3d(x + dx, y + dy, z);
    poly->AddVertex3d(x + (dx / 2), y + dy, z + (dz / 2));
    if (p_texture)
    {
        poly->Texture(p_texture);
        poly->AddTex2d(0, 0);
        poly->AddTex2d(1, 0);
        poly->AddTex2d(1, 1);
        poly->AddTex2d(0, 1);
    }
    Child(poly);

    // Back
    poly = new CGrPolygon;
    poly->AddNormal3d(0, 0, -1);
    poly->AddVertex3d(x + dx, y, z);
    poly->AddVertex3d(x, y, z);
    poly->AddVertex3d(x + (dx / 2), y + dy, z + (dz / 2));
    //poly->AddVertex3d(x + dx, y + dy, z);
    if (p_texture)
    {
        poly->Texture(p_texture);
        poly->AddTex2d(0, 0);
        poly->AddTex2d(1, 0);
        poly->AddTex2d(1, 1);
        poly->AddTex2d(0, 1);
    }
    Child(poly);

    // Left
    poly = new CGrPolygon;
    poly->AddNormal3d(-1, 0, 0);
    poly->AddVertex3d(x, y, z);
    poly->AddVertex3d(x, y, z + dz);
    poly->AddVertex3d(x + (dx / 2), y + dy, z + (dz / 2));
    //poly->AddVertex3d(x, y + dy, z);
    if (p_texture)
    {
        poly->Texture(p_texture);
        poly->AddTex2d(0, 0);
        poly->AddTex2d(1, 0);
        poly->AddTex2d(1, 1);
        poly->AddTex2d(0, 1);
    }
    Child(poly);

    /*// Top
    poly = new CGrPolygon;
    poly->AddNormal3d(0, 1, 0);
    poly->AddVertex3d(x, y + dy, z + dz);
    poly->AddVertex3d(x + dx, y + dy, z + dz);
    poly->AddVertex3d(x + dx, y + dy, z);
    poly->AddVertex3d(x, y + dy, z);
    if (p_texture)
    {
        poly->Texture(p_texture);
        poly->AddTex2d(0, 0);
        poly->AddTex2d(1, 0);
        poly->AddTex2d(1, 1);
        poly->AddTex2d(0, 1);
    }
    Child(poly);*/

    // Bottom
    poly = new CGrPolygon;
    poly->AddNormal3d(0, -1, 0);
    poly->AddVertex3d(x, y, z);
    poly->AddVertex3d(x + dx, y, z);
    poly->AddVertex3d(x + dx, y, z + dz);
    poly->AddVertex3d(x, y, z + dz);
    if (p_texture)
    {
        poly->Texture(p_texture);
        poly->AddTex2d(0, 0);
        poly->AddTex2d(1, 0);
        poly->AddTex2d(1, 1);
        poly->AddTex2d(0, 1);
    }
    Child(poly);
}

void CGrComposite::Sphere(const double x, const double y, const double z, const double r, CGrTexture* p_texture)
{
    CGrPtr<CGrPolygon> poly;

    double a[] = {1, 0, 0};
    double b[] = {0, 0, -1};
    double c[] = {-1, 0, 0};
    double d[] = {0, 0, 1};
    double e[] = {0, 1, 0};
    double f[] = {0, -1, 0};

    constexpr int recurse = 7;
    SphereFace(recurse, poly, p_texture, r, x, y, z, d, a, e);
    SphereFace(recurse, poly, p_texture, r, x, y, z, a, b, e);
    SphereFace(recurse, poly, p_texture, r, x, y, z, b, c, e);
    SphereFace(recurse, poly, p_texture, r, x, y, z, c, d, e);
    SphereFace(recurse, poly, p_texture, r, x, y, z, a, d, f);
    SphereFace(recurse, poly, p_texture, r, x, y, z, b, a, f);
    SphereFace(recurse, poly, p_texture, r, x, y, z, c, b, f);
    SphereFace(recurse, poly, p_texture, r, x, y, z, d, c, f);
}


void CGrComposite::SphereFace(const int p_recurse, CGrPtr<CGrPolygon>& poly, CGrTexture* p_texture,
    const double p_radius, const double x, const double y, const double z, double* a, double* b, double* c)
{
    if (p_recurse > 1)
    {
        // Compute vectors halfway between the passed vectors 
        double d[3] = {a[0] + b[0], a[1] + b[1], a[2] + b[2]};
        double e[3] = {b[0] + c[0], b[1] + c[1], b[2] + c[2]};
        double f[3] = {c[0] + a[0], c[1] + a[1], c[2] + a[2]};

        Normalize3(d);
        Normalize3(e);
        Normalize3(f);

        SphereFace(p_recurse - 1, poly, p_texture, p_radius, x, y, z, a, d, f);
        SphereFace(p_recurse - 1, poly, p_texture, p_radius, x, y, z, d, b, e);
        SphereFace(p_recurse - 1, poly, p_texture, p_radius, x, y, z, f, e, c);
        SphereFace(p_recurse - 1, poly, p_texture, p_radius, x, y, z, f, d, e);
    }
    else
    {
        poly = new CGrPolygon;
        poly->AddNormal3d(a[0], a[1], a[2]);
        poly->AddVertex3d(x + a[0] * p_radius, y + a[1] * p_radius, z + a[2] * p_radius);

        poly->AddNormal3d(b[0], b[1], b[2]);
        poly->AddVertex3d(x + b[0] * p_radius, y + b[1] * p_radius, z + b[2] * p_radius);

        poly->AddNormal3d(c[0], c[1], c[2]);
        poly->AddVertex3d(x + c[0] * p_radius, y + c[1] * p_radius, z + c[2] * p_radius);

        if (p_texture)
        {
            // What's the texture coordinate for this normal?
            const double tx1 = atan2(a[0], a[2]) / (2. * GR_PI) + 0.5;
            const double ty1 = asin(a[1]) / GR_PI + .5;

            double tx2 = atan2(b[0], b[2]) / (2. * GR_PI) + 0.5;
            const double ty2 = asin(b[1]) / GR_PI + .5;
            // Test for this coordinate on the other side of the
            // texture from the first coordinate.
            if (tx2 < 0.75 && tx1 > 0.75)
                tx2 += 1.0;
            else if (tx2 > 0.75 && tx1 < 0.75)
                tx2 -= 1.0;

            double tx3 = atan2(c[0], c[2]) / (2. * GR_PI) + 0.5;
            const double ty3 = asin(c[1]) / GR_PI + .5;
            // Test for this coordinate on the other side of the
            // texture from the first coordinate.
            if (tx3 < 0.75 && tx1 > 0.75)
                tx3 += 1.0;
            else if (tx3 > 0.75 && tx1 < 0.75)
                tx3 -= 1.0;

            poly->Texture(p_texture);
            poly->AddTex2d(tx2, ty2);
            poly->AddTex2d(tx1, ty1);
            poly->AddTex2d(tx3, ty3);
        }
    }
}
