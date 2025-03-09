#ifndef INC_LRAY_VECTOR_H_
#define INC_LRAY_VECTOR_H_
#include "Core.h"

namespace lray
{
//--- Vector2
//---------------------------------------------
class Vector2
{
public:
    static Vector2 Zero;
    static Vector2 One;

    f32 lengthSqr() const;
    f32 length() const;
    void normalize();
    void safe_normalize(f32 epsilon = RAY_F32_EPSILON);

    Vector2& operator+=(const Vector2& x);
    Vector2& operator-=(const Vector2& x);
    Vector2& operator*=(const Vector2& x);
    Vector2& operator/=(const Vector2& x);
    Vector2& operator*=(f32 x);
    Vector2& operator/=(f32 x);

    f32 x_;
    f32 y_;
};

Vector2 operator+(const Vector2& x0, const Vector2& x1);
Vector2 operator-(const Vector2& x0, const Vector2& x1);
Vector2 operator*(const Vector2& x0, const Vector2& x1);
Vector2 operator*(f32 x0, const Vector2& x1);
Vector2 operator*(const Vector2& x0, f32 x1);
Vector2 operator/(const Vector2& x0, const Vector2& x1);
Vector2 operator/(const Vector2& x0, f32 x1);

f32 dot(const Vector2& x0, const Vector2& x1);
Vector2 normalize2(const Vector2& x);
Vector2 safe_normalize2(const Vector2& x, f32 epsilon = RAY_F32_EPSILON);
Vector2 randomOnDisk(f32 x0, f32 x1);

//--- Vector3
//---------------------------------------------
class Vector3
{
public:
    static const Vector3 Zero;
    static const Vector3 One;
    static const Vector3 Forward;
    static const Vector3 Right;
    static const Vector3 Up;

    f32 lengthSqr() const;
    f32 length() const;
    void normalize();
    void safe_normalize(f32 epsilon = RAY_F32_EPSILON);

    Vector3 operator-() const
    {
        return {-x_, -y_, -z_};
    }
    const f32& operator[](u32 index) const
    {
        return reinterpret_cast<const f32*>(this)[index];
    }
    f32& operator[](u32 index)
    {
        return reinterpret_cast<f32*>(this)[index];
    }

    Vector3& operator+=(const Vector3& x);
    Vector3& operator-=(const Vector3& x);
    Vector3& operator*=(const Vector3& x);
    Vector3& operator/=(const Vector3& x);
    Vector3& operator*=(f32 x);
    Vector3& operator/=(f32 x);

    f32 x_;
    f32 y_;
    f32 z_;
};

Vector3 operator+(const Vector3& x0, const Vector3& x1);
Vector3 operator-(const Vector3& x0, const Vector3& x1);
Vector3 operator*(const Vector3& x0, const Vector3& x1);
Vector3 operator*(f32 x0, const Vector3& x1);
Vector3 operator*(const Vector3& x0, f32 x1);
Vector3 operator/(const Vector3& x0, const Vector3& x1);
Vector3 operator/(const Vector3& x0, f32 x1);

Vector3 normalize3(const Vector3& x);
Vector3 safe_normalize3(const Vector3& x, f32 epsilon = RAY_F32_EPSILON);

Vector3 hadamardMul(const Vector3& x0, const Vector3& x1);
f32 dot(const Vector3& x0, const Vector3& x1);
Vector3 cross(const Vector3& x0, const Vector3& x1);
Vector3 minimum(const Vector3& x0, const Vector3& x1);
Vector3 maximum(const Vector3& x0, const Vector3& x1);

Vector3 saturate(const Vector3& x);
Vector3 lerp(const Vector3& x0, const Vector3& x1, f32 t);
void orthonormalBasis(Vector3& b0, Vector3& b1, const Vector3& n);
Vector3 randomInSphere(f32 x0, f32 x1, f32 x2);
Vector3 randomOnSphere(f32 x0, f32 x1);
Vector3 randomOnHemiSphere(f32 x0, f32 x1);
Vector3 randomOnHemiSphereAround(f32 x0, f32 x1, const Vector3& n);
Vector3 randomOnCosineHemiSphere(f32 x0, f32 x1);
Vector3 randomOnCosineHemiSphereAround(f32 x0, f32 x1, const Vector3& n);
Vector3 randomCone(f32 x0, f32 x1, f32 cosCutoff);
Vector3 reflect(const Vector3& x, const Vector3& n);
bool refract(Vector3& refracted, const Vector3& x, const Vector3& n, f32 niOverNt);


//--- Vector4
//---------------------------------------------
class Vector4
{
public:
    static Vector4 Zero;
    static Vector4 One;

    f32 lengthSqr() const;
    f32 length() const;
    void normalize();
    void safe_normalize(f32 epsilon);

    Vector4& operator+=(const Vector4& x);
    Vector4& operator-=(const Vector4& x);
    Vector4& operator*=(const Vector4& x);
    Vector4& operator/=(const Vector4& x);
    Vector4& operator*=(f32 x);
    Vector4& operator/=(f32 x);

    f32 x_;
    f32 y_;
    f32 z_;
    f32 w_;
};

Vector4 operator+(const Vector4& x0, const Vector4& x1);
Vector4 operator-(const Vector4& x0, const Vector4& x1);
Vector4 operator*(const Vector4& x0, const Vector4& x1);
Vector4 operator*(f32 x0, const Vector4& x1);
Vector4 operator*(const Vector4& x0, f32 x1);
Vector4 operator/(const Vector4& x0, const Vector4& x1);
Vector4 operator/(const Vector4& x0, f32 x1);

Vector4 hadamardMul(const Vector4& x0, const Vector4& x1);
f32 dot(const Vector4& x0, const Vector4& x1);
Vector4 normalize4(const Vector4& x);
Vector4 safe_normalize4(const Vector4& x, f32 epsilon = RAY_F32_EPSILON);
} // namespace ray
#endif // INC_LRAY_VECTOR_H_