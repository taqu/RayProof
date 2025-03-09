#ifndef INC_LRAY_LQBVH_H_
#define INC_LRAY_LQBVH_H_
#include <immintrin.h>
#include <functional>
#include "Core.h"
#include "HitRecord.h"
#include "Array.h"
#include "AABB.h"

namespace lray
{
    class AABB;
    class Ray;

class LQBVH
{
public:
    inline static constexpr f32 Epsilon = 1.0e-6f;
    inline static constexpr u32 MinLeafPrimitives = 1;
    inline static constexpr u32 MaxDepth = 16;
    inline static constexpr u32 NumSplits = 1024;

    struct Joint
    {
        __m128 bbox_[2][3];
        u32 children_;
        u8 axis0_;
        u8 axis1_;
        u8 axis2_;
        u8 flags_;
    };

    struct Leaf
    {
        u32 padding0_[22];
        u32 start_;
        u32 size_;
        u32 children_;
        u8 axis0_;
        u8 axis1_;
        u8 axis2_;
        u8 flags_;
    };

    union Node
    {
        inline static constexpr u8 LeafFlag = (0x01U<<7);
        bool isLeaf() const;
        void setLeaf(u32 start, u32 size);
        void setJoint(u32 child, const AABB bbox[4], const u8 axis[3]);
        u32 getPrimitiveIndex() const;
        u32 getNumPrimitives() const;
        Joint joint_;
        Leaf leaf_;
    };

    struct Work
    {
        u32 start_;
        u32 numPrimitives_;
        u32 node_;
        u32 depth_;
    };

    LQBVH();
    ~LQBVH();
    void build(u32 numPrimitives, const AABB* primitives);
    HitRecord intersect(const Ray& ray, f32 tmin, f32 tmax, std::function<HitRecord(const Ray& ray,u32,f32,f32)> func);

private:
    LQBVH(const LQBVH&) = delete;
    LQBVH& operator=(const LQBVH&) = delete;
    static Vector3 calcInvUnit(const AABB& bbox);
    static u32 calcMortonCode3(const Vector3& x, const Vector3& invUnit, const AABB& bbox);
    void recursiveConstruct(u32 numPrimitives);
    void getBBox(AABB& bbox, u32 start, u32 end);
    void findSplit(u32& split, u8& axis, const u32* codes, u32 first, u32 last);

    u32 depth_;
    Array<Node> nodes_;
    Array<u32> indexCodes_;
    Array<AABB> primitives_;
};
} // namespace lray
#endif // INC_LRAY_LQBVH_H_
