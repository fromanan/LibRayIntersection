//
// Name :         Texture.h
// Description :  Header for CTexture, texture image class for OpenGL.
// Version :      See Texture.cpp
//

#pragma once

#include "grafx.h"

class CTexture  
{
public:
    CTexture();
    CTexture(const CTexture &p_img);
    virtual ~CTexture();

    GLuint TexName();
    GLuint MipTexName();

    void Fill(int r, int g, int b) const;
    void Set(int x, int y, int r, int g, int b) const;
    void SetSize(int p_x, int p_y);
    void SameSize(const CTexture &p_img);
    void Copy(const CTexture &p_img);
    bool LoadFile(LPCTSTR lpszPathName);
    BOOLEAN Empty() const {return m_width <= 0 || m_height <= 0;}
    CTexture &operator=(const CTexture &p_img);
    void SetAutoBmp(const bool s) {m_autobmp = s;}

    BYTE *operator[](const int i) {return m_image[i];}
    const BYTE *operator[](const int i) const {return m_image[i];}
    BYTE *Row(const int i) {return m_image[i];}
    const BYTE *Row(const int i) const {return m_image[i];}

    int Width() const {return m_width;}
    int Height() const {return m_height;}
    BYTE *ImageBits() const {return m_image[0];}

private:
    bool m_mipinitialized;
    bool m_initialized;
    bool m_autobmp;         // Force suffix to .bmp, no matter what the actual type.
    GLuint m_texname;
    GLuint m_miptexname;
    bool ReadDIBFile(std::istream &file);
    bool ReadPPMFile(std::istream &file);
    int m_height;
    int m_width;
    BYTE ** m_image;
};
