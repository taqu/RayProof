#ifndef INC_LRAY_PRIMITIVE_H_
#define INC_LRAY_PRIMITIVE_H_
#include "Common.h"

namespace lray
{
struct Primitive
{
	u32 indices_[3];
	u16 object_;
	u16 material_;
};
}
#endif //INC_LRAY_PRIMITIVE_H_
