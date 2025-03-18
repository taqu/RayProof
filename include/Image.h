#ifndef INC_LRAY_IMAGE_H_
#define INC_LRAY_IMAGE_H_
#include "Core.h"

namespace lray
{
struct RGBA16;

//--- Image
//---------------------------------------------
class Image
{
public:
    Image();
    Image(uint32_t width, uint32_t height);
    Image(Image&& other);
    Image& operator=(Image&& other);
    ~Image();
    uint16_t width() const;
    uint16_t height() const;

    const RGBA16& operator()(uint32_t x, uint32_t y) const;
    RGBA16& operator()(uint32_t x, uint32_t y);

private:
    Image(const Image&) = delete;
    Image& operator=(const Image&) = delete;
    friend void intrusive_ptr_addref(Image*);
    friend void intrusive_ptr_release(Image*);

    friend bool load_bmp(const char* filepath, Image& img);
    friend bool save_bmp(const char* filepath, const Image& img);
    friend bool load_tga(const char* filepath, Image& img);
    friend bool load_jpg(const char* filepath, Image& img);
    friend bool load_png(const char* filepath, Image& img);

    void reset(uint32_t width, uint32_t height);

    s32 refCount_;
    uint16_t width_;
    uint16_t height_;
    RGBA16* pixels_;
};

void intrusive_ptr_addref(Image* pointer);
void intrusive_ptr_release(Image* pointer);

bool load_bmp(const char* filepath, Image& img);
bool save_bmp(const char* filepath, const Image& img);
bool load_tga(const char* filepath, Image& img);
bool load_jpg(const char* filepath, Image& img);
bool load_png(const char* filepath, Image& img);

} // namespace ray
#endif // INC_LRAY_IMAGE_H_