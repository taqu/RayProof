#ifndef INC_LRAY_CAMERA_H_
#define INC_LRAY_CAMERA_H_
#include "Core.h"
#include "Vector.h"
#include "Ray.h"

namespace lray
{
class Camera
{
public:
    Camera();
    ~Camera();

    u32 width() const;
    u32 height() const;
    f32 aspect() const;
    f32 dx() const;
    f32 dy() const;
    const Vector3& origin() const;
    const Vector3& forward() const;
    const Vector3& right() const;
    const Vector3& up() const;
    f32 lensRadius() const;

    void perspectiveFov(u32 width, u32 height, f32 fovy);
    void perspectiveLens(u32 width, u32 height, f32 fovy, f32 aperture);
    void lookAt(const Vector3& eye, const Vector3& at, const Vector3& up);
    f32 screenToNDC(u32 x, u32 resolution, f32 jitter) const;
    Vector2 screenToNDC(const Vector2u& x, const Vector2u& resolution, const Vector2& jitter) const;
    Ray generate(const Vector2u& x, const Vector2& screenSample, const Vector2& lensSample) const;
private:
    Camera(const Camera&) = delete;
    Camera& operator=(const Camera&) = delete;
    u32 width_;
    u32 height_;
    f32 fovy_;
    f32 aspect_;
    f32 dx_;
    f32 dy_;
    Vector3 origin_;
    Vector3 forward_;
    Vector3 right_;
    Vector3 up_;
    f32 lensRadius_;
};
} // namespace lray
#endif // INC_LRAY_CAMERA_H_

