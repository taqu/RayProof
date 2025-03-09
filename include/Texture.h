#ifndef INC_LRAY_TEXTURE_H_
#define INC_LRAY_TEXTURE_H_
#include <functional>
#include "Image.h"
#include "Color.h"

namespace lray
{
class Texture
{
public:
    static IntrusivePtr<Texture> load(const char* filename);
    enum class Address
    {
        Clamp,
        Repeate,
    };
    Texture();
    Texture(const Texture&);
    Texture(Texture&&);
    Texture& operator=(const Texture&);
    Texture& operator=(Texture&&);
    ~Texture();
    u32 width() const;
    u32 height() const;
    RGBA16 sample(f32 u, f32 v) const;

    void convert(std::function<RGBA16(u32,u32,const Texture&)> func);
    RGBA16 get(u32 x, u32 y) const;
    void get9pixels(RGBA16 pixels[9], u32 x, u32 y) const;
private:
    Texture(u32 width, u32 height, Address addressU=Address::Clamp, Address addressV=Address::Clamp);

    RGBA16 get(s32 x, s32 y) const;

    f32 address(f32 x, Address addressType) const;
    u32 addressU(u32 x) const;
    u32 addressV(u32 x) const;
    __m128 load(u32 x, u32 y) const;
    s32 refCount_;
    Address addressU_;
    Address addressV_;
    IntrusivePtr<Image> image_;
};
} // namespace lray
#endif //INC_LRAY_TEXTURE_H_