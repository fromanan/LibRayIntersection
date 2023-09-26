//
// Name :         accjitter.h
// Description :  Jittering functions from the OpenGL Red book.
//

#include <stdafx.h>
#include <cmath>
#include "accjitter.h"

GLdouble PI_ = 3.14159265358979323846;

void accFrustrum(const GLdouble left, const GLdouble right, const GLdouble bottom, const GLdouble top,
                 const GLdouble zNear, const GLdouble zFar, const GLdouble pixdx, const GLdouble pixdy,
                 const GLdouble eyedx, const GLdouble eyedy, const GLdouble focus)
{
   GLint viewport[4];

   // Get the current viewport
   glGetIntegerv(GL_VIEWPORT, viewport);

   const GLdouble xwsize = right - left;
   const GLdouble ywsize = top - bottom;

   const GLdouble dx = -(pixdx * xwsize / static_cast<GLdouble>(viewport[2]) + eyedx * zNear / focus);
   const GLdouble dy = -(pixdy * ywsize / static_cast<GLdouble>(viewport[3]) + eyedy * zNear / focus);

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glFrustum(left + dx, right + dx, bottom + dy, top + dy, zNear, zFar);
   glMatrixMode(GL_MODELVIEW);

   glLoadIdentity();
   glTranslated(-eyedx, -eyedy, 0.0);
}

void accPerspective(const GLdouble fovy, const GLdouble aspect, const GLdouble zNear, const GLdouble zFar,
                    const GLdouble pixdx, const GLdouble pixdy, const GLdouble eyedx, const GLdouble eyedy,
                    const GLdouble focus)
{
   const GLdouble fov2 = fovy * PI_ / 180. / 2.0;

   const GLdouble top = zNear * tan(fov2);
   const GLdouble bottom = -top;
   const GLdouble right = top * aspect;
   const GLdouble left = - right;

   accFrustrum(left, right, bottom, top, zNear, zFar, pixdx, pixdy, eyedx, eyedy, focus);
}

