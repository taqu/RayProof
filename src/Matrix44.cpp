#include "Matrix44.h"
#include "Quaternion.h"
#include "Vector3.h"
#include "Vector4.h"

namespace lray
{
namespace
{
    __m128 dot(const __m128& v0, const __m128& v1)
    {
        __m128 tmp = _mm_mul_ps(v0, v1);
        tmp = _mm_add_ps(_mm_shuffle_ps(tmp, tmp, 0x4E), tmp);
        tmp = _mm_add_ps(_mm_shuffle_ps(tmp, tmp, 0xB1), tmp);
        tmp = _mm_shuffle_ps(tmp, tmp, 0);
        return tmp;
    }

    __m128 dotForLookAt(const __m128& v, const __m128& eye)
    {
        f32 f;
        *((u32*)&f) = 0x80000000U;
        __m128 mask = _mm_set1_ps(f);

        __m128 tmp = _mm_mul_ps(v, eye);
        tmp = _mm_add_ps(_mm_shuffle_ps(tmp, tmp, 0x4E), tmp);
        tmp = _mm_add_ps(_mm_shuffle_ps(tmp, tmp, 0xB1), tmp);
        tmp = _mm_xor_ps(tmp, mask);

        // x,y,z,v
        tmp = _mm_shuffle_ps(v, tmp, 0x0A);
        tmp = _mm_shuffle_ps(v, tmp, 0xC4);

        return tmp;
    }

    void rcp(__m128& r)
    {
#if 0
            //Newton-Raphson
            __m128 tmp = _mm_rcp_ss(r);
            r = _mm_mul_ss(r, tmp);
            r = _mm_mul_ss(r, tmp);
            tmp = _mm_add_ss(tmp, tmp);
            r = _mm_sub_ss(tmp, r);
            r = _mm_shuffle_ps(r, r, 0);
#else
        __m128 one = _mm_set_ss(1.0f);
        r = _mm_div_ss(one, r);
        r = _mm_shuffle_ps(r, r, 0);
#endif
    }

    void normalize(__m128& v)
    {
        __m128 r1 = v;
        v = _mm_mul_ps(v, v);
        v = _mm_add_ps(_mm_shuffle_ps(v, v, 0x4E), v);
        v = _mm_add_ps(_mm_shuffle_ps(v, v, 0xB1), v);

        v = _mm_sqrt_ss(v);
        v = _mm_shuffle_ps(v, v, 0);

        rcp(v);
        v = _mm_mul_ps(r1, v);
    }

    __m128 cross3(const __m128& v0, const __m128& v1)
    {
        __m128 xv0 = _mm_shuffle_ps(v0, v0, 0xC9);
        __m128 xv1 = _mm_shuffle_ps(v1, v1, 0xC9);
        __m128 tmp0 = _mm_fmsub_ps(v0, xv1, _mm_mul_ps(xv0, v1));
        tmp0 = _mm_shuffle_ps(tmp0, tmp0, 0xC9);
        return tmp0;
    }

