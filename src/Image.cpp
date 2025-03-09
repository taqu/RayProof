#include "Image.h"
#include "cppimg.h"
#include "Color.h"
#include "Core.h"

namespace lray
{
//--- Image
//---------------------------------------------
Image::Image()
    : refCount_(0)
    , width_(0)
    , height_(0)
    , pixels_(nullptr)
{
}

Image::Image(uint32_t width, uint32_t height)
    : refCount_(0)
    , width_(static_cast<uint16_t>(width))
    , height_(static_cast<uint16_t>(height))
    , pixels_(nullptr)
{
    pixels_ = static_cast<RGBA16*>(LRAY_MALLOC(width_ * height_ * sizeof(RGBA16)));
}

Image::Image(Image&& other)
    : refCount_(other.refCount_)
    , width_(other.width_)
    , height_(other.height_)
    , pixels_(other.pixels_)
{
    other.refCount_ = 0;
    other.width_ = 0;
    other.height_ = 0;
    other.pixels_ = nullptr;
}

Image& Image::operator=(Image&& other)
{
    if(this != &other) {
        LRAY_FREE(pixels_);
        refCount_ = other.refCount_;
        width_ = other.width_;
        height_ = other.height_;
        pixels_ = other.pixels_;
        other.refCount_= 0;
        other.width_ = 0;
        other.height_ = 0;
        other.pixels_ = nullptr;
    }
    return *this;
}

Image::~Image()
{
    refCount_ = 0;
    width_ = 0;
    height_ = 0;
    LRAY_FREE(pixels_);
    pixels_ = nullptr;
}

uint16_t Image::width() const
{
    return width_;
}

uint16_t Image::height() const
{
    return height_;
}

const RGBA16& Image::operator()(uint32_t x, uint32_t y) const
{
    LRAY_ASSERT(x<width_ && y<height_);
    LRAY_ASSERT(nullptr != pixels_);
    return pixels_[y * width_ + x];
}

RGBA16& Image::operator()(uint32_t x, uint32_t y)
{
    LRAY_ASSERT(x<width_ && y<height_);
    LRAY_ASSERT(nullptr != pixels_);
    return pixels_[y * width_ + x];
}

void Image::reset(uint32_t width, uint32_t height)
{
    LRAY_FREE(pixels_);
    width_ = static_cast<uint16_t>(width);
    height_ = static_cast<uint16_t>(height);
    pixels_ = static_cast<RGBA16*>(LRAY_MALLOC(width_ * height_ * sizeof(RGBA16)));
}


namespace
{
    void convertFromGray(uint32_t width, uint32_t height, RGBA16* dst, const uint8_t* src)
    {
        for(uint32_t i=0; i<height; ++i){
            for(uint32_t j=0; j<width; ++j){
                uint32_t index = i*width + j;
                dst[index].r_ = (uint16_t)((uint16_t)src[index] << Fixed16::Frac);
                dst[index].g_ = (uint16_t)((uint16_t)src[index] << Fixed16::Frac);
                dst[index].b_ = (uint16_t)((uint16_t)src[index] << Fixed16::Frac);
                dst[index].a_ = 255UL << Fixed16::Frac;
            }
        }
    }

    void convertFromRGB(uint32_t width, uint32_t height, RGBA16* dst, const uint8_t* src)
    {
        for(uint32_t i=0; i<height; ++i){
            for(uint32_t j=0; j<width; ++j){
                uint32_t index = i*width + j;
                uint32_t src_index = index*3;
                dst[index].r_ = (uint16_t)((uint16_t)src[src_index+0] << Fixed16::Frac);
                dst[index].g_ = (uint16_t)((uint16_t)src[src_index+1] << Fixed16::Frac);
                dst[index].b_ = (uint16_t)((uint16_t)src[src_index+2] << Fixed16::Frac);
                dst[index].a_ = 255UL << Fixed16::Frac;
            }
        }
    }

