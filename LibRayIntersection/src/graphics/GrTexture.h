//
// Name :         GrTexture.h
// Description :  Header for CTexture, texture image class for OpenGL.
// Notice :       This is a modified version designed to work with the scene graph
//                system.  We can have an actual texture in a scene graph.
// Version :      See Texture.cpp
//

#pragma once

#include "grafx.h"

#include "GrObject.h"
#include <fstream>
#include <valarray>

class CGrTexture : public CGrObject
{
public:
    CGrTexture();
    CGrTexture(const CGrTexture &p_img);
    ~CGrTexture() override;

    void glRender() override;
    void Render(CGrRenderer *p_renderer) override;

    GLuint TexName();

    bool LoadFile(const _TCHAR *lpszPathName);
    bool LoadMemory(const BYTE *image, int width, int height, 
                    int colpitch, int rowpitch, bool repeatS, bool repeatT, bool transparency);

    void Fill(int r, int g, int b) const;
    void Set(int x, int y, int r, int g, int b) const;
    void SetSize(int p_x, int p_y);
    void SameSize(const CGrTexture &p_img);
    void Copy(const CGrTexture &p_img);
    bool Empty() const {return m_width <= 0 || m_height <= 0;}
    CGrTexture &operator=(const CGrTexture &p_img);

    BYTE *operator[](const int i) {return m_image[i];}
    const BYTE *operator[](const int i) const {return m_image[i];}
    BYTE *Row(const int i) {return m_image[i];}
    const BYTE *Row(const int i) const {return m_image[i];}

    int Width() const {return m_width;}
    int Height() const {return m_height;}
    BYTE *ImageBits() const {return m_image[0];}

    shared_ptr<CGrPoint> Sample(const double u, const double v, bool smoothResult = false) const
    {
	    const auto c = static_cast<int>(u * Width());
	    const auto r = static_cast<int>(v * Height());

    	// Bounds Check
    	if (r >= m_height || r < 0 || (c >= m_width || c < 0))
            return nullptr;
    	
        return make_shared<CGrPoint>
    	(
            static_cast<double>(m_image[r][c * 3 + 0]) / 255.,
            static_cast<double>(m_image[r][c * 3 + 1]) / 255.,
            static_cast<double>(m_image[r][c * 3 + 2]) / 255.
		);
    }

    shared_ptr<CGrPoint> Sample(CGrPoint texCoord) const
    {
        if (texCoord == nullptr) return nullptr;
	    return Sample(texCoord.X(), texCoord.Y());
    }

private:
    bool ReadDIBFile(std::istream &file);
    bool ReadPPMFile(std::istream &file);

    bool    m_initialized;
    bool    m_mipmap;
    GLuint  m_texname;
    int     m_height;
    int     m_width;
    BYTE  **m_image;
};
