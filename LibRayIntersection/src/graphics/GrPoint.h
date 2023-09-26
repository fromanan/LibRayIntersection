//
// Name :         GrPoint.h
// Description :  Classes and operators for homogeneous coordinates.
// Author :       Charles B. Owen
// Version :      1.00  2-03-01 Declared version number
//                1.01  3-23-04 Visual Studio 2003 bug fixes
//                              NOOPENGL option.
//                1.02  3-11-07 CGrPoint allows for direct access to X, Y, Z, W
//                              Added CGrPoint::MemberMultiply3
//                              Added several additional set functions
//
// Notice :       This class has no associated .cpp file.  All functions are inline.
//

#pragma once

#include "grafx.h"

// class CGrPoint
// Class for a graphics point or vector.  
// I'm going to make this 4D since we'll need
// that later, anyway.

class CGrPoint
{
public:
    CGrPoint() = default;
    CGrPoint(const double x, const double y, const double z=0, const double w=1.) {m[0] = x;  m[1] = y;  m[2] = z;  m[3] = w;}
    CGrPoint(const float *p) {m[0] = p[0];  m[1] = p[1];  m[2] = p[2];  m[3] = p[3];}
    CGrPoint(const double *p) {m[0] = p[0];  m[1] = p[1];  m[2] = p[2];  m[3] = p[3];}
    CGrPoint(const CGrPoint &p) {m[0]=p.m[0];  m[1]=p.m[1];  m[2]=p.m[2];  m[3]=p.m[3];} 

    CGrPoint &operator=(const CGrPoint &p) {m[0]=p.m[0];  m[1]=p.m[1];  m[2]=p.m[2];  m[3]=p.m[3]; return *this;}

    double &X() {return m[0];}
    double &Y() {return m[1];}
    double &Z() {return m[2];}
    double &W() {return m[3];}
    const double &X() const {return m[0];}
    const double &Y() const {return m[1];}
    const double &Z() const {return m[2];}
    const double &W() const {return m[3];}

    double X(const double p) {return m[0] = p;}
    double Y(const double p) {return m[1] = p;}
    double Z(const double p) {return m[2] = p;}
    double W(const double p) {return m[3] = p;}

    void Set(const double x, const double y, const double z, const double w=1.) {m[0] = x;  m[1] = y;  m[2] = z;  m[3] = w;}
    void Set(const double *p) {m[0] = p[0];  m[1] = p[1];  m[2] = p[2];  m[3] = p[3];}
    void Set(const float *p) {m[0] = p[0];  m[1] = p[1];  m[2] = p[2];  m[3] = p[3];}

    CGrPoint Perp2() const { return {-m[1], m[0], 0}; }

#ifndef NOOPENGL
    void glVertex() const {glVertex4dv(m);}
    void glNormal() const {glNormal3dv(m);}
    void glTexVertex() const {glTexCoord2dv(m);}
#endif

    CGrPoint operator -(const CGrPoint &b) const { return {m[0]-b.m[0], m[1]-b.m[1], m[2]-b.m[2], m[3]-b.m[3]}; }
    CGrPoint operator -() const { return {-m[0], -m[1], -m[2], -m[3]}; }
    CGrPoint operator +(const CGrPoint &b) const { return {m[0]+b.m[0], m[1]+b.m[1], m[2]+b.m[2], m[3]+b.m[3]}; }
    CGrPoint &operator -=(const CGrPoint &b) {m[0]-=b.m[0]; m[1]-=b.m[1]; m[2]-=b.m[2]; m[3]-=b.m[3]; return *this;}
    CGrPoint &operator +=(const CGrPoint &b) {m[0]+=b.m[0]; m[1]+=b.m[1]; m[2]+=b.m[2]; m[3]+=b.m[3]; return *this;}
    CGrPoint& operator *=(const double n) { m[0] *= n, m[1] *= n, m[2] *= n, m[3] *= n; return *this; }
    CGrPoint& operator /=(const double n) { m[0] /= n, m[1] /= n, m[2] /= n, m[3] /= n; return *this; }
    CGrPoint operator *(const CGrPoint& b) { m[0] *= b.m[0]; m[1] *= b.m[1]; m[2] *= b.m[2]; m[3] *= b.m[3]; return *this; } //< Default to Hadamard Multiplication
    CGrPoint operator *(const double n) const { return { m[0]*n, m[1]*n, m[2]*n, m[3]*n }; }
    CGrPoint operator /(const double n) const { return { m[0]/n, m[1]/n, m[2]/n, m[3]/n }; }
    operator const double *() const {return m;}
    operator double *() {return m;}
    void Normalize3() { const double l = Length3();  m[0] /= l;  m[1] /= l;  m[2] /= l;}
    double Length3() const {return sqrt(m[0]*m[0] + m[1]*m[1] + m[2]*m[2]);}
    double LengthSquared3() const {return m[0]*m[0] + m[1]*m[1] + m[2]*m[2];}

