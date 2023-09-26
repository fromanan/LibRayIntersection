//////////////////////////////////////////////////////////////////////
//
// Name :         GrTransform.cpp
// Description :  Implementation file for CGrTransform.  This class implements
//                a 4x4 transformation matrix.  
// Version :      2.00 8-25-04 Declared version number.
//							   Modified interface in several places.
//

#include "stdafx.h"

#include "GrTransform.h"

void CGrTransform::SetIdentity()
{
   m[0][0] = 1;  m[0][1] = 0;  m[0][2] = 0;  m[0][3] = 0;
   m[1][0] = 0;  m[1][1] = 1;  m[1][2] = 0;  m[1][3] = 0;
   m[2][0] = 0;  m[2][1] = 0;  m[2][2] = 1;  m[2][3] = 0;
   m[3][0] = 0;  m[3][1] = 0;  m[3][2] = 0;  m[3][3] = 1;
}

void CGrTransform::SetZero()
{
   m[0][0] = 0;  m[0][1] = 0;  m[0][2] = 0;  m[0][3] = 0;
   m[1][0] = 0;  m[1][1] = 0;  m[1][2] = 0;  m[1][3] = 0;
   m[2][0] = 0;  m[2][1] = 0;  m[2][2] = 0;  m[2][3] = 0;
   m[3][0] = 0;  m[3][1] = 0;  m[3][2] = 0;  m[3][3] = 0;
}

//
// Rotation matrices.  There are two versions of each
// of the standard matrices, a version that accepts
// an angle IN DEGREES and a version that accepts
// the cosine and sine of the rotation angle.
//
CGrTransform &CGrTransform::SetRotateX(const double r)
{
   const double rr = r * GR_DTOR;
   const double cr = cos(rr);
   const double sr = sin(rr);

   m[0][0] = 1;   m[0][1] = 0;      m[0][2] = 0;   m[0][3] = 0;
   m[1][0] = 0;   m[1][1] = cr;     m[1][2] = -sr; m[1][3] = 0;
   m[2][0] = 0;   m[2][1] = sr;     m[2][2] = cr;  m[2][3] = 0;
   m[3][0] = 0;   m[3][1] = 0;      m [3][2] = 0;  m[3][3] = 1;

   return *this;
}

CGrTransform &CGrTransform::SetRotateX(const double cr, const double sr)
{
   m[0][0] = 1;   m[0][1] = 0;      m[0][2] = 0;    m[0][3] = 0;
   m[1][0] = 0;   m[1][1] = cr;     m[1][2] = -sr;  m[1][3] = 0;
   m[2][0] = 0;   m[2][1] = sr;     m[2][2] = cr;   m[2][3] = 0;
   m[3][0] = 0;   m[3][1] = 0;      m[3][2] = 0;    m[3][3] = 1;

   return *this;
}

CGrTransform &CGrTransform::SetRotateY(const double r)
{
   const double rr = r * GR_DTOR;
   const double cr = cos(rr);
   const double sr = sin(rr);

   m[0][0] = cr;     m[0][1] = 0;   m[0][2] = sr;   m[0][3] = 0;
   m[1][0] = 0;      m[1][1] = 1;   m[1][2] = 0;    m[1][3] = 0;
   m[2][0] = -sr;    m[2][1] = 0;   m[2][2] = cr;   m[2][3] = 0;
   m[3][0] = 0;      m[3][1] = 0;   m[3][2] = 0;    m[3][3] = 1;

   return *this;
}

CGrTransform & CGrTransform::SetRotateY(const double cr, const double sr)
{
   m[0][0] = cr;     m[0][1] = 0;   m[0][2] = sr;   m[0][3] = 0;
   m[1][0] = 0;      m[1][1] = 1;   m[1][2] = 0;    m[1][3] = 0;
   m[2][0] = -sr;    m[2][1] = 0;   m[2][2] = cr;   m[2][3] = 0;
   m[3][0] = 0;      m[3][1] = 0;   m[3][2] = 0;    m[3][3] = 1;

   return *this;
}

