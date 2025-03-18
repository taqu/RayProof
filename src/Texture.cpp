#include "Texture.h"
#include "Color.h"
#include "cppimg.h"
#include <algorithm>
#include <cstring>
#include <memory>

namespace lray
{
namespace
{
    bool endswith(const char* str, const char* ext)
    {
         str = strrchr(str, '.');
         if(nullptr == str){
             return false;
         }
         if('\0' == str[1]){
             return false;
         }
         ++str;
         while('\0' != str[0] && '\0' != ext[0]){
             if(str[0] != ext[0]){
                 return false;
             }
             ++str;
             ++ext;
         }
         return '\0' == str[0] && '\0' == ext[0];
    }
}

bool Texture::load(Texture& texture, const char* filepath, Address addressU, Address addressV)
{
    LRAY_ASSERT(nullptr != filepath);
    Image* image = LRAY_NEW Image;
    if(endswith(filepath, "png")) {
        if(!load_png(filepath, *image)){
            LRAY_DELETE(image);
        }
    } else if(endswith(filepath, "bmp")) {
        if(!load_bmp(filepath, *image)){
            LRAY_DELETE(image);
        }
    } else if(endswith(filepath, "tga")) {
        if(!load_tga(filepath, *image)){
            LRAY_DELETE(image);
        }
    } else if(endswith(filepath, "jpg") || endswith(filepath, "jpeg")) {
        if(!load_jpg(filepath, *image)){
            LRAY_DELETE(image);
        }
    }else{
        LRAY_DELETE(image);
    }
    if(nullptr != image){
        texture.addressU_ = addressU;
        texture.addressV_ = addressV;
        texture.image_ = image;
        return true;
    }else{
        return false;
    }
}

Texture::Texture()
    : addressU_(Address::Clamp)
    , addressV_(Address::Clamp)
{
}

Texture::Texture(const Texture& other)
    :addressU_(other.addressU_)
    ,addressV_(other.addressV_)
    ,image_(other.image_)
{
}

    Texture::Texture(Texture&& other)
    :addressU_(other.addressU_)
    ,addressV_(other.addressV_)
    ,image_(std::move(other.image_))
{
}

    Texture& Texture::operator=(const Texture& other)
{
        if(this != &other){
            addressU_ = other.addressU_;
            addressV_ = other.addressV_;
            image_ = other.image_;
        }
        return *this;
}

