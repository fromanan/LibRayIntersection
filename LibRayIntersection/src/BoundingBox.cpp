#include "StdAfx.h"
#include "BoundingBox.h"

inline double bmin(const double a, const double b) {return a < b ? a : b;}
inline double bmax(const double a, const double b) {return a < b ? b : a;}

CBoundingBox::CBoundingBox()
{
    mBounds[0].Set(0, 0, 0);
    mBounds[1].Set(0, 0, 0);
}

CBoundingBox::~CBoundingBox() = default;

//
// Name :         CBoundingBox::Include()
// Description :  Include a point in the bounding box, extending if necessary.
//

void CBoundingBox::Include(const CGrVector &p)
{
    if (p.X() < mBounds[0].X())
        mBounds[0].X() = p.X();
    if (p.Y() < mBounds[0].Y())
        mBounds[0].Y() = p.Y();
    if (p.Z() < mBounds[0].Z())
        mBounds[0].Z() = p.Z();

    if (p.X() > mBounds[1].X())
        mBounds[1].X() = p.X();
    if (p.Y() > mBounds[1].Y())
        mBounds[1].Y() = p.Y();
    if (p.Z() > mBounds[1].Z())
        mBounds[1].Z() = p.Z();
}

//
// Name :         CBoundingBox::IntersectWith()
// Description :  Make this bounding box the intersection with another.
//
void CBoundingBox::IntersectWith(const CBoundingBox &box)
{
    mBounds[0].X(bmax(mBounds[0].X(), box.mBounds[0].X()));
    mBounds[0].Y(bmax(mBounds[0].Y(), box.mBounds[0].Y()));
    mBounds[0].Z(bmax(mBounds[0].Z(), box.mBounds[0].Z()));
    mBounds[1].X(bmin(mBounds[1].X(), box.mBounds[1].X()));
    mBounds[1].Y(bmin(mBounds[1].Y(), box.mBounds[1].Y()));
    mBounds[1].Z(bmin(mBounds[1].Z(), box.mBounds[1].Z()));

    // Determine if empty...
    if (mBounds[0].X() >= mBounds[1].X() ||
        mBounds[0].Y() >= mBounds[1].Y() ||
        mBounds[0].Z() >= mBounds[1].Z())
    {
        mBounds[0].Set(0, 0, 0);
        mBounds[1].Set(0, 0, 0);
    }
}

//
// Name :         CBoundingBox::IntersectTest()
// Description :  Determine if a ray intersects this bounding box
//                Modified Smits method
//
bool CBoundingBox::IntersectTest(const CRayp &ray, const double t0, const double t1) const
{
    const CGrVector &invDirection = ray.InvDirection();
    const CGrVector &origin = ray.Origin();

    double tmin, tmax;

    if (invDirection.X() < 0)
    {
        tmin = (mBounds[1].X() - origin.X()) * invDirection.X();
        tmax = (mBounds[0].X() - origin.X()) * invDirection.X();
    }
    else
    {
        tmin = (mBounds[0].X() - origin.X()) * invDirection.X();
        tmax = (mBounds[1].X() - origin.X()) * invDirection.X();
    }

    double tymin;
    double tymax;

    if (invDirection.Y() < 0)
    {
        tymin = (mBounds[1].Y() - origin.Y()) * invDirection.Y();
        tymax = (mBounds[0].Y() - origin.Y()) * invDirection.Y();
    }
    else
    {
        tymin = (mBounds[0].Y() - origin.Y()) * invDirection.Y();
        tymax = (mBounds[1].Y() - origin.Y()) * invDirection.Y();
    }

    if (tmin > tymax || tymin > tmax)
        return false;

    if (tymin > tmin)
        tmin = tymin;
    if (tymax < tmax)
        tmax = tymax;

    double tzmin, tzmax;

    if (invDirection.Z() < 0)
    {
        tzmin = (mBounds[1].Z() - origin.Z()) * invDirection.Z();
        tzmax = (mBounds[0].Z() - origin.Z()) * invDirection.Z();
    }
    else
    {
        tzmin = (mBounds[0].Z() - origin.Z()) * invDirection.Z();
        tzmax = (mBounds[1].Z() - origin.Z()) * invDirection.Z();
    }

    if (tmin > tzmax || tzmin > tmax)
        return false;

    if (tzmin > tmin)
        tmin = tzmin;
    if (tzmax < tmax)
        tmax = tzmax;

    return tmin < t1 && tmax > t0;
}