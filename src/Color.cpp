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

RGBAF32 ToRGBAF32(const RGBA16& x)
{
    __m128i mi = _mm_loadl_epi64((const __m128i*)&x);
    mi = _mm_cvtepu16_epi32(mi);
    __m128 mf = _mm_cvtepi32_ps(mi);
    mf = _mm_div_ps(mf, _mm_set1_ps((float)(1 << Fixed16::Frac)));
    alignas(16) RGBAF32 r;
    _mm_store_ps((float*)&r, mf);
    return r;
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

f32 toGray(const RGBA16& x)
{
    f32 r = x.r_;
    f32 g = x.g_;
    f32 b = x.b_;
    return 0.299f*r + 0.587f*g + 0.114f*b;
}
} // namespace lray
