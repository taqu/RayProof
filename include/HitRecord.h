#ifndef INC_LRAY_HITRECORD_H_
#define INC_LRAY_HITRECORD_H_
#include "Vector.h"

namespace lray
{
class Object;
class Material;

struct HitRecord
{
    inline static constexpr u32 Invalid = 0xFFFFFFFFUL;

    f32 t_;
    Vector3 position_;
    Vector3 normal_;
    const Object* object_;
    u32 face_;
};
} // namespace lray
#endif // INC_LRAY_HITRECORD_H_
