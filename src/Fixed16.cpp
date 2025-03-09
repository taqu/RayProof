#include "Fixed16.h"
#include <cmath>

namespace lray
{
Fixed16::Fixed16()
{
}

Fixed16::Fixed16(uint16_t x)
    : x_(x)
{
}

Fixed16::Fixed16(float x)
{
    x_ = (uint16_t)round(x * (1 << Frac));
}

Fixed16& Fixed16::operator=(float x)
{
    x_ = (uint16_t)round(x * (1 << Frac));
    return *this;
}

Fixed16::operator float() const
{
    return (float)x_ / (float)(1 << Frac);
}

bool operator==(const Fixed16& x0, const Fixed16& x1)
{
    return x0.x_ == x1.x_;
}

bool operator!=(const Fixed16& x0, const Fixed16& x1)
{
    return x0.x_ != x1.x_;
}

bool operator<(const Fixed16& x0, const Fixed16& x1)
{
    return x0.x_ < x1.x_;
}

bool operator<=(const Fixed16& x0, const Fixed16& x1)
{
    return x0.x_ <= x1.x_;
}

bool operator>(const Fixed16& x0, const Fixed16& x1)
{
    return x0.x_ > x1.x_;
}

bool operator>=(const Fixed16& x0, const Fixed16& x1)
{
    return x0.x_ >= x1.x_;
}

Fixed16 operator+(const Fixed16& x0, const Fixed16& x1)
{
    return Fixed16((uint16_t)(x0.x_ + x1.x_));
}

Fixed16 operator-(const Fixed16& x0, const Fixed16& x1)
{
    return x0 < x1 ? Fixed16((uint16_t)0) : Fixed16((uint16_t)(x0.x_ - x1.x_));
}

Fixed16 operator*(const Fixed16& x0, const Fixed16& x1)
{
    uint32_t r = (uint32_t)x0.x_ * (uint32_t)x1.x_;
    r >>= Fixed16::Frac;
    return Fixed16((uint16_t)r);
}

Fixed16 operator/(const Fixed16& x0, const Fixed16& x1)
{
    uint32_t r = ((uint32_t)x0.x_ << Fixed16::Frac) / (uint32_t)x1.x_;
    return Fixed16((uint16_t)r);
}
} // namespace ray
