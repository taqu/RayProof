#include "LQBVH.h"
#include "AABB.h"
#include "RadixSort.h"
#include "Ray.h"

namespace lray
{
bool LQBVH::Node::isLeaf() const
{
    return LeafFlag == (leaf_.flags_ & LeafFlag);
}

void LQBVH::Node::setLeaf(u32 start, u32 size)
{
    leaf_.flags_ = LeafFlag;
    leaf_.start_ = start;
    leaf_.size_ = size;
}

void LQBVH::Node::setJoint(u32 child, const AABB bbox[4], const u8 axis[3])
{
    LRAY_ALIGN16 f32 bb[2][3][4];
    joint_.flags_ = 0;
    joint_.children_ = child;
    for(u32 i = 0; i < 3; ++i) {
        for(u32 j = 0; j < 4; ++j) {
            bb[0][i][j] = bbox[j].min_[i] - BBoxEpsilon;
            bb[1][i][j] = bbox[j].max_[i] + BBoxEpsilon;
        }
    }

    for(u32 i = 0; i < 2; ++i) {
        for(u32 j = 0; j < 3; ++j) {
            _mm_store_ps((f32*)&joint_.bbox_[i][j], _mm_load_ps(bb[i][j]));
        }
    }

    joint_.axis0_ = axis[0];
    joint_.axis1_ = axis[1];
    joint_.axis2_ = axis[2];
}

u32 LQBVH::Node::getPrimitiveIndex() const
{
    return leaf_.start_;
}

u32 LQBVH::Node::getNumPrimitives() const
{
    return leaf_.size_;
}

LQBVH::LQBVH()
    : depth_(0)
{
}

LQBVH::~LQBVH()
{
}

void LQBVH::build(u32 numPrimitives, const AABB* primitives)
{
    u32 depth = static_cast<u32>(std::ceil(std::log(static_cast<f32>(numPrimitives >> 4)) / std::log(4.0f)));
    depth = minimum(depth, MaxDepth);
    u32 numNodes = 2;
    u32 leaves = 1;
    for(u32 i = 1; i < depth; ++i) {
        leaves *= 4;
        numNodes += leaves;
    }

    nodes_.reserve(numNodes);
    nodes_.resize(1);

    primitives_.resize(numPrimitives);
    ::memcpy(&primitives_[0], primitives, sizeof(AABB)*numPrimitives);

    // Calc bbox
    AABB bbox;
    bbox.setInvalid();
    for(u32 i = 0; i < numPrimitives; ++i) {
        bbox.extend(primitives_[i]);
    }

    // Calc Morton codes
    indexCodes_.resize(numPrimitives*2);
    u32* indices = &indexCodes_[0];
    u32* codes = &indexCodes_[numPrimitives];
    Vector3 invUnit = calcInvUnit(bbox);
    for(u32 i = 0; i < numPrimitives; ++i) {
        Vector3 centroid = primitives_[i].centroid();
        indices[i] = i;
        codes[i] = calcMortonCode3(centroid, invUnit, bbox);
    }

    depth_ = 1;
    radix_sort(numPrimitives, codes, indices);
    recursiveConstruct(numPrimitives);
}

HitRecord LQBVH::intersect(const Ray& ray, f32 tmin, f32 tmax, std::function<HitRecord(const Ray& ray,u32,f32,f32)> func)
{
    __m128 origin[3];
    __m128 invDir[3];
    __m128 tminSSE;
    __m128 tmaxSSE;
    origin[0] = _mm_set1_ps(ray.origin_.x_);
    origin[1] = _mm_set1_ps(ray.origin_.y_);
    origin[2] = _mm_set1_ps(ray.origin_.z_);

    Vector3 invDirection = {1.0f / ray.direction_.x_, 1.0f / ray.direction_.y_, 1.0f / ray.direction_.z_};
    invDir[0] = _mm_set1_ps(invDirection.x_);
    invDir[1] = _mm_set1_ps(invDirection.y_);
    invDir[2] = _mm_set1_ps(invDirection.z_);

    tminSSE = _mm_set1_ps(tmin);
    tmaxSSE = _mm_set1_ps(tmax);

    u32 raySign[3];
    raySign[0] = (0.0f <= ray.direction_[0]) ? 0 : 1;
    raySign[1] = (0.0f <= ray.direction_[1]) ? 0 : 1;
    raySign[2] = (0.0f <= ray.direction_[2]) ? 0 : 1;

    HitRecord hitRecord;
    hitRecord.object_ = nullptr;
    hitRecord.t_ = tmax;
    const u32* indices = &indexCodes_[0];
    s32 stack = 0;
    u32 nodeStack[MaxDepth << 2];
    nodeStack[0] = 0;
    while(0 <= stack) {
        u32 index = nodeStack[stack];
        const Node& node = nodes_[index];
        assert(node.leaf_.flags_ == node.joint_.flags_);
        --stack;
        if(node.isLeaf()) {
            u32 primIndex = node.getPrimitiveIndex();
            u32 primEnd = primIndex + node.getNumPrimitives();
            for(u32 i = primIndex; i < primEnd; ++i) {
                f32 t;
                u32 idx = indices[i];
                if(!primitives_[idx].testRay(t, ray, tmin, hitRecord.t_)) {
                    continue;
                }
                if(tmin < t && t < hitRecord.t_) {
                    HitRecord hr = func(ray, idx, t, hitRecord.t_);
                    if(nullptr != hr.object_){
                        hitRecord = hr;
                        tmaxSSE = _mm_set1_ps(hr.t_);
                    }
                }
            } // for(u32 i=primIndex;

        } else {
            s32 hit = testRayAABB(tminSSE, tmaxSSE, origin, invDir, raySign, node.joint_.bbox_);
            s32 split = raySign[node.joint_.axis0_] + (raySign[node.joint_.axis1_] << 1) + (raySign[node.joint_.axis2_] << 2);

            // whether to revert for each splits 2x2x2
            static const u16 TraverseOrder[] =
                {
                    0x0123U,
                    0x2301U,
                    0x1023U,
                    0x3201U,
                    0x0132U,
                    0x2301U,
                    0x1032U,
                    0x3210U,
                };
            u16 order = TraverseOrder[split];
            u32 children = node.joint_.children_;
            for(u32 i = 0; i < 4; ++i) {
                u16 o = order & 0x03U;
                if(hit & (0x01U << o)) {
                    nodeStack[++stack] = children + o;
                }
                order >>= 4;
            }
        }
    } // while(0<=stack){
    return hitRecord;
}

Vector3 LQBVH::calcInvUnit(const AABB& bbox)
{
    Vector3 invUnit = bbox.max_ - bbox.min_;
    invUnit = invUnit * (1.0f / NumSplits);
    for(u32 i = 0; i < 3; ++i) {
        invUnit[i] = 1.0f / invUnit[i];
    }
    return invUnit;
}

u32 LQBVH::calcMortonCode3(const Vector3& x, const Vector3& invUnit, const AABB& bbox)
    {
        Vector3 d = (x - bbox.min_);
        d.x_ *= invUnit.x_;
        d.y_ *= invUnit.y_;
        d.z_ *= invUnit.z_;

        u32 v[3];
        for(u32 i=0; i<3; ++i){
            v[i] = static_cast<u32>(d[i]);
            v[i] = (v[i]<NumSplits)? v[i] : NumSplits-1;
        }
        return mortonCode3(v[0], v[1], v[2]);
    }

void LQBVH::recursiveConstruct(u32 numPrimitives)
    {
        Work works[MaxDepth << 2];
        AABB childBBox[4];
        u32 primStart[4];
        u32 num[4];
        u8 axis[4];

        s32 stack = 0;
        works[0] = {0, numPrimitives, 0, 1};
        while(0 <= stack) {
            Work work = works[stack];
            --stack;

            depth_ = maximum(work.depth_, depth_);
            if(work.numPrimitives_ <= MinLeafPrimitives || MaxDepth <= work.depth_) {
                nodes_[work.node_].setLeaf(work.start_, work.numPrimitives_);
                continue;
            }

            s32 end = work.start_ + work.numPrimitives_;
            s32 last = end - 1;

            primStart[0] = work.start_;
            findSplit(primStart[2], axis[0], &indexCodes_[0], work.start_, last);
            findSplit(primStart[1], axis[1], &indexCodes_[0], work.start_, primStart[2]);
            findSplit(primStart[3], axis[2], &indexCodes_[0], primStart[2], last);

            ++primStart[1];
            ++primStart[2];
            ++primStart[3];
            num[0] = primStart[1] - work.start_;
            num[1] = primStart[2] - primStart[1];
            num[2] = primStart[3] - primStart[2];
            num[3] = end - primStart[3];

            getBBox(childBBox[0], primStart[0], primStart[1]);
            getBBox(childBBox[1], primStart[1], primStart[2]);
            getBBox(childBBox[2], primStart[2], primStart[3]);
            getBBox(childBBox[3], primStart[3], end);

            if(nodes_.capacity() < (nodes_.size() + 4)) {
                nodes_.reserve(nodes_.capacity() << 1);
            }

            s32 child = nodes_.size();
            nodes_[work.node_].setJoint(child, childBBox, axis);
            nodes_.resize(nodes_.size() + 4);
            for(s32 i = 0; i < 4; ++i) {
                works[++stack] = Work(primStart[i], num[i], child, work.depth_ + 1);
                ++child;
            }
        }
    }

