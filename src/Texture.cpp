#include "Texture.h"
#include <cstring>
#include <memory>
#include <algorithm>
#include "cppimg.h"
#include "Color.h"

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

IntrusivePtr<Texture> Texture::load(const char* filename)
{
    assert(nullptr != filename);
    cppimg::IFStream file;
    if(!file.open(filename)){
        return nullptr;
    }
    cppimg::s32 width;
    cppimg::s32 height;
    cppimg::ColorType colorType;
    std::unique_ptr<u8[]> image;
    if(endswith(filename, "png")){
        if(!cppimg::PNG::read(width, height, colorType, nullptr, file)){
            return nullptr;
        }
        image.reset(new u8[cppimg::getBytesPerPixel(colorType)*width*height]);
        if(!cppimg::PNG::read(width, height, colorType, image.get(), file)){
            return nullptr;
        }
    }else if(endswith(filename, "bmp")){
        if(!cppimg::BMP::read(width, height, colorType, nullptr, file)){
            return nullptr;
        }
        image.reset(new u8[cppimg::getBytesPerPixel(colorType)*width*height]);
        if(!cppimg::BMP::read(width, height, colorType, image.get(), file)){
            return nullptr;
        }
    }else if(endswith(filename, "tga")){
        if(!cppimg::TGA::read(width, height, colorType, nullptr, file)){
            return nullptr;
        }
        image.reset(new u8[cppimg::getBytesPerPixel(colorType)*width*height]);
        if(!cppimg::TGA::read(width, height, colorType, image.get(), file)){
            return nullptr;
        }
    }else if(endswith(filename, "jpg") || endswith(filename, "jpeg")){
        if(!cppimg::JPEG::read(width, height, colorType, nullptr, file)){
            return nullptr;
        }
        image.reset(new u8[cppimg::getBytesPerPixel(colorType)*width*height]);
        if(!cppimg::JPEG::read(width, height, colorType, image.get(), file)){
            return nullptr;
        }
    }
    file.close();

    IntrusivePtr<Texture> texture = new Texture(static_cast<u32>(width), static_cast<u32>(height));
    u32 size = static_cast<u32>(width*height);
    switch(colorType) {
    case cppimg::ColorType::GRAY: {
        for(u32 i=0; i<size; ++i){
            u8 r = image[i];
            texture->pixels_[i] = toColor16(r);
        }
    } break;
    case cppimg::ColorType::RGB: {
        for(u32 i = 0; i < size; ++i) {
            u8 r = image[3 * i + 0];
            u8 g = image[3 * i + 1];
            u8 b = image[3 * i + 2];
            texture->pixels_[i] = toColor16(r, g, b);
        }
    } break;
    case cppimg::ColorType::RGBA: {
        for(u32 i = 0; i < size; ++i) {
            u8 r = image[4 * i + 0];
            u8 g = image[4 * i + 1];
            u8 b = image[4 * i + 2];
            u8 a = image[4 * i + 3];
            texture->pixels_[i] = toColor16(r, g, b, a);
        }
    } break;
    }
    return texture;
}

Texture::Texture()
    : refCount_(0)
    ,width_(0)
    ,height_(0)
    ,addressU_(Address::Clamp)
    ,addressV_(Address::Clamp)
    ,pixels_(nullptr)
{
}

Texture::Texture(u32 width, u32 height, Address addressU, Address addressV)
    :refCount_(0)
    ,width_(width)
    ,height_(height)
    ,addressU_(addressU)
    ,addressV_(addressV)
    ,pixels_(nullptr)
{
    pixels_ = new Color16[width_*height_];
    ::memset(pixels_, 0, sizeof(Color16)*width_*height_);
}

Texture::~Texture()
{
    refCount_ = 0;
    delete[] pixels_;
    pixels_ = nullptr;
}

u32 Texture::width() const
{
    return width_;
}

u32 Texture::height() const
{
    return height_;
}

void Texture::convert(std::function<Color16(u32,u32,const Texture&)> func)
{
    for(u32 i=0; i<height_; ++i){
        for(u32 j=0; j<width_; ++j){
            pixels_[i*width_+j] = func(j,i,*this);
        }
    }
}

Color32 Texture::get(u32 x, u32 y) const
{
    return toColor32(pixels_[y*width_+x]);
}

void Texture::get9pixels(Color32 pixels[9], u32 x, u32 y) const
{
    s32 sx = static_cast<s32>(x);
    s32 sy = static_cast<s32>(y);
    pixels[0] = toColor32(get(sx-1, sy-1));
    pixels[1] = toColor32(get(sx  , sy-1));
    pixels[2] = toColor32(get(sx+1, sy-1));

    pixels[3] = toColor32(get(sx-1, sy));
    pixels[4] = toColor32(get(sx  , sy));
    pixels[5] = toColor32(get(sx+1, sy));

    pixels[6] = toColor32(get(sx-1, sy+1));
    pixels[7] = toColor32(get(sx  , sy+1));
    pixels[8] = toColor32(get(sx+1, sy+1));
}

Color16 Texture::get(s32 x, s32 y) const
{
    assert(0<width_);
    assert(0<height_);
    x = std::clamp(x, 0, static_cast<s32>(width_)-1);
    y = std::clamp(y, 0, static_cast<s32>(height_)-1);
    return pixels_[y*width_+x];
}

Color32 Texture::sample(f32 u, f32 v) const
{
    assert(0<width_);
    assert(0<height_);
    u = address(u, addressU_);
    v = address(v, addressV_);

    u *= (width_-1);
    v *= (height_-1);
    u32 px0 = static_cast<u32>(u);
    u32 py0 = static_cast<u32>(v);
    assert(px0<width_);
    assert(py0<height_);
    u32 px1 = addressU(px0+1);
    u32 py1 = addressV(py0+1);
    __m128 p0 = load(px0, py0);
    __m128 p1 = load(px1, py0);
    __m128 p2 = load(px0, py1);
    __m128 p3 = load(px1, py1);

    __m128 dx0 = _mm_set1_ps(u-px0);
    __m128 dx1 = _mm_sub_ps(_mm_set1_ps(1.0f), dx0);
    __m128 r0 = _mm_add_ps(_mm_mul_ps(p0, dx0), _mm_mul_ps(p1, dx1));
    __m128 r1 = _mm_add_ps(_mm_mul_ps(p2, dx0), _mm_mul_ps(p3, dx1));

    __m128 dy0 = _mm_set1_ps(v-py0);
    __m128 dy1 = _mm_sub_ps(_mm_set1_ps(1.0f), dy0);

    __m128 c = _mm_add_ps(_mm_mul_ps(r0, dy0), _mm_mul_ps(r1, dy1));
    Color32 color;
    _mm_storeu_ps(reinterpret_cast<f32*>(&color), c);
    return color;
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
        x = minimum(x, width_-1);
        break;
    case Address::Repeate:
        if(width_<=x){
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
        x = minimum(x, height_-1);
        break;
    case Address::Repeate:
        if(height_<=x){
            x = 0;
        }
        break;
    }
    return x;
}

__m128 Texture::load(u32 x, u32 y) const
{
    u32 p = y*width_ + x;
    __m128i f16c = _mm_loadl_epi64((__m128i*)&pixels_[p]);
    return _mm_cvtph_ps(f16c);
}

} // namespace lray