    void convertFromRGBA(uint32_t width, uint32_t height, RGBA16* dst, const uint8_t* src)
    {
        for(uint32_t i=0; i<height; ++i){
            for(uint32_t j=0; j<width; ++j){
                uint32_t index = i*width + j;
                uint32_t src_index = index*4;
                dst[index].r_ = (uint16_t)((uint16_t)src[src_index+0] << Fixed16::Frac);
                dst[index].g_ = (uint16_t)((uint16_t)src[src_index+1] << Fixed16::Frac);
                dst[index].b_ = (uint16_t)((uint16_t)src[src_index+2] << Fixed16::Frac);
                dst[index].a_ = (uint16_t)((uint16_t)src[src_index+3] << Fixed16::Frac);
            }
        }
    }

    void convert(uint32_t width, uint32_t height, RGBA16* dst, cppimg::ColorType type, const uint8_t* src)
    {
        switch(type){
        case cppimg::ColorType::GRAY:
            convertFromGray(width, height, dst, src);
            break;
        case cppimg::ColorType::RGB:
            convertFromRGB(width, height, dst, src);
            break;
        case cppimg::ColorType::RGBA:
            convertFromRGBA(width, height, dst, src);
            break;
        default:
            LRAY_ASSERT(false);
            break;
        }
    }
}

bool load_bmp(const char* filepath, Image& img)
{
    LRAY_ASSERT(nullptr != filepath);

    cppimg::IFStream file;
    if(!file.open(filepath)) {
        return false;
    }
    cppimg::s32 width = 0, height = 0;
    cppimg::ColorType type;
    if(!cppimg::BMP::read(width, height, type, nullptr, file)){
        return false;
    }
    uint32_t size = cppimg::getBytesPerPixel(type) * width * height;
    uint8_t* data = (uint8_t*)LRAY_MALLOC(size);
    if(!cppimg::BMP::read(width, height, type, data, file)){
        LRAY_FREE(data);
        return false;
    }
    img.reset(width, height);
    convert(width, height, img.pixels_, type, data);
    LRAY_FREE(data);
    return true;
}

bool save_bmp(const char* filepath, const Image& img)
{
    LRAY_ASSERT(nullptr != filepath);
    cppimg::OFStream file;
    if(!file.open(filepath)) {
        return false;
    }
    RGBA* rgba = static_cast<RGBA*>(LRAY_MALLOC(sizeof(RGBA)*img.width()*img.height()));
    if(nullptr == rgba){
        return false;
    }
    for(uint32_t i=0; i<img.height(); ++i){
        for(uint32_t j=0; j<img.width(); ++j){
            RGBA& pixel = rgba[i*img.width()+j];
            pixel = ToRGBA(LinearToSRGB(img(j,i)));
        }
    }
    bool r = cppimg::BMP::write(file, static_cast<cppimg::s32>(img.width()), static_cast<cppimg::s32>(img.height()), cppimg::ColorType::RGBA, rgba);
    LRAY_FREE(rgba);
    return r;
}

bool load_png(const char* filepath, Image& img)
{
    LRAY_ASSERT(nullptr != filepath);

    cppimg::IFStream file;
    if(!file.open(filepath)) {
        return false;
    }
    cppimg::s32 width = 0, height = 0;
    cppimg::ColorType type;
    if(!cppimg::PNG::read(width, height, type, nullptr, file)){
        return false;
    }
    uint32_t size = cppimg::getBytesPerPixel(type) * width * height;
    uint8_t* data = (uint8_t*)LRAY_MALLOC(size);
    if(!cppimg::PNG::read(width, height, type, data, file)){
        LRAY_FREE(data);
        return false;
    }
    img.reset(width, height);
    convert(width, height, img.pixels_, type, data);
    LRAY_FREE(data);
    return true;
}

void intrusive_ptr_addref(Image* pointer)
{
    pointer->refCount_ += 1;
}

void intrusive_ptr_release(Image* pointer)
{
    pointer->refCount_ -= 1;
    if(pointer->refCount_ <= 0) {
        delete pointer;
    }
}
} // namespace ray
