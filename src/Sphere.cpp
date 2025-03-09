#include "Sphere.h"
#include <cmath>
#include "Ray.h"
#include "HitRecord.h"

namespace lray
{
Sphere::Sphere()
{
}

Sphere::Sphere(const Vector3& center, f32 radius)
    :center_(center)
    ,radius_(radius)
{
}

Sphere::~Sphere()
{
}

bool Sphere::hit(const Ray& ray, f32 tmin, f32 tmax, HitRecord& record) const
{
	Vector3 oc = ray.origin_ - center_;
	f32 a = dot(ray.direction_, ray.direction_);
	f32 b = dot(oc, ray.direction_);
	f32 c = dot(oc, oc) - radius_*radius_;

	f32 discriminant = b*b - a*c;
	if(discriminant<=0.0f){
		return false;
	}
	f32 inva = 1.0f/a;
	discriminant = std::sqrt(discriminant);
	f32 t;
	t = (-b-discriminant)*inva;
	if(tmin<t && t<tmax){
		record.t_ = t;
		record.position_ = ray.pointAt(t);
		record.normal_ = (record.position_-center_)/radius_;
		return true;
	}
	t = (-b + discriminant)*inva;
	if(tmin<t && t<tmax){
		record.t_ = t;
		record.position_ = ray.pointAt(t);
		record.normal_ = (record.position_-center_)/radius_;
		return true;
	}
	return false;
}
} // namespace lray
