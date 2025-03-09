#ifndef INC_LRAY_HITTABLE_H_
#define INC_LRAY_HITTABLE_H_
#include "Common.h"

namespace lray
{
    class Ray;
    struct HitRecord;

class Hittable
{
public:
    virtual ~Hittable()
    {
    }

    virtual bool hit(const Ray& ray, f32 tmin, f32 tmax, HitRecord& record) const = 0;

protected:
    Hittable()
    {
    }
};
} // namespace lray
#endif // INC_LRAY_HITTABLE_H_
