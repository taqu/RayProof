#ifndef INC_LRAY_RAY_H_
#define INC_LRAY_RAY_H_
#include "Vector.h"

namespace lray
{
	class Ray
	{
    public:
        Vector3 pointAt(f32 t) const;
		Vector3 origin_;
		Vector3 direction_;
    };
}
#endif //INC_LRAY_RAY_H_

