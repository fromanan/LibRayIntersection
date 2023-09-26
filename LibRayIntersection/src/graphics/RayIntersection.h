#pragma once

//
// Name :         RayIntersection.h
// Description :  Header for CRayIntersection
//                Polygon ray intersection support class/DLL.
//                Implements kdTree fast ray intersection algorithm.
// Author :       Charles B. Owen
// Version :       3-13-2007 2.00 Moved to DLL
//                 4-11-2007 2.01 Fixed problems related to coincident vertices
//                 2-27-2011 2.02 CGrPoint changes to CGrVector
//                                New IMaterial and ITexture interfaces
//

#ifndef LIBRIEXPORT
#ifdef LIBRIDLL
#define LIBRIEXPORT  __declspec( dllexport )
#else
#define LIBRIEXPORT  __declspec( dllimport )
#endif
#endif

#include "grafx.h"

#include "GrVector.h"
#include "RayInterfaces.h"

//! Ray Intersection testing library

//! The libRayIntersection library provides classes to support ray intersection
//! tests with triangles and polygons. 
//!
//! The libRayIntersection libary is available from 
//! <a href="https://www.cse.msu.edu/~cse472/secure/rayintersection.zip">
//! rayintersection.zip</a>
//! 
//! To use:
//!
//! -# Call Initialize() to initialize the system.
//! -# Call Material() to set a pointer to the current material property
//! -# Call Texture() to set a pointer to the current texture (optional)
//! -# Add polygons or triangles to the system:
//!     -# Call PolygonBegin() or TriangleBegin()
//!     -# Call Vertex() to add vertices for the polygon
//!     -# Call Normal() to specify a normal for the polygon
//!     -# Call TexVertex() to specify a vertex for the polygon
//!     -# Call PolygonEnd() or TriangleEnd()
//! -# Call LoadingComplete()
//! -# Call Intersect() to test for intersections
//! -# Call IntersectInfo() to get intersection information for rendering
//!
//! Notice:  CRayIntersection is NOT thread safe. Do not use with more than one
//! thread. An easy way to support multithreading is to create a separate copy of
//! the class for each thread.


// Anonymous reference to the class that does all of the actual work
class CRayIntersectionD;

//
// class CRay
// Describes a simple ray tracer ray.
//

//! Class that supports a Ray
class LIBRIEXPORT CRay
{
public:
    //! Constructor that initializes the ray.
    /*! \param o Ray origin.
        \param d Ray direction. */
    CRay(const CGrVector &o, const CGrVector &d) {m_o=o;  m_d=d;}

    //! Default constructor. Does not initialize the ray.
    CRay() {}

    //! Copy constructor.
    /*! \param r Ray to copy */
    CRay(const CRay &r) {m_o = r.m_o; m_d = r.m_d;}

    //! Ray origin.
    /*! Allows access to the origin of the ray as a CGrVector object.
        \return Reference to the origin of the ray */
    const CGrVector &Origin() const {return m_o;}

    //! Ray origin components.
    /*! Allows access to the origin of the ray as a 
        component by dimension.
        \param d Dimension to access, 0 - 2
        \return Component of the origin of the ray */
    double Origin(const int d) const {return m_o[d];}

    //! Ray direction.
    /*! Allows access to the direction of the ray as a CGrVector object.
        \return Reference to the direction of the ray */
    const CGrVector &Direction() const {return m_d;}

    //! Ray direction components.
    /*! Allows access to the direction of the ray as a 
        component by dimension.
        \param d Dimension to access, 0 - 2
        \return Component of the direction of the ray */
    double Direction(const int d) const {return m_d[d];}

    //! Assignment operator.
    CRay &operator=(const CRay &r) {m_o = r.m_o; m_d = r.m_d; return *this;}

    //! Determine a point on a ray given the t value.
    /*! Given a t value, this function computes a point on the ray.
        \param t t value to use.
        \return Point on the ray */
    CGrVector PointOnRay(const double t) const {return m_o + m_d * t;}

private:
    CGrVector    m_o;
    CGrVector    m_d;
};

//! The Ray Intersection class
class LIBRIEXPORT CRayIntersection  
{
public:
    //! Default constructor
	CRayIntersection();

    //! Destructor.
	virtual ~CRayIntersection();

    //! Initialize the system prior to insertion of polygons or triangles.
    void Initialize() const;

    //! Indication that all polygons or triangles have been loaded.
	void LoadingComplete() const;

    //! Begin polygon insertion.
    /*! Begin insertion of a polygon into the system. 
        
        Polygons with 3 vertices are automatically treated as triangles. */
	void PolygonBegin() const;

    //! End polygon insertion. 
    /*! This function indicates the end of a polygon. Subsequent calls to
        set vertices, normals, or texture coordinates are ignored until the 
        next call to PolygonBegin or TriangleBegin. */
	void PolygonEnd() const;

