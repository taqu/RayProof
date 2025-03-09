#ifndef INC_LRAY_WORLD_H_
#define INC_LRAY_WORLD_H_
#include "Common.h"
#include "Array.h"
#include "LQBVH.h"
#include "BinQBVH.h"
#include "AABB.h"
#include "HitRecord.h"

namespace lray
{
    class Ray;
class Object;
class World
{
public:
    World();
    ~World();
    void clear();
    void add(Object* object);
    void build();
    HitRecord intersect(const Ray& ray, f32 tmin, f32 tmax);
private:
    Array<AABB> bounds_;
    Array<Object*> objects_;
    LQBVH topAcceleration_;
    Array<BinQBVH*> bottomAccelerations_;
};
} // namespace lray
#endif // INC_LRAY_WORLD_H_