    void LQBVH::getBBox(AABB& bbox, u32 start, u32 end)
    {
        bbox.setInvalid();
        const u32* indices = &indexCodes_[0];
        for(u32 i=start; i<end; ++i){
            bbox.extend(primitives_[indices[i]]);
        }
    }

    void LQBVH::findSplit(u32& split, u8& axis, const u32* codes, u32 first, u32 last)
    {
        u32 firstCode = codes[first];
        u32 lastCode = codes[last];
        if(firstCode == lastCode){
            axis = 0;
            split = (first+last)>>1;
            return;
        }
        u32 commonPrefix = leadingzero(firstCode^lastCode);
        split = first;
        u32 step = last-first;
        //binary search
        do{
            step = (step+1)>>1;
            u32 newSplit = split + step;
            if(newSplit<last){
                u32 splitCode = codes[newSplit];
                u32 splitPrefix = leadingzero(firstCode^splitCode);
                if(commonPrefix<splitPrefix){
                    split = newSplit;
                }
            }
        }while(1<step);
        assert(first<=split && split<last);
        if(first<split){
            firstCode = codes[split-1];
            lastCode = codes[split];
            u32 bitIndex = 31-leadingzero(firstCode^lastCode);
            axis = bitIndex%3;
        } else{
            axis = 0;
        }
    }
    } // namespace lray

