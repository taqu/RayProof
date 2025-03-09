#ifndef INC_LRAY_COORDINATE_H_
#define INC_LRAY_COORDINATE_H_
#include "Vector.h"

namespace lray
{
	class Coordinate
	{
    public:
        static Coordinate create(const Vector3& normal);

		Vector3 worldToLocal(const Vector3& x);
        Vector3 localToWorld(const Vector3& x);

		Vector3 normal_;
		Vector3 binormal0_;
		Vector3 binormal1_;
	};
} // namespace lray
#endif // INC_LRAY_COORDINATE_H_