CGrTransform &CGrTransform::SetRotateZ(const double r)
{
   const double rr = r * GR_DTOR;
   const double cr = cos(rr);
   const double sr = sin(rr);

   m[0][0] = cr;     m[0][1] = -sr;  m[0][2] = 0;   m[0][3] = 0;
   m[1][0] = sr;     m[1][1] = cr;   m[1][2] = 0;   m[1][3] = 0;
   m[2][0] = 0;      m[2][1] = 0;    m[2][2] = 1;   m[2][3] = 0;
   m[3][0] = 0;      m[3][1] = 0;    m[3][2] = 0;   m[3][3] = 1;

   return *this;
}

CGrTransform & CGrTransform::SetRotateZ(const double cr, const double sr)
{
   m[0][0] = cr;     m[0][1] = -sr;  m[0][2] = 0;   m[0][3] = 0;
   m[1][0] = sr;     m[1][1] = cr;   m[1][2] = 0;   m[1][3] = 0;
   m[2][0] = 0;      m[2][1] = 0;    m[2][2] = 1;   m[2][3] = 0;
   m[3][0] = 0;      m[3][1] = 0;    m[3][2] = 0;   m[3][3] = 1;

   return *this;
}

//
// Name :         CGrTransform::SetRotate()
// Description :  Create a rotation matrix that will rotate
//                by angle r around vector v.  Note that
//                r is in degrees.
//
CGrTransform &CGrTransform::SetRotate(const double r, const CGrPoint& v)
{
   const double rr = r * GR_DTOR;
   const double c = cos(rr);
   const double s = sin(rr);
   const double t = 1. - c;

   const double l = v.Length3();
   const double x = v.X() / l;
   const double y = v.Y() / l;
   const double z = v.Z() / l;

   m[0][0] = t * x * x + c;
   m[0][1] = t * x * y - s * z;
   m[0][2] = t * x * z + s * y;
   m[0][3] = 0;

   m[1][0] = t * x * y + s * z;
   m[1][1] = t * y * y + c;
   m[1][2] = t * y * z - s * x;
   m[1][3] = 0;

   m[2][0] = t * x * z - s * y;
   m[2][1] = t * y * z + s * x;
   m[2][2] = t * z * z + c;
   m[2][3] = 0;

   m[3][0] = 0;
   m[3][1] = 0;
   m[3][2] = 0;
   m[3][3] = 1;

   return *this;
}

CGrTransform & CGrTransform::SetRotate(const CGrPoint &x, const CGrPoint &y, const CGrPoint &z)
{
   m[0][0] = x.X();  m[0][1] = x.Y();  m[0][2] = x.Z();  m[0][3] = 0;
   m[1][0] = y.X();  m[1][1] = y.Y();  m[1][2] = y.Z();  m[1][3] = 0;
   m[2][0] = z.X();  m[2][1] = z.Y();  m[2][2] = z.Z();  m[2][3] = 0;
   m[3][0] = 0;      m[3][1] = 0;      m[3][2] = 0;      m[3][3] = 1;

   return *this;
}

inline void _swap(double &a, double &b)
{
   const double t = a;
   a = b;
   b = t;
}

CGrTransform & CGrTransform::Transpose()
{
   _swap(m[0][1], m[1][0]);
   _swap(m[0][2], m[2][0]);
   _swap(m[0][3], m[3][0]);
   _swap(m[1][2], m[2][1]);
   _swap(m[1][3], m[3][1]);
   _swap(m[2][3], m[3][2]);

   return *this;
}

