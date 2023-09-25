#pragma once

// C Libraries
#include <afxwin.h>
#include <stdio.h>

// C++ Libraries (STL)
#include <cmath>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <cassert>

using namespace std;

// Graphics Libraries
#define GLEW_STATIC
#include <glew.h>
#include <glext.h>
#include <wglext.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <glm.hpp>
#include <constants.hpp>
#include <matrix_transform.hpp>
#include <type_ptr.hpp>

using namespace glm;

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

/*#ifndef NOOPENGL
#include <GL/gl.h>
#endif*/

///
/// CONSTANTS
///
const double GR_PI = 3.1415926535897932384626433832795;
const double GR_PI2 = 2. * GR_PI;
const double GR_RTOD = 180. / GR_PI;      // Converts radians to degrees
const double GR_DTOR = GR_PI / 180.;      // Converts degrees to radians