#include "Quaternion.h"
#include "Vector3.h"

namespace lray
{
namespace
{
    // LRAY_ALIGN16 u32 QuaternionConjugateMask_[4] =
    //{
    //     0x00000000U,
    //     0x80000000U,
    //     0x80000000U,
    //     0x80000000U,
    // };

    LRAY_ALIGN16 u32 QuaternionMulMask0_[4] =
        {
            0x80000000U,
            0x00000000U,
            0x00000000U,
            0x80000000U,
    };

    LRAY_ALIGN16 u32 QuaternionMulMask1_[4] =
        {
            0x80000000U,
            0x80000000U,
            0x00000000U,
            0x00000000U,
    };

    LRAY_ALIGN16 u32 QuaternionMulMask2_[4] =
        {
            0x80000000U,
            0x00000000U,
            0x80000000U,
            0x00000000U,
    };
} // namespace

Quaternion conjugate(const Quaternion& q)
{
    return {q.w_, -q.x_, -q.y_, -q.z_};
}

Quaternion normalize(const Quaternion& q)
{
    __m128 r0 = _mm_loadu_ps(&q.x_);
    __m128 r1 = _mm_mul_ps(r0, r0);
    r1 = _mm_add_ps(_mm_shuffle_ps(r1, r1, 0x4E), r1);
    r1 = _mm_add_ps(_mm_shuffle_ps(r1, r1, 0xB1), r1);

    r1 = _mm_sqrt_ss(r1);
    r1 = _mm_shuffle_ps(r1, r1, 0);
    r0 = _mm_div_ps(r0, r1);
    Quaternion r;
    _mm_storeu_ps(&r.w_, r0);
    return r;
}

Quaternion Quaternion::rotateX(f32 radian)
{
    f32 over2 = radian * 0.5f;
    return {::cosf(over2), ::sinf(over2), 0.0f, 0.0f};
}

Quaternion Quaternion::rotateY(f32 radian)
{
    f32 over2 = radian * 0.5f;
    return {::cosf(over2), 0.0f, ::sinf(over2), 0.0f};
}

Quaternion Quaternion::rotateZ(f32 radian)
{
    f32 over2 = radian * 0.5f;
    return {::cosf(over2), 0.0f, 0.0f, ::sinf(over2)};
}

Quaternion Quaternion::rotateAxis(const Vector3& axis, f32 radian)
{
    return rotateAxis(axis.x_, axis.y_, axis.z_, radian);
}

Quaternion Quaternion::rotateAxis(const Vector4& axis, f32 radian)
{
    return rotateAxis(axis.x_, axis.y_, axis.z_, radian);
}

Quaternion Quaternion::rotateAxis(f32 x, f32 y, f32 z, f32 radian)
{
    f32 over2 = radian * 0.5f;
    f32 sinOver2 = ::sinf(over2);
    return {::cosf(over2), x * sinOver2, y * sinOver2, z * sinOver2};
}

Quaternion mul(const Quaternion& q0, const Quaternion& q1)
{
    __m128 mask;

    __m128 tw = _mm_set1_ps(q0.w_);

    __m128 tx = _mm_set1_ps(q0.x_);
    mask = _mm_load_ps((f32*)QuaternionMulMask0_);
    tx = _mm_xor_ps(tx, mask);

    __m128 ty = _mm_set1_ps(q0.y_);
    mask = _mm_load_ps((f32*)QuaternionMulMask1_);
    ty = _mm_xor_ps(ty, mask);

    __m128 tz = _mm_set1_ps(q0.z_);
    mask = _mm_load_ps((f32*)QuaternionMulMask2_);
    tz = _mm_xor_ps(tz, mask);

    __m128 t0 = _mm_loadu_ps(&q1.w_);
    __m128 t1 = _mm_shuffle_ps(t0, t0, 0xB1);
    __m128 t2 = _mm_shuffle_ps(t0, t0, 0x4E);
    __m128 t3 = _mm_shuffle_ps(t0, t0, 0x1B);

    t0 = _mm_mul_ps(t0, tw);
    t0 = _mm_fmadd_ps(tx, t1, t0);
    t0 = _mm_fmadd_ps(ty, t2, t0);
    t0 = _mm_fmadd_ps(tz, t3, t0);
    // t0 = _mm_add_ps(t0, _mm_mul_ps(tx, t1));
    // t0 = _mm_add_ps(t0, _mm_mul_ps(ty, t2));
    // t0 = _mm_add_ps(t0, _mm_mul_ps(tz, t3));
    Quaternion r;
    _mm_storeu_ps(&r.w_, t0);
    return r;
}

Quaternion mul(f32 a, const Quaternion& q)
{
    __m128 r0 = _mm_set1_ps(a);
    __m128 r1 = _mm_loadu_ps(&q.w_);
    r0 = _mm_mul_ps(r0, r1);
    Quaternion r;
    _mm_storeu_ps(&r.w_, r0);
    return r;
}

Quaternion mul(const Vector3& v, const Quaternion& q)
{
    __m128 mask;

    __m128 tx = _mm_set1_ps(v.x_);
    mask = _mm_load_ps((f32*)QuaternionMulMask0_);
    tx = _mm_xor_ps(tx, mask);

    __m128 ty = _mm_set1_ps(v.y_);
    mask = _mm_load_ps((f32*)QuaternionMulMask1_);
    ty = _mm_xor_ps(ty, mask);

    __m128 tz = _mm_set1_ps(v.z_);
    mask = _mm_load_ps((f32*)QuaternionMulMask2_);
    tz = _mm_xor_ps(tz, mask);

    __m128 t0 = _mm_loadu_ps(&q.w_);
    __m128 t1 = _mm_shuffle_ps(t0, t0, 0xB1);
    __m128 t2 = _mm_shuffle_ps(t0, t0, 0x4E);
    __m128 t3 = _mm_shuffle_ps(t0, t0, 0x1B);

    t0 = _mm_mul_ps(tx, t1);
    t0 = _mm_fmadd_ps(ty, t2, t0);
    t0 = _mm_fmadd_ps(tz, t3, t0);
    // t0 = _mm_add_ps(t0, _mm_mul_ps(ty, t2));
    // t0 = _mm_add_ps(t0, _mm_mul_ps(tz, t3));
    Quaternion r;
    _mm_storeu_ps(&r.w_, t0);
    return r;
}

Quaternion mul(const Quaternion& q, const Vector3& v)
{
    __m128 mask;

    __m128 tw = _mm_set1_ps(q.w_);

    __m128 tx = _mm_set1_ps(q.x_);
    mask = _mm_load_ps((f32*)QuaternionMulMask0_);
    tx = _mm_xor_ps(tx, mask);

    __m128 ty = _mm_set1_ps(q.y_);
    mask = _mm_load_ps((f32*)QuaternionMulMask1_);
    ty = _mm_xor_ps(ty, mask);

    __m128 tz = _mm_set1_ps(q.z_);
    mask = _mm_load_ps((f32*)QuaternionMulMask2_);
    tz = _mm_xor_ps(tz, mask);

    __m128 t0 = _mm_load_ss(&v.x_);
    __m128 t = _mm_loadl_pi(t0, reinterpret_cast<const __m64*>(&v.y_));
    t0 = _mm_shuffle_ps(t0, t, _MM_SHUFFLE(1, 0, 0, 1));

    __m128 t1 = _mm_shuffle_ps(t0, t0, 0xB1);
    __m128 t2 = _mm_shuffle_ps(t0, t0, 0x4E);
    __m128 t3 = _mm_shuffle_ps(t0, t0, 0x1B);

    t0 = _mm_mul_ps(t0, tw);
    t0 = _mm_fmadd_ps(tx, t1, t0);
    t0 = _mm_fmadd_ps(ty, t2, t0);
    t0 = _mm_fmadd_ps(tz, t3, t0);
    // t0 = _mm_add_ps(t0, _mm_mul_ps(tx, t1));
    // t0 = _mm_add_ps(t0, _mm_mul_ps(ty, t2));
    // t0 = _mm_add_ps(t0, _mm_mul_ps(tz, t3));
    Quaternion r;
    _mm_storeu_ps(&r.w_, t0);
    return r;
}

Quaternion mul(const Vector4& v, const Quaternion& q)
{
    __m128 mask;

    __m128 tx = _mm_set1_ps(v.x_);
    mask = _mm_load_ps((f32*)QuaternionMulMask0_);
    tx = _mm_xor_ps(tx, mask);

    __m128 ty = _mm_set1_ps(v.y_);
    mask = _mm_load_ps((f32*)QuaternionMulMask1_);
    ty = _mm_xor_ps(ty, mask);

    __m128 tz = _mm_set1_ps(v.z_);
    mask = _mm_load_ps((f32*)QuaternionMulMask2_);
    tz = _mm_xor_ps(tz, mask);

    __m128 t0 = _mm_loadu_ps(&q.w_);
    __m128 t1 = _mm_shuffle_ps(t0, t0, 0xB1);
    __m128 t2 = _mm_shuffle_ps(t0, t0, 0x4E);
    __m128 t3 = _mm_shuffle_ps(t0, t0, 0x1B);

    t0 = _mm_mul_ps(tx, t1);
    t0 = _mm_fmadd_ps(ty, t2, t0);
    t0 = _mm_fmadd_ps(tz, t3, t0);
    // t0 = _mm_add_ps(t0, _mm_mul_ps(ty, t2));
    // t0 = _mm_add_ps(t0, _mm_mul_ps(tz, t3));
    Quaternion r;
    _mm_storeu_ps(&r.w_, t0);
    return r;
}

Quaternion mul(const Quaternion& q, const Vector4& v)
{
    __m128 mask;

    __m128 tw = _mm_set1_ps(q.w_);

    __m128 tx = _mm_set1_ps(q.x_);
    mask = _mm_load_ps((f32*)QuaternionMulMask0_);
    tx = _mm_xor_ps(tx, mask);

    __m128 ty = _mm_set1_ps(q.y_);
    mask = _mm_load_ps((f32*)QuaternionMulMask1_);
    ty = _mm_xor_ps(ty, mask);

    __m128 tz = _mm_set1_ps(q.z_);
    mask = _mm_load_ps((f32*)QuaternionMulMask2_);
    tz = _mm_xor_ps(tz, mask);

    __m128 t0 = _mm_load_ss(&v.x_);
    t0 = _mm_shuffle_ps(t0, t0, _MM_SHUFFLE(0, 1, 0, 1));
    t0 = _mm_loadh_pi(t0, reinterpret_cast<const __m64*>(&v.y_));

    __m128 t1 = _mm_shuffle_ps(t0, t0, 0xB1);
    __m128 t2 = _mm_shuffle_ps(t0, t0, 0x4E);
    __m128 t3 = _mm_shuffle_ps(t0, t0, 0x1B);

    t0 = _mm_mul_ps(t0, tw);
    t0 = _mm_fmadd_ps(tx, t1, t0);
    t0 = _mm_fmadd_ps(ty, t2, t0);
    t0 = _mm_fmadd_ps(tz, t3, t0);
    // t0 = _mm_add_ps(t0, _mm_mul_ps(tx, t1));
    // t0 = _mm_add_ps(t0, _mm_mul_ps(ty, t2));
    // t0 = _mm_add_ps(t0, _mm_mul_ps(tz, t3));
    Quaternion r;
    _mm_storeu_ps(&r.w_, t0);
    return r;
}

Vector3 rotate(const Vector3& v, const Quaternion& rotation)
{
    // conjugate(Q) x V x Q
    Quaternion conj = conjugate(rotation);
    Quaternion rot = mul(conj, v);
    rot = mul(rot, rotation);

    return {rot.x_, rot.y_, rot.z_};
}

Vector3 rotate(const Quaternion& rotation, const Vector3& v)
{
    // conjugate(Q) x V x Q
    Quaternion conj = conjugate(rotation);
    Quaternion rot = mul(conj, v);
    rot = mul(rot, rotation);

    return {rot.x_, rot.y_, rot.z_};
}

Vector4 rotate(const Vector4& v, const Quaternion& rotation)
{
    Quaternion conj = conjugate(rotation);
    Quaternion rot = mul(conj, v);
    rot = mul(rot, rotation);
    return {rot.x_, rot.y_, rot.z_, v.w_};
}

Vector4 rotate(const Quaternion& rotation, const Vector4& v)
{
    Quaternion conj = conjugate(rotation);
    Quaternion rot = mul(conj, v);
    rot = mul(rot, rotation);
    return {rot.x_, rot.y_, rot.z_, v.w_};
}
} // namespace lray
