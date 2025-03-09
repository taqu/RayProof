#ifndef INC_LRAY_QUATERNION_H_
#define INC_LRAY_QUATERNION_H_
#include "Common.h"
#include "Vector3.h"
#include "Vector4.h"

namespace lray
{
class Quaternion
{
public:
    static Quaternion rotateX(f32 radian);
    static Quaternion rotateY(f32 radian);
    static Quaternion rotateZ(f32 radian);
    static Quaternion rotateAxis(const Vector3& axis, f32 radian);
    static Quaternion rotateAxis(const Vector4& axis, f32 radian);
    static Quaternion rotateAxis(f32 x, f32 y, f32 z, f32 radian);

    f32 w_;
    f32 x_;
    f32 y_;
    f32 z_;
};

Quaternion conjugate(const Quaternion& q);
Quaternion normalize(const Quaternion& q);

Quaternion mul(const Quaternion& q0, const Quaternion& q1);
Quaternion mul(f32 a, const Quaternion& q);
Quaternion mul(const Vector3& v, const Quaternion& q);
Quaternion mul(const Quaternion& q, const Vector3& v);
Quaternion mul(const Vector4& v, const Quaternion& q);
Quaternion mul(const Quaternion& q, const Vector4& v);
Vector3 rotate(const Vector3& v, const Quaternion& rotation);
Vector3 rotate(const Quaternion& rotation, const Vector3& v);
Vector4 rotate(const Vector4& v, const Quaternion& rotation);
Vector4 rotate(const Quaternion& rotation, const Vector4& v);
} // namespace lray
#endif // INC_LRAY_QUATERNION_H_