//
// Name :         CGrTransform::SetLookAt() 
// Description :  Construct a LookAt matrix.  This is a rotation and
//                translation that will put the eye at 0,0,0, up in the
//                direction of the Y axis and looking down the -Z axis.
//
void CGrTransform::SetLookAt(const double ex, const double ey, const double ez,
                             const double cx, const double cy, const double cz,
                             const double ux, const double uy, const double uz)
{
   const CGrPoint eye(ex, ey, ez);
   const CGrPoint center(cx, cy, cz);
   const CGrPoint up(ux, uy, uz);
   
   CGrPoint cameraz = Normalize3(eye - center);
   CGrPoint camerax = Normalize3(Cross3(up, cameraz));
   CGrPoint cameray = Cross3(cameraz, camerax);

   CGrTransform r;
   r[0][0] = camerax.X();  r[0][1]= camerax.Y();   r[0][2] = camerax.Z();  r[0][3] = 0;
   r[1][0] = cameray.X();  r[1][1] = cameray.Y();  r[1][2] = cameray.Z();  r[1][3] = 0;
   r[2][0] = cameraz.X();  r[2][1] = cameraz.Y();  r[2][2] = cameraz.Z();  r[2][3] = 0;
   r[3][0]               = r[3][1]               = r[3][2] = 0.;           r[3][3] = 1.0;

   CGrTransform t;
   t.SetTranslate(-ex, -ey, -ez);

   *this = r * t;
}

//
// Name :         CGrTransform::SetAffineInverse()
// Description :  Set this matrix to the inverse of another matrix,
//                assuming the other matrix is an affine transform
//
CGrTransform &CGrTransform::SetAffineInverse(const CGrTransform &fm)
{
    // First compute the inverse of the upper left 3x3 submatrix
    double adjoint[3][3];

    adjoint[0][0] =  (fm.M(1, 1) * fm.M(2, 2) - fm.M(1, 2) * fm.M(2, 1));
    adjoint[1][0] = -(fm.M(1, 0) * fm.M(2, 2) - fm.M(1, 2) * fm.M(2, 0));
    adjoint[2][0] =  (fm.M(1, 0) * fm.M(2, 1) - fm.M(1, 1) * fm.M(2, 0));
    adjoint[0][1] = -(fm.M(0, 1) * fm.M(2, 2) - fm.M(0, 2) * fm.M(2, 1));
    adjoint[1][1] =  (fm.M(0, 0) * fm.M(2, 2) - fm.M(0, 2) * fm.M(2, 0));
    adjoint[2][1] = -(fm.M(0, 0) * fm.M(2, 1) - fm.M(0, 1) * fm.M(2, 0));
    adjoint[0][2] =  (fm.M(0, 1) * fm.M(1, 2) - fm.M(0, 2) * fm.M(1, 1));
    adjoint[1][2] = -(fm.M(0, 0) * fm.M(1, 2) - fm.M(0, 2) * fm.M(1, 0));
    adjoint[2][2] =  (fm.M(0, 0) * fm.M(1, 1) - fm.M(0, 1) * fm.M(1, 0));

    // Now compute the determinate
    // It is the sum of the products of the cofactors and the elements of one 
    // row of the matrix:

    double det = fm.M(0, 0) * adjoint[0][0] + fm.M(0, 1) * adjoint[1][0] + fm.M(0, 2) * adjoint[2][0];
    if (det == 0)
        det = 0.000001;

    // Put in as the rotation part:

    M(0, 0) = adjoint[0][0] / det;
    M(0, 1) = adjoint[0][1] / det;
    M(0, 2) = adjoint[0][2] / det;
    M(1, 0) = adjoint[1][0] / det;
    M(1, 1) = adjoint[1][1] / det;
    M(1, 2) = adjoint[1][2] / det;
    M(2, 0) = adjoint[2][0] / det;
    M(2, 1) = adjoint[2][1] / det;
    M(2, 2) = adjoint[2][2] / det;
    M(3, 0) = M(3, 1) = M(3, 2) = 0.;
    M(3, 3) = fm.M(3, 3);

    const double x = -fm.M(0, 3);
    const double y = -fm.M(1, 3);
    const double z = -fm.M(2, 3);

    M(0, 3) = x * M(0, 0) + y * M(0, 1) + z * M(0, 2);
    M(1, 3) = x * M(1, 0) + y * M(1, 1) + z * M(1, 2);
    M(2, 3) = x * M(2, 0) + y * M(2, 1) + z * M(2, 2);

    return *this;
}