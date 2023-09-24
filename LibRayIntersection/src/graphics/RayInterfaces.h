#pragma once

//
// Name :         RayInterfaces.h
// Description :  Interface classes to pass to the ray intersection system.
//                Allows the ray intersection system to be independent
//                of the material and texture classes.
// Author :       Charles B. Owen
// Version :       2-17-2011 1.00 New interface classes replace anonymous references
//

#ifndef LIBRIEXPORT
#ifdef LIBRIDLL
#define LIBRIEXPORT  __declspec( dllexport )
#else
#define LIBRIEXPORT  __declspec( dllimport )
#endif
#endif

//! Interface class for materials in the Ray Intersection system.
/*! Any material class supplied to the ray intersection system must be
    derived from this interface. The interface exists mainly as a
    safe way of pointing to materials. The system does not look at the 
    contents of the class at all. */
class LIBRIEXPORT IMaterial
{
public:
    IMaterial();
    virtual ~IMaterial();
};

//! Interface class for textures in the Ray Intersection system.
/*! Any texture class supplied to the ray intersection system must be
    derived from this interface. The interface exists mainly as a
    safe way of pointing to materials. The system does not look at the 
    contents of the class at all. */
class LIBRIEXPORT ITexture
{
public:
    ITexture();
    virtual ~ITexture();
};
