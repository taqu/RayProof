#include "BinQBVH.h"
#include "Object.h"
#include "Ray.h"

namespace lray
{
bool BinQBVH::Node::isLeaf() const
{
    return LeafFlag == (leaf_.flags_ & LeafFlag);
}

void BinQBVH::Node::setLeaf(u32 start, u32 size)
{
    leaf_.flags_ = LeafFlag;
    leaf_.start_ = start;
    leaf_.size_ = size;
}

void BinQBVH::Node::setJoint(u32 child, const AABB bbox[4], u8 axis[3])
{
    LRAY_ALIGN16 f32 bb[2][3][4];
    joint_.flags_ = 0;
    joint_.children_ = child;
    for(u32 i = 0; i < 3; ++i) {
        for(u32 j = 0; j < 4; ++j) {
            bb[0][i][j] = bbox[j].min_[i] - Epsilon;
            bb[1][i][j] = bbox[j].max_[i] + Epsilon;
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

u32 BinQBVH::Node::getPrimitiveIndex() const
{
    return leaf_.start_;
}

u32 BinQBVH::Node::getNumPrimitives() const
{
    return leaf_.size_;
}

BinQBVH::BinQBVH()
    : SAH_KI_(1.5f)
    , SAH_KT_(1.0f)
    , object_(nullptr)
    , depth_(0)
{
}

BinQBVH::~BinQBVH()
{
}

void BinQBVH::build(const Object* object)
{
    assert(nullptr != object);
    u32 numPrimitives = object->getNumFaces();
    f32 depth = logf(static_cast<f32>(numPrimitives) / MinLeafPrimitives) / logf(4.0f);
    u32 numNodes = static_cast<u32>(powf(2.0f, depth) + 0.5f);
    nodes_.reserve(numNodes);
    nodes_.resize(1);

    object_ = object;
    primitiveIndices_.resize(numPrimitives);
    primitiveCentroids_.resize(numPrimitives);
    primitiveBBoxes_.resize(numPrimitives);

    AABB bbox;
    bbox.setInvalid();
    for(u32 i = 0; i < numPrimitives; ++i) {
        primitiveIndices_[i] = i;
        primitiveBBoxes_[i] = object->getAABB(i);
        primitiveCentroids_[i] = primitiveBBoxes_[i].centroid();
        bbox.extend(primitiveBBoxes_[i]);
    }

    depth_ = 1;
    recursiveConstruct(numPrimitives, bbox);

    primitiveCentroids_.clear();
    primitiveBBoxes_.clear();
}

HitRecord BinQBVH::intersect(const Ray& ray, f32 tmin, f32 tmax)
{
    __m128 origin[3];
    __m128 invDir[3];
    __m128 tminSSE;
    __m128 tmaxSSE;
    origin[0] = _mm_set1_ps(ray.origin_.x_);
    origin[1] = _mm_set1_ps(ray.origin_.y_);
    origin[2] = _mm_set1_ps(ray.origin_.z_);

    invDir[0] = _mm_set1_ps(1.0f/ray.direction_.x_);
    invDir[1] = _mm_set1_ps(1.0f/ray.direction_.y_);
    invDir[2] = _mm_set1_ps(1.0f/ray.direction_.z_);

    tminSSE = _mm_set1_ps(tmin);
    tmaxSSE = _mm_set1_ps(tmax);

    u32 raySign[3];
    raySign[0] = (0.0f <= ray.direction_[0]) ? 0 : 1;
    raySign[1] = (0.0f <= ray.direction_[1]) ? 0 : 1;
    raySign[2] = (0.0f <= ray.direction_[2]) ? 0 : 1;

    HitRecord hitRecord;
    hitRecord.t_ = tmax;
    hitRecord.object_ = nullptr;
    hitRecord.face_ = HitRecord::Invalid;

    s32 stack = 0;
    u32 nodeStack[MaxDepth << 2];
    nodeStack[0] = 0;
    while(0 <= stack) {
        u32 index = nodeStack[stack];
        const Node& node = nodes_[index];
        --stack;
        if(node.isLeaf()) {
            u32 primIndex = node.getPrimitiveIndex();
            u32 primEnd = primIndex + node.getNumPrimitives();
            for(u32 i = primIndex; i < primEnd; ++i) {
                f32 t;
                u32 idx = primitiveIndices_[i];
                if(!object_->testRay(t, idx, ray, tmin, tmax)) {
                    continue;
                }
                if(HitEpsilon < t && t < hitRecord.t_) {
                    tmax = t;
                    hitRecord.t_ = t;
                    hitRecord.object_ = object_;
                    hitRecord.face_ = idx;
                    tmaxSSE = _mm_set1_ps(t);
                }
            } // for(u32 i=primIndex;

        } else {
            s32 hit = testRayAABB(tminSSE, tmaxSSE, origin, invDir, raySign, node.joint_.bbox_);
            s32 split = raySign[node.joint_.axis0_] + (raySign[node.joint_.axis1_] << 1) + (raySign[node.joint_.axis2_] << 2);

            // Visit pattern from ray direction. 2x2x2
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
            s32 children = node.joint_.children_;
            for(s32 i = 0; i < 4; ++i) {
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

u32 BinQBVH::getDepth() const
{
    return depth_;
}

void BinQBVH::getBBox(AABB& bbox, u32 start, u32 end)
{
    bbox.setInvalid();
    for(u32 i = start; i < end; ++i) {
        bbox.extend(primitiveBBoxes_[primitiveIndices_[i]]);
    }
}

void BinQBVH::recursiveConstruct(u32 numPrimitives, const AABB& bbox)
{
    AABB childBBox[4];
    u32 primStart[4];
    u32 num[4];
    u8 axis[4];

    s32 stack = 0;
    works_[0] = Work(0, numPrimitives, 0, 1, bbox);
    while(0 <= stack) {
        Work work = works_[stack];
        --stack;

        depth_ = maximum(work.depth_, depth_);
        if(work.numPrimitives_ <= MinLeafPrimitives || MaxDepth <= work.depth_ || MaxNodes <= nodes_.size()) {
            nodes_[work.node_].setLeaf(work.start_, work.numPrimitives_);
            continue;
        }

        primStart[0] = work.start_;
#if 0
        if(MaxBinningDepth < work.depth_) {
            // Split top
            splitMid(axis[0], num[0], num[2], childBBox[0], childBBox[2], primStart[0], work.numPrimitives_, work.bbox_);
            primStart[2] = work.start_ + num[0];

            // Split left
            splitMid(axis[1], num[0], num[1], childBBox[0], childBBox[1], work.start_, num[0], childBBox[0]);
            primStart[1] = work.start_ + num[0];

            // Split right
            splitMid(axis[2], num[2], num[3], childBBox[2], childBBox[3], primStart[2], num[2], childBBox[2]);
            primStart[3] = primStart[2] + num[2];

        } else
#endif
        {
            // Split top
            f32 area = work.bbox_.halfArea();
            if(area <= Epsilon) {
                splitMid(axis[0], num[0], num[2], childBBox[0], childBBox[2], area, primStart[0], work.numPrimitives_, work.bbox_);

            } else if(work.numPrimitives_ < NumBins) {
                splitMid(axis[0], num[0], num[2], childBBox[0], childBBox[2], area, primStart[0], work.numPrimitives_, work.bbox_);

            } else {
                splitBinned(axis[0], num[0], num[2], childBBox[0], childBBox[2], area, primStart[0], work.numPrimitives_, work.bbox_);
            }
            primStart[2] = work.start_ + num[0];

            // Split left
            area = childBBox[0].halfArea();
            if(area <= Epsilon) {
                splitMid(axis[1], num[0], num[1], childBBox[0], childBBox[1], area, primStart[0], num[0], childBBox[0]);

            } else if(num[0] < NumBins) {
                splitMid(axis[1], num[0], num[1], childBBox[0], childBBox[1], area, primStart[0], num[0], childBBox[0]);

            } else {
                splitBinned(axis[1], num[0], num[1], childBBox[0], childBBox[1], area, primStart[0], num[0], childBBox[0]);
            }
            primStart[1] = work.start_ + num[0];

            // Split right
            area = childBBox[2].halfArea();
            if(area <= Epsilon) {
                splitMid(axis[2], num[2], num[3], childBBox[2], childBBox[3], area, primStart[2], num[2], childBBox[2]);

            } else if(num[2] < NumBins) {
                splitMid(axis[2], num[2], num[3], childBBox[2], childBBox[3], area, primStart[2], num[2], childBBox[2]);

            } else {
                splitBinned(axis[2], num[2], num[3], childBBox[2], childBBox[3], area, primStart[2], num[2], childBBox[2]);
            }
            primStart[3] = primStart[2] + num[2];
        }

        if(nodes_.capacity() < (nodes_.size() + 4)) {
            nodes_.reserve(nodes_.capacity() << 1);
        }

        u32 child = nodes_.size();
        nodes_[work.node_].setJoint(child, childBBox, axis);
        nodes_.resize(nodes_.size() + 4);
        for(u32 i = 0; i < 4; ++i) {
            works_[++stack] = Work(primStart[i], num[i], child, work.depth_ + 1, childBBox[i]);
            ++child;
        }
    }
}

namespace
{
    struct SortFuncCentroid
    {
        SortFuncCentroid(u32 axis, const Vector3* centroids)
            :axis_(axis)
            ,centroids_(centroids)
        {}

        bool operator()(u32 i0, u32 i1) const
        {
            return centroids_[i0][axis_] < centroids_[i1][axis_];
        }
        u32 axis_;
        const Vector3* centroids_;
    };

    f32 halfArea(const Vector3& x)
    {
        return x.x_ * x.y_ + x.y_ * x.z_ + x.z_ * x.x_;
    }
}

void BinQBVH::splitMid(u8& axis, u32& num_l, u32& num_r, AABB& bbox_l, AABB& bbox_r, f32 area, u32 start, u32 numPrimitives, const AABB& bbox)
{
    u32 end = start + numPrimitives;
    u32 mid = start + (numPrimitives >> 1);

    f32 area_l, area_r;
    f32 bestCost = std::numeric_limits<f32>::max();
    f32 invArea = 1.0f/area;

    // Try all splits, and choose minimum SAH
    axis = static_cast<u8>(bbox.maxExtentAxis());
    SortFuncCentroid func(axis, &primitiveCentroids_[0]);
    insertionsort(numPrimitives, &primitiveIndices_[start], func);

    AABB bl, br;
    for(u32 m = start + 1; m < end; ++m) {
        getBBox(bl, start, m);
        getBBox(br, m, end);

        area_l = bl.halfArea();
        area_r = br.halfArea();
        num_l = m - start;
        num_r = numPrimitives - num_l;

        f32 cost = SAH_KT_ + SAH_KI_ * invArea * (area_l * num_l + area_r * num_r);
        if(cost < bestCost) {
            mid = m;
            bestCost = cost;
            bbox_l = bl;
            bbox_r = br;
        }
    }

    num_l = mid - start;
    num_r = numPrimitives - num_l;
}

void BinQBVH::splitBinned(u8& axis, u32& num_l, u32& num_r, AABB& bbox_l, AABB& bbox_r, f32 area, u32 start, u32 numPrimitives, const AABB& bbox)
{
    LRAY_ALIGN16 u32 minBins[NumBins];
    LRAY_ALIGN16 u32 maxBins[NumBins];

    __m128 zero = _mm_setzero_ps();

    f32 invArea = 1.0f / area;
    axis = 0;
    u32 end = start + numPrimitives;

    f32 bestCost = std::numeric_limits<f32>::max();
    u32 midBin = NumBins / 2;
    u32 step = static_cast<u32>(::log10f(static_cast<f32>(numPrimitives)));

    Vector3 extent = bbox.extent();
    Vector3 unit = extent * (1.0f / NumBins);
    for(u8 curAxis = 0; curAxis < 3; ++curAxis) {
        for(u32 i = 0; i < NumBins; i += 4) {
            _mm_store_ps(reinterpret_cast<f32*>(&minBins[i]), zero);
            _mm_store_ps(reinterpret_cast<f32*>(&maxBins[i]), zero);
        }
        SortFuncCentroid func(axis, &primitiveCentroids_[0]);
        introsort(numPrimitives, &primitiveIndices_[start], func);

        f32 invUnit = (absolute(unit[curAxis]) < Epsilon) ? 0.0f : 1.0f / unit[curAxis];
        f32 bmin = bbox.min_[curAxis];

        for(u32 i = start; i < end; i += step) {
            u32 index = primitiveIndices_[i];
            u32 minIndex = minimum(static_cast<u32>(invUnit * (primitiveBBoxes_[index].min_[curAxis] - bmin)), NumBins - 1);
            u32 maxIndex = minimum(static_cast<u32>(invUnit * (primitiveBBoxes_[index].max_[curAxis] - bmin)), NumBins - 1);
            assert(0 <= minIndex && minIndex < NumBins);
            assert(0 <= maxIndex && maxIndex < NumBins);
            ++minBins[minIndex];
            ++maxBins[maxIndex];
        }

        Vector3 e = extent;
        e[curAxis] = unit[curAxis];
        f32 unitArea = halfArea(e);

        s32 binLeft = 0;
        s32 binRight = static_cast<s32>(NumBins) - 1;

        while(minBins[binLeft] <= 0) {
            ++binLeft;
        }
        while(maxBins[binRight] <= 0) {
            --binRight;
        }
        assert(0 <= binLeft && binLeft < NumBins);
        assert(0 <= binRight && binRight < NumBins);

        s32 n_l = minBins[0];
        s32 n_r = 0;
        for(s32 i = 1; i < binRight; ++i) {
            n_r += maxBins[i];
        }
        for(s32 m = binLeft; m <= binRight; ++m) {
            f32 area_l = m * unitArea;
            f32 area_r = (NumBins - m) * unitArea;
            f32 cost = SAH_KT_ + SAH_KI_ * invArea * (area_l * n_l + area_r * n_r);
            if(cost < bestCost) {
                midBin = m;
                bestCost = cost;
                axis = curAxis;
            }

            assert(0 <= m && m < NumBins);
            n_l += minBins[m];
            n_r -= maxBins[m];
        }
    } // for(s32 curAxis=0;

    f32 separate = unit[axis] * (midBin + 1) + bbox.min_[axis];
    u32 mid = start + (numPrimitives >> 1);

#if 1
    s32 left = start;
    s32 right = end - 1;
    for(;;) {
        while(left < static_cast<s32>(end) && primitiveCentroids_[primitiveIndices_[left]][axis] <= separate) {
            ++left;
        }
        while(static_cast<s32>(start) <= right && separate < primitiveCentroids_[primitiveIndices_[right]][axis]) {
            --right;
        }
        if(right <= left) {
            mid = left;
            break;
        }
        swap(primitiveIndices_[left], primitiveIndices_[right]);
        ++left;
        --right;
    }
#else
    PrimitivePolicy::sort(numPrimitives, &primitiveIndices_[start], bestCentroids);
    for(s32 i = start; i < end; ++i) {
        if(separate < bestCentroids[primitiveIndices_[i]]) {
            mid = i;
            break;
        }
    }
#endif

    if(mid <= start || end <= mid) {
        splitMid(axis, num_l, num_r, bbox_l, bbox_r, area, start, numPrimitives, bbox);
    } else {

        getBBox(bbox_l, start, mid);
        getBBox(bbox_r, mid, end);

        num_l = mid - start;
        num_r = numPrimitives - num_l;
    }
}
} // namespace lray

