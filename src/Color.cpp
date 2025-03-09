#include "Color.h"
#include <cmath>

namespace lray
{
RGBA16 ToRGBA16(const RGBA& x)
{
    Fixed16 r((uint16_t)((uint16_t)x.r_ << Fixed16::Frac));
    Fixed16 g((uint16_t)((uint16_t)x.g_ << Fixed16::Frac));
    Fixed16 b((uint16_t)((uint16_t)x.b_ << Fixed16::Frac));
    Fixed16 a((uint16_t)((uint16_t)x.a_ << Fixed16::Frac));
    return {r, g, b, a};
}

RGBA ToRGBA(const RGBA16& x)
{
    uint8_t r = (uint8_t)(x.r_.x_ >> Fixed16::Frac);
    uint8_t g = (uint8_t)(x.g_.x_ >> Fixed16::Frac);
    uint8_t b = (uint8_t)(x.b_.x_ >> Fixed16::Frac);
    uint8_t a = (uint8_t)(x.a_.x_ >> Fixed16::Frac);
    return {r, g, b, a};
}

float LinearToSRGB(float x)
{
    return x <= 0.0031308f ? 12.92f * x : 1.055f * ::powf(x, 1.0f / 2.4f) - 0.055f;
}

RGBA16 LinearToSRGB(const RGBA16& x)
{
    Fixed16 r(LinearToSRGB((float)x.r_));
    Fixed16 g(LinearToSRGB((float)x.g_));
    Fixed16 b(LinearToSRGB((float)x.b_));
    Fixed16 a(LinearToSRGB((float)x.a_));
    return {r, g, b, a};
}

float SRGBToLinear(float x)
{
    return x <= 0.040450f ? x / 12.92f : ::powf((x + 0.055f) / 1.055f, 2.4f);
}

RGBA16 SRGBToLinear(const RGBA16& x)
{
    Fixed16 r(SRGBToLinear((float)x.r_));
    Fixed16 g(SRGBToLinear((float)x.g_));
    Fixed16 b(SRGBToLinear((float)x.b_));
    Fixed16 a(SRGBToLinear((float)x.a_));
    return {r, g, b, a};
}
} // namespace lray