    //! Begin triangle insertion.
    /*! This function is called to indicate the beginning of the process of
        entering triangles. Only one triangle can be entered at a time prior 
        to the call to TriangleEnd(). */
    void TriangleBegin() const;

    //! End triangle insertion.
    /*! This function indicates the end of a triangle. Subsequent calls to
        set vertices, normals, or texture coordinates are ignored until the 
        next call to PolygonBegin or TriangleBegin. */
    void TriangleEnd() const;

    //! Set the current material.
    /*! Sets a current pointer to a material. This material will be associated
        with subsequent triangles and polygons. The pointer is persistent and
        does not have to be reset for each triangle or polygon. This is a 
        pointer to an interface to materials. Material classes should be derived
        from this interface.
        \param material The material pointer to set. */
	void Material(IMaterial *material) const;

    //! Set the current texture.
    /*! Sets a current pointer to a texture. This texture will be associated
        with subsequent triangles and polygons. The pointer is persistent and
        does not have to be reset for each triangle or polygon. This is a 
        pointer to an interface to texture. Texture classes should be derived
        from this interface. If no texture is supplied, call the function with nullptr.

        Texture pointer are optional. Textures can be identified by a supplied
        texture pointer or make a part of the material class.
        \param texture The texture pointer to set. */
    void Texture(ITexture *texture) const;

    //! Specifies a vertex for a polygon or triangle.
    /*! This call specifies a vertex for a polygon or a triangle. Vertices are 
        supplied in counter-clockwise order. At least 3 vertices must be supplied 
        for a polygon and exactly three for a triangle.
        \param vertex The vertex specified for the triangle or polygon. */
	void Vertex(const CGrVector &vertex) const;

    //! Specifies a normal for a polygon or triangle.
    /*! This call specifies a normal for a polygon or a triangle. Normals are associated
        with vertices. Either one normal can be supplied for the entire object or one
        normal per vertex should be supplied.
        \param normal The normal specified for the triangle or polygon. */
	void Normal(const CGrVector &normal) const;

    //! Specifies a texture coordinate. 
    /*! This call specifies a texture coordinate for a polygon or triangle. 
        Texture coordinates are optional and should not be supplied if they are
        not required, as the texture computation does increase the runtime, slightly.
        \param tvertex The texture coordinate. */
	void TexVertex(const CGrVector &tvertex) const;

    //! \cond INTERNAL
    // Parameter routines
    double SetIntersectionCost(double c);
    double GetIntersectionCost() const;
    double SetTraverseCost(double c);
    double GetTraverseCost() const;
    int SetMaxDepth(int m);
    int GetMaxDepth() const;
    int SetMinLeaf(int m);
    int GetMinLeaf() const;

    //! \endcond

    //! An identifier for the type of object.
    enum ObjectType {Polygon, Triangle, Other, None};

    //! Base class for objects in the ray intersection system.
    /*! This class is the base class for objects internal to
        the ray intersection system. It is used as a means of 
        keeping track of what object an intersection has occurred with. */
    class Object
    {
    public:
        //! Get the type of object (polygon or triangle or other)
        virtual ObjectType Type() const = 0;
    };

    //! The intersection test.
    /*! This function is called to determine any intersections with 
        objects. 
        \param ray Ray to test.
        \param maxt A maximum allowable t value. No intersection will be 
        returned for an object farther away than this.
        \param ignore An object to ignore or nullptr if nothing is to be ignored.
        \param object [out] The object hit if any.
        \param t [out] The t value for the intersection point.
        \param intersect [out] The intersection point.
        \return true if an intersection occurs. object, r, and intersect are only
        valid when this function returns true. */
    bool Intersect(const CRay &ray, double maxt, const Object *ignore, 
       const Object *&object, double &t, CGrVector &intersect);

    //! Determine information about the intersection
    /*! Given an intersection object and a ray, this funciton determines the
        normal and the texture coordinate at the point and any associated 
        material or texture objects. 
        \param ray The ray that hit the object.
        \param object The object hit by the ray.
        \param t The t value at the intersection.
        \param normal [out] A computed (interpolated) normal at the intersection point.
        \param material [out] A material pointer associated with the object.
        \param texture [out] A texture pointer associated with the object.
        \param textcoord [out] An interpolated texture coordinate at the intersection. */
    void IntersectInfo(const CRay &ray, const Object *object, double t, 
                      CGrVector &normal, IMaterial *&material, 
                      ITexture *&texture, CGrVector &texcoord) const; 

    //! Save statistics about the intersection session
    /*! When called, this function creates a file called stats.txt in the current
        directory that contains statistics about the intersection system such as the
        number of rays tested, number of objects tested, and statistics about the kd-tree 
        that stores the objects. */
    void SaveStats();

private:
    CRayIntersection(const CRayIntersection &);     // No copy constructor
    CRayIntersection &operator=(const CRayIntersection &);     // No assignment operator

    CRayIntersectionD *ri;
};