    Vector4 cross3(const Vector4& v0, const Vector4& v1)
    {
        __m128 xv0 = _mm_loadu_ps(&v0.x_);
        __m128 xv1 = _mm_loadu_ps(&v1.x_);
        __m128 t0 = cross3(xv0, xv1);
        Vector4 r;
        _mm_storeu_ps(&r.x_, t0);
        return r;
    }
} // namespace

void Matrix44::lookAt(const Vector4& eye, const Vector4& at, const Vector4& up)
{
    __m128 xaxis, yaxis, zaxis, teye;
    teye = _mm_loadu_ps(&eye.x_);

    zaxis = _mm_loadu_ps(&at.x_);
    zaxis = _mm_sub_ps(zaxis, teye);

    normalize(zaxis);

    xaxis = cross3(_mm_loadu_ps(&up.x_), zaxis);
    normalize(xaxis);

    yaxis = cross3(zaxis, xaxis);

    xaxis = dotForLookAt(xaxis, teye);
    yaxis = dotForLookAt(yaxis, teye);
    zaxis = dotForLookAt(zaxis, teye);

    _mm_storeu_ps(&m_[0][0], xaxis);
    _mm_storeu_ps(&m_[1][0], yaxis);
    _mm_storeu_ps(&m_[2][0], zaxis);

    LRAY_ALIGN16 f32 buffer[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    __m128 t = _mm_load_ps(buffer);
    _mm_storeu_ps(&m_[3][0], t);
}

void Matrix44::lookAt(const Vector3& eye, const Vector3& at, const Vector3& up)
{
    Vector3 xaxis, yaxis, zaxis = normalize(at - eye);

    xaxis = normalize(cross(up, zaxis));
    yaxis = cross(zaxis, xaxis);

    m_[0][0] = xaxis.x_;
    m_[0][1] = xaxis.y_;
    m_[0][2] = xaxis.z_;
    m_[0][3] = -dot(eye, xaxis);
    m_[1][0] = yaxis.x_;
    m_[1][1] = yaxis.y_;
    m_[1][2] = yaxis.z_;
    m_[1][3] = -dot(eye, yaxis);
    m_[2][0] = zaxis.x_;
    m_[2][1] = zaxis.y_;
    m_[2][2] = zaxis.z_;
    m_[2][3] = -dot(eye, zaxis);
    m_[3][0] = 0.0f;
    m_[3][1] = 0.0f;
    m_[3][2] = 0.0f;
    m_[3][3] = 1.0f;
}

void Matrix44::lookAt(const Vector4& eye, const Quaternion& rotation)
{
    Vector4 zaxis = rotate(rotation, Vector4::Forward);
    Vector4 xaxis, yaxis;

    Vector4 up = (isEquals(zaxis.y_, 1.0f)) ? Vector4::Backward : Vector4::Up;
    xaxis = Vector4(normalize(cross3(up, zaxis)));
    yaxis = Vector4(cross3(zaxis, xaxis));

    m_[0][0] = xaxis.x_;
    m_[0][1] = xaxis.y_;
    m_[0][2] = xaxis.z_;
    m_[0][3] = -dot(eye, xaxis);
    m_[1][0] = yaxis.x_;
    m_[1][1] = yaxis.y_;
    m_[1][2] = yaxis.z_;
    m_[1][3] = -dot(eye, yaxis);
    m_[2][0] = zaxis.x_;
    m_[2][1] = zaxis.y_;
    m_[2][2] = zaxis.z_;
    m_[2][3] = -dot(eye, zaxis);
    m_[3][0] = 0.0f;
    m_[3][1] = 0.0f;
    m_[3][2] = 0.0f;
    m_[3][3] = 1.0f;
}

void Matrix44::lookAt(const Vector3& eye, const Quaternion& rotation)
{
    Vector3 zaxis = rotate(rotation, Vector3::Forward);
    Vector3 xaxis, yaxis;

    Vector3 up = (isEquals(zaxis.y_, 1.0f)) ? Vector3::Backward : Vector3::Up;
    xaxis = normalize(cross(up, zaxis));
    yaxis = cross(zaxis, xaxis);

    m_[0][0] = xaxis.x_;
    m_[0][1] = xaxis.y_;
    m_[0][2] = xaxis.z_;
    m_[0][3] = -dot(eye, xaxis);
    m_[1][0] = yaxis.x_;
    m_[1][1] = yaxis.y_;
    m_[1][2] = yaxis.z_;
    m_[1][3] = -dot(eye, yaxis);
    m_[2][0] = zaxis.x_;
    m_[2][1] = zaxis.y_;
    m_[2][2] = zaxis.z_;
    m_[2][3] = -dot(eye, zaxis);
    m_[3][0] = 0.0f;
    m_[3][1] = 0.0f;
    m_[3][2] = 0.0f;
    m_[3][3] = 1.0f;
}

void Matrix44::perspective(f32 width, f32 height, f32 znear, f32 zfar)
{
    f32 invDepth = zfar / (zfar - znear);
    m_[0][0] = 2.0f * znear / width;
    m_[0][1] = 0.0f;
    m_[0][2] = 0.0f;
    m_[0][3] = 0.0f;
    m_[1][0] = 0.0f;
    m_[1][1] = 2.0f * znear / height;
    m_[1][2] = 0.0f;
    m_[1][3] = 0.0f;
    m_[2][0] = 0.0f;
    m_[2][1] = 0.0f;
    m_[2][2] = invDepth;
    m_[2][3] = -znear * invDepth;
    m_[3][0] = 0.0f;
    m_[3][1] = 0.0f;
    m_[3][2] = 1.0f;
    m_[3][3] = 0.0f;
}

void Matrix44::perspectiveFov(f32 fovy, f32 aspect, f32 znear, f32 zfar)
{
    f32 yscale = 1.0f / ::tanf(0.5f * fovy);
    f32 xscale = yscale / aspect;
    f32 invDepth = zfar / (zfar - znear);

    m_[0][0] = xscale;
    m_[0][1] = 0.0f;
    m_[0][2] = 0.0f;
    m_[0][3] = 0.0f;
    m_[1][0] = 0.0f;
    m_[1][1] = yscale;
    m_[1][2] = 0.0f;
    m_[1][3] = 0.0f;
    m_[2][0] = 0.0f;
    m_[2][1] = 0.0f;
    m_[2][2] = invDepth;
    m_[2][3] = -znear * invDepth;
    m_[3][0] = 0.0f;
    m_[3][1] = 0.0f;
    m_[3][2] = 1.0f;
    m_[3][3] = 0.0f;
}

void Matrix44::perspectiveReverseZ(f32 width, f32 height, f32 znear, f32 zfar)
{
    f32 invDepth = znear / (znear - zfar);
    m_[0][0] = 2.0f * znear / width;
    m_[0][1] = 0.0f;
    m_[0][2] = 0.0f;
    m_[0][3] = 0.0f;
    m_[1][0] = 0.0f;
    m_[1][1] = 2.0f * znear / height;
    m_[1][2] = 0.0f;
    m_[1][3] = 0.0f;
    m_[2][0] = 0.0f;
    m_[2][1] = 0.0f;
    m_[2][2] = invDepth;
    m_[2][3] = -zfar * invDepth;
    m_[3][0] = 0.0f;
    m_[3][1] = 0.0f;
    m_[3][2] = 1.0f;
    m_[3][3] = 0.0f;
}

void Matrix44::perspectiveFovReverseZ(f32 fovy, f32 aspect, f32 znear, f32 zfar)
{
    f32 yscale = 1.0f / ::tanf(0.5f * fovy);
    f32 xscale = yscale / aspect;
    f32 invDepth = znear / (znear - zfar);

    m_[0][0] = xscale;
    m_[0][1] = 0.0f;
    m_[0][2] = 0.0f;
    m_[0][3] = 0.0f;
    m_[1][0] = 0.0f;
    m_[1][1] = yscale;
    m_[1][2] = 0.0f;
    m_[1][3] = 0.0f;
    m_[2][0] = 0.0f;
    m_[2][1] = 0.0f;
    m_[2][2] = invDepth;
    m_[2][3] = -zfar * invDepth;
    m_[3][0] = 0.0f;
    m_[3][1] = 0.0f;
    m_[3][2] = 1.0f;
    m_[3][3] = 0.0f;
}

} // namespace lray