    Texture& Texture::operator=(Texture&& other)
    {
        if(this != &other){
            addressU_ = other.addressU_;
            addressV_ = other.addressV_;
            image_ = std::move(other.image_);
        }
        return *this;
    }

Texture::~Texture()
{
}

bool Texture::valid() const
{
    return (bool)image_;
}

u32 Texture::width() const
{
    LRAY_ASSERT(image_);
    return image_->width();
}

u32 Texture::height() const
{
    LRAY_ASSERT(image_);
    return image_->height();
}

void Texture::get9pixels(RGBA16 pixels[9], u32 x, u32 y) const
{
    s32 sx = static_cast<s32>(x);
    s32 sy = static_cast<s32>(y);
    pixels[0] = get(sx - 1, sy - 1);
    pixels[1] = get(sx, sy - 1);
    pixels[2] = get(sx + 1, sy - 1);

    pixels[3] = get(sx - 1, sy);
    pixels[4] = get(sx, sy);
    pixels[5] = get(sx + 1, sy);

    pixels[6] = get(sx - 1, sy + 1);
    pixels[7] = get(sx, sy + 1);
    pixels[8] = get(sx + 1, sy + 1);
}

RGBA16 Texture::get(u32 x, u32 y) const
{
    assert(0 < image_->width());
    assert(0 < image_->height());
    u32 ux = std::clamp(x, 0U, static_cast<u32>(image_->width() - 1));
    u32 uy = std::clamp(y, 0U, static_cast<u32>(image_->height() - 1));
    return (*image_)(ux,uy);
}

RGBA16 Texture::get(s32 x, s32 y) const
{
    assert(0 < image_->width());
    assert(0 < image_->height());
    s32 sx = std::clamp(x, 0, static_cast<s32>(image_->width() - 1));
    s32 sy = std::clamp(y, 0, static_cast<s32>(image_->height() - 1));
    return (*image_)(sx,sy);
}

void Texture::set(u32 x, u32 y, const RGBA16& c)
{
    assert(image_);
    assert(0 < image_->width());
    assert(0 < image_->height());
    (*image_)(x,y) = c;
}

RGBA16 Texture::sample(f32 u, f32 v) const
{
    assert(0 < image_->width());
    assert(0 < image_->height());
    u = address(u, addressU_);
    v = address(v, addressV_);

    u *= (image_->width() - 1);
    v *= (image_->height() - 1);
    u32 px0 = static_cast<u32>(u);
    u32 py0 = static_cast<u32>(v);
    assert(px0 < image_->width());
    assert(py0 < image_->height());
    u32 px1 = addressU(px0 + 1);
    u32 py1 = addressV(py0 + 1);
    __m128 p0 = load(px0, py0);
    __m128 p1 = load(px1, py0);
    __m128 p2 = load(px0, py1);
    __m128 p3 = load(px1, py1);

    __m128 dx0 = _mm_set1_ps(u - px0);
    __m128 dx1 = _mm_sub_ps(_mm_set1_ps(1.0f), dx0);
    __m128 r0 = _mm_add_ps(_mm_mul_ps(p0, dx0), _mm_mul_ps(p1, dx1));
    __m128 r1 = _mm_add_ps(_mm_mul_ps(p2, dx0), _mm_mul_ps(p3, dx1));

    __m128 dy0 = _mm_set1_ps(v - py0);
    __m128 dy1 = _mm_sub_ps(_mm_set1_ps(1.0f), dy0);

    __m128 c = _mm_add_ps(_mm_mul_ps(r0, dy0), _mm_mul_ps(r1, dy1));
    c = _mm_mul_ps(c, _mm_set1_ps((float)(1 << Fixed16::Frac)));
    c = _mm_round_ps(c, 0);
    __m128i mi = _mm_cvttps_epi32(c);
    mi = _mm_packus_epi32(mi, mi);
    RGBA16 rgba;
    _mm_storel_epi64((__m128i*)&rgba, mi);
    return rgba;
}

void Texture::convert(std::function<RGBA16(u32,u32,const Texture&)> func)
{
    for(u32 i = 0; i < image_->height(); ++i) {
        for(u32 j = 0; j < image_->width(); ++j) {
            set(j, i, func(j, i, *this));
        }
    }
}

f32 Texture::address(f32 x, Address addressType) const
{
    switch(addressType) {
    case Address::Clamp:
        x = lray::clamp01(x);
        break;
    case Address::Repeate:
        while(x < 0.0f) {
            x += 1.0f;
        }
        while(1.0f < x) {
            x -= 1.0f;
        }
        break;
    }
    return x;
}

u32 Texture::addressU(u32 x) const
{
    switch(addressU_) {
    case Address::Clamp:
        x = minimum(x, (u32)(image_->width() - 1));
        break;
    case Address::Repeate:
        if(image_->width() <= x) {
            x = 0;
        }
        break;
    }
    return x;
}

u32 Texture::addressV(u32 x) const
{
    switch(addressV_) {
    case Address::Clamp:
        x = minimum(x, (u32)(image_->height() - 1));
        break;
    case Address::Repeate:
        if(image_->height() <= x) {
            x = 0;
        }
        break;
    }
    return x;
}

__m128 Texture::load(u32 x, u32 y) const
{
    const RGBA16& rgba = (*image_)(x, y);
    __m128i mi = _mm_loadl_epi64((const __m128i*)&rgba);
    mi = _mm_cvtepu16_epi32(mi);
    __m128 mf = _mm_cvtepi32_ps(mi);
    mf = _mm_div_ps(mf, _mm_set1_ps((float)(1 << Fixed16::Frac)));
    return mf;
}

} // namespace lray
