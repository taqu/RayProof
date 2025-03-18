#include "Plane.h"
#include "Vector.h"
#include "Ray.h"
#include "HitRecord.h"

namespace lray
{
Plane::Plane()
{
}

Plane::Plane(f32 nx, f32 ny, f32 nz, f32 d, const Material& material)
    :nx_(nx)
    ,ny_(ny)
    ,nz_(nz)
    ,d_(d)
    ,material_(material)
{
}

Plane::~Plane()
{
}

bool Plane::hit(const Ray& ray, f32 tmin, f32 tmax, HitRecord& record) const
{
    Vector3 n = {nx_, ny_, nz_};
    f32 t = d_ - dot(n, ray.origin_);
    t /= dot(n, ray.direction_);
    if(tmin<=t && t<=tmax){
        record.t_ = t;
        record.position_ = ray.pointAt(t);
        record.normal_ = n;
        return true;
    }
    return false;
}
} // namespace lray

