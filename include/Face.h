#ifndef INC_LRAY_FACE_H_
#define INC_LRAY_FACE_H_
#include "Core.h"

namespace lray
{
struct Face
{
	u32 material_;
	u32 v0_;
	u32 v1_;
	u32 v2_;
};
}
#endif //INC_LRAY_FACE_H_
