#ifndef INC_LRAY_VERTEX_H_
#define INC_LRAY_VERTEX_H_
#include "Core.h"
#include "Vector.h"

namespace lray
{
struct Vertex
{
	Vector3 position_;
	Vector2 texcoord_;
	Vector3 normal_;
	Vector3 binormal0_;
};
}
#endif //INC_LRAY_VERTEX_H_