    void Minimize(const CGrPoint &p) {for (int i=0;  i<4;  i++) m[i] = p.m[i] < m[i] ? p.m[i] : m[i];}
    void Maximize(const CGrPoint &p) {for (int i=0;  i<4;  i++) m[i] = p.m[i] > m[i] ? p.m[i] : m[i];}

    void WeightedAdd3(const CGrPoint &p, const double w) {m[0] += p.m[0] * w;  m[1] += p.m[1] * w;  m[2] += p.m[2] * w;} 
    CGrPoint &MemberMultiply3(const CGrPoint &p) {m[0] *= p.m[0];  m[1] *= p.m[1];  m[2] *= p.m[2];  return *this;}

    vec3 ToVec3() const { return vec3(m[0], m[1], m[2]); }

private:
    double m[4];
};

// Normalize a vector. 
inline CGrPoint Normalize3(const CGrPoint &p)
{
   CGrPoint a(p);
   a.Normalize3();
   return a;
}

// Cross product
inline CGrPoint Cross3(const CGrPoint &a, const CGrPoint &b)
{
   return { a.Y()*b.Z() - a.Z()*b.Y(), a.Z()*b.X() - a.X()*b.Z(), a.X()*b.Y() - a.Y()*b.X(), 0 };
}

// Dot product
inline double Dot3(const CGrPoint &a, const CGrPoint &b)
{
   return a.X() * b.X() + a.Y() * b.Y() + a.Z() * b.Z();
}

// Dot product
inline double Dot2(const CGrPoint &a, const CGrPoint &b)
{
   return a.X() * b.X() + a.Y() * b.Y();
}

// Distance between two points
inline double Distance(const CGrPoint &a, const CGrPoint &b)
{
    return sqrt( (a.X() - b.X()) * (a.X() - b.X()) +
                (a.Y() - b.Y()) * (a.Y() - b.Y()) +
                (a.Z() - b.Z()) * (a.Z() - b.Z()));
}

inline CGrPoint Hadamard(const CGrPoint &a, const CGrPoint &b)
{
    return { a.X() * b.X(), a.Y() * b.Y(), a.Z() * b.Z() };
}

inline ostream& operator<<(ostream& str, const CGrPoint &p)
{
	str << p.X() << " " << p.Y() << " " << p.Z() << " " << p.W(); return str;
}

inline shared_ptr<CGrPoint> BilinearInterpolation(BYTE** const img, const int width, const int height, const int c,
    const int r)
{
    // Invalid Input
    if (c < 0 || c >= height || r < 0 || r >= width)
        return nullptr;

    double pixel[3]{ 0., 0., 0. };

    // BiLerp Each Channel
    for (int i = 0; i < 2; i++)
    {
        // Bounds Checking
        double point = img[r][c * 3 + i];
        double up = r + 1 >= width ? img[r][c * 3 + i] : img[r + 1][c * 3 + i];
        double corner = r + 1 >= width ? c + 1 >= height ? img[r][c * 3 + i] : img[r][(c + 1) * 3 + i] : img[r + 1][(c + 1) * 3 + i];
        double right = c + 1 >= height ? img[r][c * 3 + i] : img[r][(c + 1) * 3 + i];

        point /= 255.;
        up /= 255.;
        corner /= 255.;
        right /= 255.;

        // Calculate Values
        const double fx = fmod(c * 3 + i, 1);
        const double ct = up * (1. - fx) + corner * fx;
        const double cb = point * (1. - fx) + right * fx;
        const double fy = fmod(r, 1);

        // Apply Calculation
        pixel[i] = ct * fy + cb * (1. - fy);
    }

    return make_shared<CGrPoint>(pixel);
}
