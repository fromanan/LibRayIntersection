#pragma once

// C Libraries
#include <afxwin.h>
#include <stdio.h>

#include <cmath>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <iomanip>

using namespace std;

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