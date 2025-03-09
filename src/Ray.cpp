#include "Ray.h"

namespace lray
{
	Vector3 Ray::pointAt(f32 t) const
	{
		return origin_ + t*direction_;
	}
}