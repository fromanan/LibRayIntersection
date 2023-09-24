#pragma once

#include "Rayp.h"

class CBoundingBox
{
public:
    CBoundingBox(void);
    CBoundingBox(const CGrVector &p)
        {mBounds[0].Set(p);  mBounds[1].Set(p);}
    virtual ~CBoundingBox(void);

    void SetEmpty() {mBounds[0].Set(0, 0, 0);  mBounds[1].Set(0, 0, 0);}
    void Set(const CGrVector &p) {mBounds[0].Set(p);  mBounds[1].Set(p);}
    void Set(const CBoundingBox &b) {mBounds[0] = b.Min();  mBounds[1] = b.Max();}
    void Include(const CGrVector &p);
    void Include(const CBoundingBox &b) {Include(b.Min()); Include(b.Max());}

    const CGrVector &Max() const {return mBounds[1];}
    const double Max(int d) const {return mBounds[1][d];}
    const CGrVector &Min() const {return mBounds[0];}
    const double Min(int d) const {return mBounds[0][d];}
    CGrVector Extent() const {return (Max() - Min());}
    double Extent(int d) const {return mBounds[1][d] - mBounds[0][d];}
    double ExtentX() const {return mBounds[1].X() - mBounds[0].X();}
    double ExtentY() const {return mBounds[1].Y() - mBounds[0].Y();}
    double ExtentZ() const {return mBounds[1].Z() - mBounds[0].Z();}

    void SetMinD(int d, double v) {mBounds[0][d] = v;}
    void SetMaxD(int d, double v) {mBounds[1][d] = v;}

    void IntersectWith(const CBoundingBox &box);
    bool IntersectTest(const CRayp &ray, double t0=0, double t1=1e10) const;

    bool IsEmpty() {return mBounds[0].X() >= mBounds[1].X() || 
                           mBounds[0].Y() >= mBounds[1].Y() || 
                           mBounds[0].Z() >= mBounds[1].Z();}

private:
    CGrVector       mBounds[2];     // 0=min, 1=max
    //CGrVector    mBounds[0];
    //CGrVector    mBounds[1];
};
