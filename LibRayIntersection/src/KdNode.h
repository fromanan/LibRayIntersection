#pragma once

#include "Polygon.h"
#include "Triangle.h"
#include "BoundingBox.h"

class CRayInersectionD;

//
// THis is a node in the KdTree that stores our geometry
//

class CKdNode
{
public:
    CKdNode(CRayIntersectionD *user);
    virtual ~CKdNode(void);
    
    friend class CRayIntersectionD;

    void Add(CIntersectionObject *p);

    void SetBoundingBox(const CBoundingBox &box) {m_bbox = box;}

    void ShrinkBoundingBox();
    void Subdivide();

private:
    CKdNode();

    CRayIntersectionD *GetUser() {return mUser;}

    CRayIntersectionD *mUser;

    // Members of this node (all of the polygons in the node)
    struct Member
    {
        CIntersectionObject *m_object;  // The actual object we are pointing to.
        CBoundingBox m_bbox;            // Bounding box clipped to this tree subdivision
    };

    std::vector<Member> m_members;

    CBoundingBox        m_bbox;         // Bounding box for the node
    int                 m_depth;        // Depth of the node in the tree

    CKdNode *m_left;     // Left subtree
    CKdNode *m_right;    // Right subtree

    // Member of the list of locations we will sort for a split computation
    struct SplitItem
    {
        enum Types {BEGIN, END, PLANAR};

        SplitItem(CKdNode::Member *m, Types t, double v) :
            mMember(m), m_type(t), m_value(v) {}

        CKdNode::Member *mMember;    // The member this is for

        Types   m_type;                 // Type of entry
        double  m_value;                // Value for a dimension

        bool operator<(const SplitItem &s) {return m_value < s.m_value;}
    };

    double  m_splitPoint;       // Split point
    int     m_splitDim;         // Split dimension
};
