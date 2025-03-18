#ifndef INC_LRAY_SPHERE_H_
#define INC_LRAY_SPHERE_H_
#include "Core.h"
#include "Hittable.h"
#include "Vector.h"

namespace lray
{
struct HitRecord;

class Sphere: public Hittable
{
public:
    Sphere();
    Sphere(const Vector3& center, f32 radius);
    virtual ~Sphere();

    virtual bool hit(const Ray& ray, f32 tmin, f32 tmax, HitRecord& record) const override;

private:
    Vector3 center_;
    f32 radius_;
};
} // namespace lray
#endif // INC_LRAY_SPHERE_H_
