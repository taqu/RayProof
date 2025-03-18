#include "Camera.h"
#include <algorithm>
#include <cmath>
#include "Sampler.h"

namespace lray
{
Camera::Camera()
    : width_(0)
    , height_(0)
    , aspect_(0.0f)
    , dx_(0.0f)
    , dy_(0.0f)
    , origin_{}
    , forward_{}
    , right_{}
    , up_{}
    , lensRadius_(0.0f)
{
}

Camera::~Camera()
{
}

u32 Camera::width() const
{
    return width_;
}

u32 Camera::height() const
{
    return height_;
}

f32 Camera::aspect() const
{
    return aspect_;
}

f32 Camera::dx() const
{
    return dx_;
}

f32 Camera::dy() const
{
    return dy_;
}

const Vector3& Camera::origin() const
{
    return origin_;
}

const Vector3& Camera::forward() const
{
    return forward_;
}

const Vector3& Camera::right() const
{
    return right_;
}

const Vector3& Camera::up() const
{
    return up_;
}

f32 Camera::lensRadius() const
{
    return lensRadius_;
}

void Camera::perspectiveFov(u32 width, u32 height, f32 fovy)
{
    aspect_ = static_cast<f32>(width) / static_cast<f32>(height);
    fovy_ = std::tan(0.5f * fovy);
    f32 fovx = fovy_ * aspect_;
    width_ = width;
    height_ = height;
    dx_ = fovx;
    dy_ = fovy;
    origin_ = {};
    forward_ = {0.0f, 0.0f, -1.0f};
    right_ = {1.0f, 0.0f, 0.0f};
    up_ = {0.0f, 1.0f, 0.0f};
    lensRadius_ = 0.0f;
}

void Camera::perspectiveLens(u32 width, u32 height, f32 fovy, f32 aperture)
{
    aspect_ = static_cast<f32>(width) / static_cast<f32>(height);
    fovy_ = std::tan(0.5f * fovy);
    f32 fovx = fovy_ * aspect_;
    width_ = width;
    height_ = height;
    dx_ = fovx;
    dy_ = fovy;
    origin_ = {};
    forward_ = {0.0f, 0.0f, -1.0f};
    right_ = {1.0f, 0.0f, 0.0f};
    up_ = {0.0f, 1.0f, 0.0f};
    lensRadius_ = aperture * 0.5f;
}

void Camera::lookAt(const Vector3& eye, const Vector3& at, const Vector3& up)
{
    forward_ = safe_normalize3(at - eye);
    f32 cs = dot(forward_, up);
    if(0.999f < std::abs(cs)){
        up_ = {forward_.z_, forward_.x_, forward_.y_};
    }else{
        up_ = up;
    }
    right_ = safe_normalize3(cross(forward_, up_));
    up_ = safe_normalize3(cross(right_, forward_));
    origin_ = eye;
}

f32 Camera::screenToNDC(u32 x, u32 resolution, f32 jitter) const
{
return 2.0f*((static_cast<f32>(x)+0.5f+jitter)/static_cast<f32>(resolution))-1.0f;
}

Vector2 Camera::screenToNDC(const Vector2u& x, const Vector2u& resolution, const Vector2& jitter) const
{
f32 cx = screenToNDC(x.x_, resolution.x_, jitter.x_);
f32 cy = screenToNDC(x.y_, resolution.y_, jitter.y_);
return {cx, cy};
}

Ray Camera::generate(const Vector2u& x, const Vector2& screenSample, const Vector2& lensSample) const
{
    Vector2 sample = randomOnDisk(lensSample.x_, lensSample.y_);
    Vector3 originUp = sample.x_ * up_;
    Vector3 originRight = sample.y_ * right_;
    Vector3 origin = origin_ + originUp + originRight;
    f32 dx = dx_ * screenToNDC(x.x_, width_, screenSample.x_-0.49999f);
    f32 dy = dy_ * screenToNDC(x.y_, height_, screenSample.y_-0.49999f);
    Vector3 right = dx * right_;
    Vector3 up = dy * up_;
    Vector3 direction = safe_normalize3(right + up + forward_);
    return {origin, direction};
}
} // namespace lray

