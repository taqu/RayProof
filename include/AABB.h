#ifndef INC_LRAY_AABB_H_
#define INC_LRAY_AABB_H_
#include "Vector.h"

namespace lray
{
class Ray;

class AABB
{
public:
    void setZero();
    void setInvalid();
    Vector3 extent() const;
    Vector3 diagonal() const;
    Vector3 centroid() const;
    void extend(const AABB& aabb);
    u32 maxExtentAxis() const;
    f32 halfArea() const;
    bool testRay(f32& tmin, f32& tmax, const Ray& ray, f32 t) const;
    bool testRay(f32& hitt, const Ray& ray, f32 tmin, f32 tmax) const;
    const Vector3& operator[](u32 index) const;
    Vector3& operator[](u32 index);

    Vector3 min_;
    Vector3 max_;
};
} // namespace lray
#endif // INC_LRAY_AABB_H_
