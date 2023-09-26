#include "StdAfx.h"
#include <algorithm>
#include <cassert>

#include "KdNode.h"
#include "RayIntersectionD.h"

using namespace std;

// The surface area computation algorithm.  
inline double AreaCompute(const CGrVector &p)
{
    return p.X() * p.Y() + p.X() * p.Z() + p.Y() * p.Z();
}

CKdNode::CKdNode(CRayIntersectionD *user)
    : mUser(user), m_depth(0), m_left(nullptr), m_right(nullptr), m_splitPoint(0) {}

CKdNode::~CKdNode()
{
    delete m_left;
    delete m_right;
}

//
// Name :         CRayIntersectionD::KdNode::Add()
// Description :  Add an object to a node.  This function should only be
//                called when building the initial single-level tree.  
// Post :         A new member.
//                Valid bounding box for the member.
//                Valid node bounding box (always current)
//
void CKdNode::Add(CIntersectionObject *p)
{
    // Add new member to the list
    m_members.emplace_back();
    auto & [m_object, m_bbox] = m_members.back();

    // Set the polygon in the member
    m_object = p;
    m_bbox = p->GetBoundingBox();
}

void CKdNode::Subdivide()
{
    //
    // Determine the actual bounding box for this 
    // node. This is the intersection of the bounding box 
    // as supplied and the bounding box of all contained
    // members.
    //

    // Check for early return conditions
    const int nMembers = static_cast<int>(m_members.size());
    if (m_depth >= GetUser()->GetMaxDepth() 
        || nMembers <= GetUser()->GetMinLeaf())
        return;             // All done

    // Get cost parameters
    const double intersectionCost = GetUser()->GetIntersectionCost();
    const double traverseCost = GetUser()->GetTraverseCost();

    // Cost estimation if we do not split
    const double costNoSplit = intersectionCost * nMembers * AreaCompute(m_bbox.Extent());

    // ShrinkBoundingBox();

    // Information to keep track of the best cost we have seen...
    double bestCost = costNoSplit;
    bool bestIsSplit = false;           // Until we know otherwise.
    bool bestIsLeft;                    // True if we put planer objects on the left side
    double bestSplitPoint;
    int bestDim;
    int bestL;
    int bestR;
    int bestP;

    // The array of split items
    std::vector<SplitItem>  splitItems;

    // Try all three possible dimensions to see if we can split
    // and get a better cost.
    for (int dim=0;  dim<3;  dim++)
    {
        splitItems.clear();

        //
        // Iterate over all of the members and build up a list of 
        // begins and ends for the clipped members.
        //

        int planer = 0;
        int nplaner = 0;
        for (auto member=m_members.begin(); member != m_members.end(); ++member)
        {
            // Obtain the min and max for the appropriate dimension
            const double v1 = member->m_bbox.Min()[dim];
            if (const double v2 = member->m_bbox.Max()[dim]; v1 == v2)
            {
                splitItems.emplace_back(&*member, SplitItem::PLANAR, v1);
                planer++;
            }
            else
            {
                splitItems.emplace_back(&*member, SplitItem::BEGIN, v1);
                splitItems.emplace_back(&*member, SplitItem::END, v2);

                nplaner++;
            }
        }

        //
        // Sort that list
        //

        std::sort(splitItems.begin(), splitItems.end());

        // 
        // Iterate over the split list
        //

        int tL = 0;         // Number of polygons left of split point
        int tR = nMembers; // Number of polygons right of the split point

        // Areas before and after the bounding box
        // We'll swap one value later, though
        CGrVector lsize = m_bbox.Extent();  
        CGrVector rsize = m_bbox.Extent();

        const double bFm = m_bbox.Min()[dim];
        const double bTo = m_bbox.Max()[dim];

        for (auto si=splitItems.begin(); si != splitItems.end();)
        {
            int pl = 0; // Number of polygons ending at the split point
            int pr = 0; // Number of polygons beginning at the split point
            int tP = 0; // Number of polygons lying in the split plane

            // We need to iterate over all items that have the 
            // same m_value
            const double splitPoint = si->m_value;
            while(si != splitItems.end() && si->m_value == splitPoint)
            {
                switch(si->m_type)
                {
                case SplitItem::BEGIN:
                    pr++;
                    break;

                case SplitItem::END:
                    pl++;
                    break;

                case SplitItem::PLANAR:
                    tP++;
                    break;
                }

                ++si;
            }

            tR -= pl;           // tR is right. Anything ending is removed from the right
            tR -= tP;           // Planers are removed and treated independently
            tL += pr;           // Anything that starts on left is added.
            
            assert(tL >= 0);
            assert(tR >= 0);

            // Determine the size for the left and right areas
            lsize[dim] = splitPoint - bFm;
            rsize[dim] = bTo - splitPoint;

            // Compute the cost using the surface area heuristic.
            // This uses a bonus system that favors cutting off
            // empty space at higher levels of the tree.
            const double lA = AreaCompute(lsize);         // Area left of the split point
            const double rA = AreaCompute(rsize);         // Area right of the split point

            // We compute two costs, depending on which side we put the planer objects on
            const double costL = traverseCost + intersectionCost * (lA * (tL + tP) + rA * tR);
            const double costR = traverseCost + intersectionCost * (lA * tL + rA * (tR + tP));

            const bool isLeftCost = costL < costR;
            const double cost = isLeftCost ? costL : costR;
            assert(cost >= 0);

            if (cost < bestCost)
            {
                bestCost = cost;
                bestIsSplit = true;
                bestIsLeft = isLeftCost;
                bestSplitPoint = splitPoint;
                bestDim = dim;
                bestL = tL;
                bestR = tR;
                bestP = tP;
            }

            tL += tP;       // Planar objects will be in the left next pass
        }

        assert(tL == nMembers);
        assert(tR == 0);
    }

    if (bestL == 0 || bestR == 0)
    {
        int xx = 0;
    }

    //
    // Do we split at all?
    //

    if (!bestIsSplit)
        return;             // All done

    // Indicate the split
    m_splitPoint = bestSplitPoint;
    m_splitDim = bestDim;

    // Create two new nodes that will be the children of this node
    auto* left = new CKdNode(mUser);
    m_left = left;
    auto* right = new CKdNode(mUser);
    m_right = right;

    // Left and right bounding boxes are initially the node box
    CBoundingBox lBox(m_bbox);
    CBoundingBox rBox(m_bbox);

    // And we split them
    lBox.SetMaxD(m_splitDim, m_splitPoint);
    rBox.SetMinD(m_splitDim, m_splitPoint);

    left->SetBoundingBox(lBox);
    right->SetBoundingBox(rBox);

    // Increase number of nodes statistic by 2
    int newNodesCount = 2;

    const int newDepth = m_depth + 1;
    left->m_depth = newDepth;
    right->m_depth = newDepth;

    // A three deep tree will have depth values from 0 to 2 at the leaf,
    // so add one for my statistic here.
    GetUser()->NewDepth(newDepth + 1);

    // Traverse the members list and move all items 
    // to the appropriate child.
    for (auto member = m_members.begin(); member != m_members.end(); ++member)
    {
        // Make a member for each side with a bounding box reduced to the 
        // tree intersection.
        Member lMember = *member;
        lMember.m_bbox.IntersectWith(lBox);
        const bool lEmpty = lMember.m_bbox.IsEmpty();

        Member rMember = *member;
        rMember.m_bbox.IntersectWith(rBox);
        const bool rEmpty = rMember.m_bbox.IsEmpty();

        // Determine if we can move to just one side?
        if (member->m_bbox.Max()[bestDim] == bestSplitPoint && 
            member->m_bbox.Min()[bestDim] == bestSplitPoint)
        {
            // This is a planer object at the split point.  Move it to the best side we found
            if (bestIsLeft)
            {
                if (!lEmpty)
                    left->m_members.push_back(lMember);
            }
            else
            {
                if (!rEmpty)
                    right->m_members.push_back(rMember);
            }
        }
        else if (member->m_bbox.Max()[bestDim] <= bestSplitPoint)
        {
            if (!lEmpty)
                left->m_members.push_back(lMember);
        }
        else if (member->m_bbox.Min()[bestDim] >= bestSplitPoint)
        {
            if (!rEmpty)
                right->m_members.push_back(rMember);
        }
        else
        {
            // This object straddles both sides, so it goes to both sides.
            if (!lEmpty)
                left->m_members.push_back(lMember);

            if (!rEmpty)
                right->m_members.push_back(rMember);
        }
    }

    // We are done with all members
    m_members.clear();

    // Recompute all bounding boxes for these two nodes
    //left->ShrinkBoundingBox();
    //right->ShrinkBoundingBox();

    //
    // And recurse
    //

    // There is a chance that we may split with zero on one side.  If that 
    // happens, we don't need that node, anyway.
    if (left->m_members.empty())
    {
        delete m_left;
        m_left = nullptr;
        newNodesCount--;
    }

    if (right->m_members.empty())
    {
        delete m_right;
        m_right = nullptr;
        newNodesCount--;
    }

    GetUser()->StatIncNodes(newNodesCount);

    if (m_left)
        m_left->Subdivide();

    if (m_right)
        m_right->Subdivide();
}

