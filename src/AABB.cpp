#include "AABB.h"
#include "Ray.h"

namespace lray
{
void AABB::setZero()
{
    min_.x_ = min_.y_ = min_.z_ = 0.0f;
    max_.x_ = max_.y_ = max_.z_ = 0.0f;
}

void AABB::setInvalid()
{
    min_.x_ = min_.y_ = min_.z_ = FLT_MAX;
    max_.x_ = max_.y_ = max_.z_ = -FLT_MAX;
}

Vector3 AABB::extent() const
{
    return max_ - min_;
}

Vector3 AABB::diagonal() const
{
    return {max_.x_ - min_.x_, max_.y_ - min_.y_, max_.z_ - min_.z_};
}

Vector3 AABB::centroid() const
{
    Vector3 c = min_ + max_;
    return c*0.5f;
}

void AABB::extend(const AABB& aabb)
{
    min_.x_ = minimum(min_.x_, aabb.min_.x_);
    min_.y_ = minimum(min_.y_, aabb.min_.y_);
    min_.z_ = minimum(min_.z_, aabb.min_.z_);

    max_.x_ = maximum(max_.x_, aabb.max_.x_);
    max_.y_ = maximum(max_.y_, aabb.max_.y_);
    max_.z_ = maximum(max_.z_, aabb.max_.z_);
}

u32 AABB::maxExtentAxis() const
{
    Vector3 extent = max_ - min_;
    u32 axis = (extent.x_ < extent.y_) ? 1 : 0;
    axis = (extent.z_ < extent[axis]) ? axis : 2;
    return axis;
}

f32 AABB::halfArea() const
{
    f32 dx = max_.x_ - min_.x_;
    f32 dy = max_.y_ - min_.y_;
    f32 dz = max_.z_ - min_.z_;
    return (dx * dy + dy * dz + dz * dx);
}

bool AABB::testRay(f32& tmin, f32& tmax, const Ray& ray, f32 t) const
{
    tmin = 0.0f;
    tmax = t;
    f32 invDirection[3];
    invDirection[0] = 1.0f/ray.direction_[0];
    invDirection[1] = 1.0f/ray.direction_[1];
    invDirection[2] = 1.0f/ray.direction_[2];

    for(u32 i = 0; i < 3; ++i) {
        if(absolute(ray.direction_[i]) < HitEpsilon) {
            if(ray.origin_[i] < min_[i] || max_[i] < ray.origin_[i]) {
                return false;
            }

        } else {
            f32 invD = invDirection[i];
            f32 t1 = (min_[i] - ray.origin_[i]) * invD;
            f32 t2 = (max_[i] - ray.origin_[i]) * invD;

            if(t1 > t2) {
                if(t2 > tmin){
                    tmin = t2;
                }
                if(t1 < tmax){
                    tmax = t1;
                }
            } else {
                if(t1 > tmin){
                    tmin = t1;
                }
                if(t2 < tmax){
                    tmax = t2;
                }
            }

            if(tmin > tmax) {
                return false;
            }
            if(tmax < 0.0f) {
                return false;
            }
        }
    }
    return true;
}

bool AABB::testRay(f32& hitt, const Ray& ray, f32 tmin, f32 tmax) const
{
    f32 invDirection[3];
    invDirection[0] = 1.0f/ray.direction_[0];
    invDirection[1] = 1.0f/ray.direction_[1];
    invDirection[2] = 1.0f/ray.direction_[2];

    for(u32 i = 0; i < 3; ++i) {
        if(absolute(ray.direction_[i]) < HitEpsilon) {
            if(ray.origin_[i] < min_[i] || max_[i] < ray.origin_[i]) {
                return false;
            }

        } else {
            f32 invD = invDirection[i];
            f32 t1 = (min_[i] - ray.origin_[i]) * invD;
            f32 t2 = (max_[i] - ray.origin_[i]) * invD;

            if(t1 > t2) {
                if(t2 > tmin){
                    tmin = t2;
                }
                if(t1 < tmax){
                    tmax = t1;
                }
            } else {
                if(t1 > tmin){
                    tmin = t1;
                }
                if(t2 < tmax){
                    tmax = t2;
                }
            }

            if(tmin > tmax) {
                return false;
            }
            if(tmax < 0.0f) {
                return false;
            }
        }
    }
    hitt = tmin;
    return true;
}

const Vector3& AABB::operator[](u32 index) const
{
    return reinterpret_cast<const Vector3*>(this)[index];
}

Vector3& AABB::operator[](u32 index)
{
    return reinterpret_cast<Vector3*>(this)[index];
}
} // namespace lray
