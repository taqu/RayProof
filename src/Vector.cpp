#include "Vector.h"
#include <cmath>

namespace lray
{
//--- Vector2
//---------------------------------------------
const Vector2 Vector2::Zero = {0, 0};
const Vector2 Vector2::One = {1, 1};

f32 Vector2::lengthSqr() const
{
    return x_*x_ + y_*y_;
}

f32 Vector2::length() const
{
    return std::sqrtf(lengthSqr());
}

void Vector2::normalize()
{
    f32 l = lengthSqr();
    l = 1.0f / std::sqrtf(l);
    x_ *= l;
    y_ *= l;
}

void Vector2::safe_normalize(f32 epsilon)
{
    f32 l = lengthSqr();
    if(l <= epsilon) {
        x_ = 0;
        y_ = 0;
        return;
    }
    l = 1.0f / std::sqrtf(l);
    x_ *= l;
    y_ *= l;
}

Vector2& Vector2::operator+=(const Vector2& x)
{
    x_ += x.x_;
    y_ += x.y_;
    return *this;
}

Vector2& Vector2::operator-=(const Vector2& x)
{
    x_ -= x.x_;
    y_ -= x.y_;
    return *this;
}

Vector2& Vector2::operator*=(const Vector2& x)
{
    x_ *= x.x_;
    y_ *= x.y_;
    return *this;
}

Vector2& Vector2::operator/=(const Vector2& x)
{
    x_ /= x.x_;
    y_ /= x.y_;
    return *this;
}

Vector2& Vector2::operator*=(f32 x)
{
    x_ *= x;
    y_ *= x;
    return *this;
}

Vector2& Vector2::operator/=(f32 x)
{
    x_ /= x;
    y_ /= x;
    return *this;
}

Vector2 operator+(const Vector2& x0, const Vector2& x1)
{
    return {x0.x_+x1.x_, x0.y_+x1.y_};
}

Vector2 operator-(const Vector2& x0, const Vector2& x1)
{
    return {x0.x_-x1.x_, x0.y_-x1.y_};
}

Vector2 operator*(const Vector2& x0, const Vector2& x1)
{
    return {x0.x_*x1.x_, x0.y_*x1.y_};
}

Vector2 operator*(f32 x0, const Vector2& x1)
{
    return {x0*x1.x_, x0*x1.y_};
}

Vector2 operator*(const Vector2& x0, f32 x1)
{
    return {x0.x_*x1, x0.y_*x1};
}

Vector2 operator/(const Vector2& x0, const Vector2& x1)
{
    return {x0.x_/x1.x_, x0.y_/x1.y_};
}

Vector2 operator/(const Vector2& x0, f32 x1)
{
    return {x0.x_/x1, x0.y_/x1};
}

f32 dot(const Vector2& x0, const Vector2& x1)
{
    return x0.x_*x1.x_ + x0.y_*x1.y_;
}

Vector2 normalize2a(const Vector2& x)
{
    f32 l = x.lengthSqr();
    l = 1.0f / std::sqrtf(l);
    return {x.x_*l, x.y_*l};
}

Vector2 safe_normalize2(const Vector2& x, f32 epsilon)
{
    f32 l = x.lengthSqr();
    if(l <= epsilon) {
        return {};
    }
    l = 1.0f / std::sqrtf(l);
    return {x.x_*l, x.y_*l};
}

Vector2 randomOnDisk(f32 x0, f32 x1)
{
    f32 r0 = 2.0f * x0 - 1.0f;
    f32 r1 = 2.0f * x1 - 1.0f;
    f32 absR0 = std::abs(r0);
    f32 absR1 = std::abs(r1);
    f32 phi;
    f32 r;
    if(absR0 <= F32_EPSILON && absR1 <= F32_EPSILON) {
        r = 0.0f;
        phi = 0.0f;
    } else if(absR1 < absR0) {
        r = r0;
        phi = (F32_PI / 4.0f) * (r1 / r0);
    } else {
        r = r1;
        phi = (F32_PI / 2.0f) - (r0 / r1) * (F32_PI / 4.0f);
    }
    return {r * std::cosf(phi), r * std::sinf(phi)};
}

//--- Vector2u
//---------------------------------------------
const Vector2u Vector2u::Zero = {0, 0};
const Vector2u Vector2u::One = {1, 1};

u32 Vector2u::lengthSqr() const
{
    return x_ * x_ + y_ * y_;
}

f32 Vector2u::length() const
{
    return std::sqrtf(lengthSqr());
}

Vector2u& Vector2u::operator+=(const Vector2u& x)
{
    x_ += x.x_;
    y_ += x.y_;
    return *this;
}

Vector2u& Vector2u::operator-=(const Vector2u& x)
{
    x_ += x.x_;
    y_ += x.y_;
    return *this;
}

Vector2u& Vector2u::operator*=(const Vector2u& x)
{
    x_ += x.x_;
    y_ += x.y_;
    return *this;
}

Vector2u& Vector2u::operator/=(const Vector2u& x)
{
    x_ /= x.x_;
    y_ /= x.y_;
    return *this;
}

Vector2u operator+(const Vector2u& x0, const Vector2u& x1)
{
    return {x0.x_ + x1.x_, x0.y_ + x1.y_};
}

Vector2u operator-(const Vector2u& x0, const Vector2u& x1)
{
    return {x0.x_ - x1.x_, x0.y_ - x1.y_};
}

Vector2u operator*(const Vector2u& x0, const Vector2u& x1)
{
    return {x0.x_ * x1.x_, x0.y_ * x1.y_};
}

Vector2u operator/(const Vector2u& x0, const Vector2u& x1)
{
    return {x0.x_ / x1.x_, x0.y_ / x1.y_};
}

//--- Vector3
//---------------------------------------------
const Vector3 Vector3::Zero = {0.0f, 0.0f, 0.0f};
const Vector3 Vector3::One = {1.0f, 1.0f, 1.0f};
const Vector3 Vector3::Forward = {0.0f, 0.0f, 1.0f};
const Vector3 Vector3::Right = {1.0f, 1.0f, 1.0f};
const Vector3 Vector3::Up = {0.0f, 1.0f, 0.0f};

f32 Vector3::lengthSqr() const
{
    return x_*x_ + y_*y_ + z_*z_;
}

f32 Vector3::length() const
{
    return std::sqrtf(lengthSqr());
}

void Vector3::normalize()
{
    f32 l = lengthSqr();
    l = 1.0f / std::sqrtf(l);
    x_ *= l;
    y_ *= l;
    z_ *= l;
}

void Vector3::safe_normalize(f32 epsilon)
{
    f32 l = lengthSqr();
    if(l <= epsilon) {
        x_ = 0;
        y_ = 0;
        z_ = 0;
        return;
    }
    l = 1.0f / std::sqrtf(l);
    x_ *= l;
    y_ *= l;
    z_ *= l;
}

Vector3& Vector3::operator+=(const Vector3& x)
{
    x_ += x.x_;
    y_ += x.y_;
    z_ += x.z_;
    return *this;
}

Vector3& Vector3::operator-=(const Vector3& x)
{
    x_ -= x.x_;
    y_ -= x.y_;
    z_ -= x.z_;
    return *this;
}

Vector3& Vector3::operator*=(const Vector3& x)
{
    x_ *= x.x_;
    y_ *= x.y_;
    z_ *= x.z_;
    return *this;
}

Vector3& Vector3::operator/=(const Vector3& x)
{
    x_ /= x.x_;
    y_ /= x.y_;
    z_ /= x.z_;
    return *this;
}

Vector3& Vector3::operator*=(f32 x)
{
    x_ *= x;
    y_ *= x;
    z_ *= x;
    return *this;
}

Vector3& Vector3::operator/=(f32 x)
{
    x_ /= x;
    y_ /= x;
    z_ /= x;
    return *this;
}

Vector3 operator+(const Vector3& x0, const Vector3& x1)
{
    return {x0.x_+x1.x_, x0.y_+x1.y_, x0.z_+x1.z_};
}

Vector3 operator-(const Vector3& x0, const Vector3& x1)
{
    return {x0.x_-x1.x_, x0.y_-x1.y_, x0.z_-x1.z_};
}

Vector3 operator*(const Vector3& x0, const Vector3& x1)
{
    return {x0.x_*x1.x_, x0.y_*x1.y_, x0.z_*x1.z_};
}

Vector3 operator*(f32 x0, const Vector3& x1)
{
    return {x0*x1.x_, x0*x1.y_, x0*x1.z_};
}

Vector3 operator*(const Vector3& x0, f32 x1)
{
    return {x0.x_*x1, x0.y_*x1, x0.z_*x1};
}

Vector3 operator/(const Vector3& x0, const Vector3& x1)
{
    return {x0.x_/x1.x_, x0.y_/x1.y_, x0.z_/x1.z_};
}

Vector3 operator/(const Vector3& x0, f32 x1)
{
    return {x0.x_/x1, x0.y_/x1, x0.z_/x1};
}

Vector3 normalize3(const Vector3& x)
{
    f32 l = x.lengthSqr();
    l = 1.0f / std::sqrtf(l);
    return {x.x_*l, x.y_*l, x.z_*l};
}

Vector3 safe_normalize3(const Vector3& x, f32 epsilon)
{
    f32 l = x.lengthSqr();
    if(l <= epsilon) {
        return {};
    }
    l = 1.0f / std::sqrtf(l);
    return {x.x_*l, x.y_*l, x.z_*l};
}

Vector3 hadamardMul(const Vector3& x0, const Vector3& x1)
{
    return {x0.x_ * x1.x_, x0.y_ * x1.y_, x0.z_ * x1.z_};
}

f32 dot(const Vector3& x0, const Vector3& x1)
{
    return x0.x_ * x1.x_ + x0.y_ * x1.y_ + x0.z_ * x1.z_;
}

Vector3 cross(const Vector3& x0, const Vector3& x1)
{
    f32 x = x0.y_ * x1.z_ - x0.z_ * x1.y_;
    f32 y = x0.z_ * x1.x_ - x0.x_ * x1.z_;
    f32 z = x0.x_ * x1.y_ - x0.y_ * x1.x_;
    return {x, y, z};
}

Vector3 minimum(const Vector3& x0, const Vector3& x1)
{
    f32 x = minimum(x0.x_, x1.x_);
    f32 y = minimum(x0.y_, x1.y_);
    f32 z = minimum(x0.z_, x1.z_);
    return {x, y, z};
}

Vector3 maximum(const Vector3& x0, const Vector3& x1)
{
    f32 x = maximum(x0.x_, x1.x_);
    f32 y = maximum(x0.y_, x1.y_);
    f32 z = maximum(x0.z_, x1.z_);
    return {x, y, z};
}

Vector3 saturate(const Vector3& x)
{
    f32 rx = saturate(x.x_);
    f32 ry = saturate(x.y_);
    f32 rz = saturate(x.z_);
    return {rx, ry, rz};
}

Vector3 lerp(const Vector3& x0, const Vector3& x1, f32 t)
{
    assert(0.0f <= t && t <= 1.0f);
    f32 t0 = 1.0f - t;
    f32 t1 = t;
    return x0 * t0 + x1 * t1;
}

void orthonormalBasis(Vector3& b0, Vector3& b1, const Vector3& n)
{
    if(n.z_ < -0.999999f) {
        b0 = {0, -1, 0};
        b1 = {-1, 0, 0};
        return;
    }
    f32 a = 1.0f / (1.0f + n.z_);
    f32 b = -n.x_ * n.y_ * a;
    b0 = {1.0f - n.x_ * n.x_ * a, b, -n.x_};
    b1 = {b, 1.0f - n.y_ * n.y_ * a, -n.y_};
}

Vector3 randomInSphere(f32 x0, f32 x1, f32 x2)
{
    f32 theta = 2.0f * x0 - 1.0f;
    f32 r = ::sqrtf(1.0f - theta * theta);
    f32 phi = (F32_PI * 2.0f) * x1;
    f32 sn = ::sinf(phi);
    f32 cs = ::cosf(phi);
    r *= x2;
    return {r * cs, r * sn, x2 * theta};
}

Vector3 randomOnSphere(f32 x0, f32 x1)
{
    f32 theta = 2.0f * x0 - 1.0f;
    f32 r = ::sqrtf(1.0f - theta * theta);
    f32 phi = (F32_PI * 2.0f) * x1;
    f32 sn = ::sinf(phi);
    f32 cs = ::cosf(phi);
    return {r * cs, r * sn, theta};
}

Vector3 randomOnHemiSphere(f32 x0, f32 x1)
{
    f32 theta = x0;
    f32 r = ::sqrtf(1.0f - theta * theta);
    f32 phi = (F32_PI * 2.0f) * x1;
    f32 sn = ::sinf(phi);
    f32 cs = ::cosf(phi);
    return {r * cs, r * sn, theta};
}

Vector3 randomOnHemiSphereAround(f32 x0, f32 x1, const Vector3& n)
{
    Vector3 x = randomOnHemiSphere(x0, x1);
    f32 t = dot(x, n);
    if(t < 0.0f) {
        return -x;
    }
    return x;
}

Vector3 randomOnCosineHemiSphere(f32 x0, f32 x1)
{
    Vector2 p = randomOnDisk(x0, x1);
    f32 z = std::max(0.0f, ::sqrtf(std::max(F32_EPSILON, 1.0f - p.x_ * p.x_ - p.y_ * p.y_)));
    return {p.x_, p.y_, z};
}

Vector3 randomOnCosineHemiSphereAround(f32 x0, f32 x1, const Vector3& n)
{
    Vector3 x = randomOnCosineHemiSphere(x0, x1);
    Vector3 b0, b1;
    orthonormalBasis(b0, b1, n);
    f32 rx = x.x_ * b0.x_ + x.y_ * n.x_ + x.z_ * b1.x_;
    f32 ry = x.x_ * b0.y_ + x.y_ * n.y_ + x.z_ * b1.y_;
    f32 rz = x.x_ * b0.z_ + x.y_ * n.z_ + x.z_ * b1.z_;
    return {rx, ry, rz};
}

Vector3 randomCone(f32 x0, f32 x1, f32 cosCutoff)
{
    f32 cosTheta = (1.0f - x0) + x0 * cosCutoff;
    f32 sinTheta = ::sqrtf(std::max(F32_EPSILON, (1.0f - cosTheta * cosTheta)));
    f32 phi = 2.0f * F32_PI * x1;
    f32 sinPhi = ::sinf(phi);
    f32 cosPhi = ::cosf(phi);
    return {cosPhi * sinTheta, sinPhi * sinTheta, cosTheta};
}

Vector3 reflect(const Vector3& x, const Vector3& n)
{
    return x - 2.0f * dot(x, n) * n;
}

bool refract(Vector3& refracted, const Vector3& x, const Vector3& n, f32 niOverNt)
{
    f32 dt = dot(x, n);
    f32 discriminant = 1.0f - niOverNt * niOverNt * (1.0f - dt * dt);
    if(0.0f < discriminant) {
        refracted = (niOverNt * (x - dt * n)) - ::sqrtf(discriminant) * n;
        return true;
    }
    return false;
}

//--- Vector4
//---------------------------------------------
Vector4 Vector4::Zero = {0, 0, 0, 0};
Vector4 Vector4::One = {1, 1, 1, 1};

f32 Vector4::lengthSqr() const
{
    return x_*x_ + y_*y_ + z_*z_ + w_*w_;
}

f32 Vector4::length() const
{
    return std::sqrtf(lengthSqr());
}

void Vector4::normalize()
{
    f32 l = lengthSqr();
    l = 1.0f / std::sqrtf(l);
    x_ *= l;
    y_ *= l;
    z_ *= l;
    w_ *= l;
}

void Vector4::safe_normalize(f32 epsilon)
{
    f32 l = lengthSqr();
    if(l <= epsilon) {
        x_ = 0;
        y_ = 0;
        z_ = 0;
        w_ = 0;
        return;
    }
    l = 1.0f / std::sqrtf(l);
    x_ *= l;
    y_ *= l;
    z_ *= l;
    w_ *= l;
}

Vector4& Vector4::operator+=(const Vector4& x)
{
    x_ += x.x_;
    y_ += x.y_;
    z_ += x.z_;
    w_ += x.w_;
    return *this;
}

Vector4& Vector4::operator-=(const Vector4& x)
{
    x_ -= x.x_;
    y_ -= x.y_;
    z_ -= x.z_;
    w_ -= x.w_;
    return *this;
}

Vector4& Vector4::operator*=(const Vector4& x)
{
    x_ *= x.x_;
    y_ *= x.y_;
    z_ *= x.z_;
    w_ *= x.w_;
    return *this;
}

Vector4& Vector4::operator/=(const Vector4& x)
{
    x_ /= x.x_;
    y_ /= x.y_;
    z_ /= x.z_;
    w_ /= x.w_;
    return *this;
}

Vector4& Vector4::operator*=(f32 x)
{
    x_ *= x;
    y_ *= x;
    z_ *= x;
    w_ *= x;
    return *this;
}

Vector4& Vector4::operator/=(f32 x)
{
    x_ /= x;
    y_ /= x;
    z_ /= x;
    w_ /= x;
    return *this;
}

Vector4 operator+(const Vector4& x0, const Vector4& x1)
{
    return {x0.x_+x1.x_, x0.y_+x1.y_, x0.z_+x1.z_, x0.w_+x1.w_};
}

Vector4 operator-(const Vector4& x0, const Vector4& x1)
{
    return {x0.x_-x1.x_, x0.y_-x1.y_, x0.z_-x1.z_, x0.w_-x1.w_};
}

Vector4 operator*(const Vector4& x0, const Vector4& x1)
{
    return {x0.x_*x1.x_, x0.y_*x1.y_, x0.z_*x1.z_, x0.w_*x1.w_};
}

Vector4 operator*(f32 x0, const Vector4& x1)
{
    return {x0*x1.x_, x0*x1.y_, x0*x1.z_, x0*x1.w_};
}

Vector4 operator*(const Vector4& x0, f32 x1)
{
    return {x0.x_*x1, x0.y_*x1, x0.z_*x1, x0.w_*x1};
}

Vector4 operator/(const Vector4& x0, const Vector4& x1)
{
    return {x0.x_/x1.x_, x0.y_/x1.y_, x0.z_/x1.z_, x0.w_/x1.w_};
}

Vector4 operator/(const Vector4& x0, f32 x1)
{
    return {x0.x_/x1, x0.y_/x1, x0.z_/x1, x0.w_/x1};
}

Vector4 hadamardMul(const Vector4& x0, const Vector4& x1)
{
    return {x0.x_ * x1.x_, x0.y_ * x1.y_, x0.z_ * x1.z_, x0.w_ * x1.w_};
}

f32 dot(const Vector4& x0, const Vector4& x1)
{
    return x0.x_*x1.x_ + x0.y_*x1.y_ + x0.z_*x1.z_ + x0.w_*x1.w_;
}

Vector4 normalize4(const Vector4& x)
{
    f32 l = x.lengthSqr();
    l = 1.0f / std::sqrtf(l);
    return {x.x_*l, x.y_*l, x.z_*l, x.w_*l};
}

Vector4 safe_normalize4(const Vector4& x, f32 epsilon)
{
    f32 l = x.lengthSqr();
    if(l <= epsilon) {
        return {};
    }
    l = 1.0f / std::sqrtf(l);
    return {x.x_*l, x.y_*l, x.z_*l, x.w_*l};
}
} // namespace ray
