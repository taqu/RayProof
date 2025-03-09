#ifndef INC_LRAY_MATRIX44_H_
#define INC_LRAY_MATRIX44_H_
#include "Common.h"

namespace lray
{
class Vector3;
class Vector4;
class Quaternion;

class Matrix44
{
public:
    void lookAt(const Vector4& eye, const Vector4& at, const Vector4& up);
    void lookAt(const Vector3& eye, const Vector3& at, const Vector3& up);
    void lookAt(const Vector4& eye, const Quaternion& rotation);
    void lookAt(const Vector3& eye, const Quaternion& rotation);

    void perspective(f32 width, f32 height, f32 znear, f32 zfar);
    void perspectiveFov(f32 fovy, f32 aspect, f32 znear, f32 zfar);
    void perspectiveReverseZ(f32 width, f32 height, f32 znear, f32 zfar);
    void perspectiveFovReverseZ(f32 fovy, f32 aspect, f32 znear, f32 zfar);

    f32 m_[4][4];
};
} // namespace lray
#endif // INC_LRAY_MATRIX44_H_
