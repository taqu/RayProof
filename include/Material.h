#ifndef INC_LRAY_MATERIAL_H_
#define INC_LRAY_MATERIAL_H_
#include "Core.h"
#include "Vector.h"
#include "Texture.h"

namespace lray
{
	class Ray;
	class Texture;
	struct HitRecord;
	struct MaterialSample
	{
		bool continue_;
		f32 pdf_;
		Vector3 weight_;
		Vector3 scattered_;
	};

	class Material
	{
    public:
		static f32 NsToRoughness(f32 Ns);

        static Material createLambert(const Vector3& albedo);
		static Material createMetal(const Vector3& albedo, f32 roughness, f32 metallic);
		static Material createDielectric(const Vector3& albedo, f32 refIndex);

		enum class Model
		{
			Lambert,
			Metal,
			Dielectric,
		};
        MaterialSample sample(const Vector3& wi, f32 eta0, f32 eta1) const;
        bool scatter(const Ray& ray, const HitRecord& hitRecord, Vector3& attenuation, Ray& scattered) const;
        f32 getRoughness() const;
        f32 getMetallic() const;
        const Vector3& getAlbedo() const;

		Model model_;
		Vector3 ambient_;
		Vector3 albedo_;
		Vector3 specular_;
		Vector3 emissive_;
		f32 transparency_;
		f32 roughness_;
		f32 metallic_;
		f32 refIndex_;
		IntrusivePtr<Texture> texAmbient_;
		IntrusivePtr<Texture> texAlbedo_;
		IntrusivePtr<Texture> texSpecular_;
		IntrusivePtr<Texture> texEmissive_;
		IntrusivePtr<Texture> texRoughness_;
		IntrusivePtr<Texture> texTransparency_;
		IntrusivePtr<Texture> texNormal_;
	};
}
#endif //INC_LRAY_MATERIAL_H_

