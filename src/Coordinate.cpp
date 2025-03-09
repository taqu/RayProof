#include "Coordinate.h"

namespace lray
{
Coordinate Coordinate::create(const Vector3& normal)
{
    if(normal.z_ < -0.9999999f) {
        return {normal, {0.0f, -1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}};
    }
    f32 a = 1.0f / (1.0f + normal.z_);
    f32 b = -normal.x_ * normal.y_ * a;
    Vector3 binormal0 = {1.0f - normal.x_ * normal.x_ * a, b, -normal.x_};
    Vector3 binormal1 = {b, 1.0f - normal.y_ * normal.y_ * a, -normal.y_};
    return {normal, binormal0, binormal1};
}

Vector3 Coordinate::worldToLocal(const Vector3& x)
{
    Vector3 r = {dot(x, binormal0_), dot(x, binormal1_), dot(x, normal_)};
    return normalize3(r);
}

Vector3 Coordinate::localToWorld(const Vector3& x)
{
    f32 rx = binormal0_.x_ * x.x_ + binormal1_.x_ * x.y_ + normal_.x_ * x.z_;
    f32 ry = binormal0_.y_ * x.x_ + binormal1_.y_ * x.y_ + normal_.y_ * x.z_;
    f32 rz = binormal0_.z_ * x.x_ + binormal1_.z_ * x.y_ + normal_.z_ * x.z_;
    return {rx, ry, rz};
}

} // namespace lray
