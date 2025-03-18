#ifndef INC_LRAY_COLOR_H_
#define INC_LRAY_COLOR_H_
#include "Fixed16.h"

namespace lray
{
//--- RGBA
//---------------------------------------------
struct RGBA
{
    uint8_t r_;
    uint8_t g_;
    uint8_t b_;
    uint8_t a_;
};

//--- RGBA16
//---------------------------------------------
struct RGBA16
{
    Fixed16 r_;
    Fixed16 g_;
    Fixed16 b_;
    Fixed16 a_;
};

//--- RGBAF32
//---------------------------------------------
struct RGBAF32
{
    float r_;
    float g_;
    float b_;
    float a_;
};

RGBA16 ToRGBA16(const RGBA& x);
RGBA ToRGBA(const RGBA16& x);
RGBAF32 ToRGBAF32(const RGBA16& x);
float LinearToSRGB(float x);
RGBA16 LinearToSRGB(const RGBA16& x);
float SRGBToLinear(float x);
RGBA16 SRGBToLinear(const RGBA16& x);

f32 toGray(const RGBA16& x);
} // namespace lray
#endif // INC_LRAY_COLOR_H_