//
// Name :         CKdNode::ShrinkBoundingBox()
// Description :  Compute the bounding boxes for a node. Computes all member
//                bounding boxes and intersects that with the node bounding box.
// Post :         Valid bounding box for each member.
//                Valid node bounding box
//
void CKdNode::ShrinkBoundingBox()
{
    bool first = true;
    CBoundingBox box;       // Complete enclosing box
    bool anyEmpty = false;

    // Members
    for (auto member = m_members.begin(); member != m_members.end(); ++member)
    {
        CBoundingBox &pbox = member->m_bbox;
        pbox = member->m_object->GetBoundingBox();
        pbox.IntersectWith(m_bbox);
        if (pbox.IsEmpty())
        {
            anyEmpty = true;
        }
        else
        {
            if (first)
            {
                box = pbox;
                first = false;
            }
            else
            {
                box.Include(member->m_bbox);
            }
        }
    }

    if (anyEmpty)
    {
        // We have to copy the list to remove the empty member
        vector<Member> polys;
        for (auto member=m_members.begin(); member != m_members.end(); ++member)
        {
            if (!member->m_bbox.IsEmpty())
                polys.push_back(*member);
        }

        m_members = polys;
        
        anyEmpty = false;
    }

    if (first)
    {
        // We found nothing at all
        m_bbox.SetEmpty();
    }
    else
    {
        // Node bounding box is the intersection of these
        m_bbox.IntersectWith(box);
    }
}


