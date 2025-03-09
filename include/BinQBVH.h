#ifndef INC_LRAY_BINQBVH_H_
#define INC_LRAY_BINQBVH_H_
#include "Core.h"
#include "Array.h"
#include "AABB.h"
#include "HitRecord.h"

namespace lray
{
    class Object;
    class Ray;

    class BinQBVH
    {
    public:
        inline static constexpr f32 Epsilon = 1.0e-6f;
        inline static constexpr u32 MinLeafPrimitives = 4;
        inline static constexpr u32 NumBins = 32;
        inline static constexpr u32 MaxDepth = 24;
        inline static constexpr u32 MaxNodes = 0xFFFFFF-4;
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

            void setJoint(u32 child, const AABB bbox[4], u8 axis[3]);

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
            AABB bbox_;
        };

        BinQBVH();
        ~BinQBVH();

        void build(const Object* object);
        HitRecord intersect(const Ray& ray, f32 tmin, f32 tmax);
        u32 getDepth() const;
    private:
        BinQBVH(const BinQBVH&) = delete;
        BinQBVH& operator=(const BinQBVH&) = delete;

        static const u32 MaxWorks = MaxDepth<<2;

        void getBBox(AABB& bbox, u32 start, u32 end);

        void recursiveConstruct(u32 numPrimitives, const AABB& bbox);
        void splitMid(u8& axis, u32& num_l, u32& num_r, AABB& bbox_l, AABB& bbox_r, f32 area, u32 start, u32 numPrimitives, const AABB& bbox);
        void splitBinned(u8& axis, u32& num_l, u32& num_r, AABB& bbox_l, AABB& bbox_r, f32 area, u32 start, u32 numPrimitives, const AABB& bbox);

        f32 SAH_KI_;
        f32 SAH_KT_;
        const Object* object_;

        u32 depth_;
        Array<Node> nodes_;
        Array<u32> primitiveIndices_;
        Array<Vector3> primitiveCentroids_;
        Array<AABB> primitiveBBoxes_;
        Work works_[MaxWorks];
    };
}
#endif //INC_LRAY_BINQBVH_H_
