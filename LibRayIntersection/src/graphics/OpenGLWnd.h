//
// Name :          OpenGLWnd.h
// Description :   Header file for COpenGLWnd OpenGL superclass.
// Documentation : See OpenGLWnd.cpp
// Author :        Charles B. Owen
// Modification:   Jan 12, 19 - Y. Tong
// Modification:   Sep 25, 23 - Tony Froman
//

#pragma once

#include "grafx.h"

/////////////////////////////////////////////////////////////////////////////
// COpenGLWnd window

class COpenGLWnd : public CWnd
{
public:
    COpenGLWnd();

    // Operations
    virtual void OnGLDraw(CDC *pDC);

    // Overrides
    // ClassWizard generated virtual function overrides
protected:
    BOOL PreCreateWindow(CREATESTRUCT& cs) override;

    // Implementation
public:
    void OnSaveImage();
    void GetSize(int &p_width, int &p_height) const;
    bool ObtainPixels(GLbyte **&p_pixels);
    ~COpenGLWnd() override;

    HGLRC HGLRc() const {return m_hrc;}

    // Generated message map functions
protected:
    void SetDoubleBuffer(bool p_doublebuffer);
    bool GetDoubleBuffer() const {return m_doublebuffer;}

    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnPaint();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    
    DECLARE_MESSAGE_MAP()

private:
    int FormattedErrorAfxMsgBox(const char *p_msg);

    bool m_created;
    bool m_doublebuffer;
    BOOL CreateRGBPalette(HDC hDC);

    HGLRC m_hrc;
    CPalette* m_pPal ;		//Palette

   	//
    // Support for generating RGB color palette
    //
    static unsigned char ComponentFromIndex(int i, UINT nbits, UINT shift) ;

    static unsigned char   m_oneto8[2];
    static unsigned char   m_twoto8[4];
    static unsigned char   m_threeto8[8];
    static int             m_defaultOverride[13];
    static PALETTEENTRY    m_defaultPalEntry[20];
};