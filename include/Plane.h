#ifndef INC_LRAY_PLANE_H_
#define INC_LRAY_PLANE_H_
#include "Core.h"
#include "Hittable.h"
#include "Material.h"

namespace lray
{
	class Plane : public Hittable
{
    public:
        Plane();
        Plane(f32 nx, f32 ny, f32 nz, f32 d, const Material& material);
        virtual ~Plane();
        virtual bool hit(const Ray& ray, f32 tmin, f32 tmax, HitRecord& record) const override;

        f32 nx_;
        f32 ny_;
        f32 nz_;
        f32 d_;
        Material material_;
    };
}
#endif //INC_LRAY_PLANE_H_
