#include "Material.h"
#include <cmath>
#include <algorithm>
#include "Coordinate.h"
#include "HitRecord.h"
#include "Ray.h"
#include "Texture.h"
#include "Context.h"

namespace lray
{
namespace
{
    f32 project2(f32 x, f32 y, const Vector3& v)
    {
        if(isEquals(x, y)) {
            return x * y;
        }
        f32 sn = 1.0f - v.z_ * v.z_;
        if(sn <= F32_EPSILON) {
            return x * y;
        }
        f32 invSn = 1.0f / sn;
        f32 cs2 = v.x_ * v.x_ * invSn;
        f32 sn2 = v.y_ * v.y_ * invSn;
        return cs2 * x * x + sn2 * y * y;
    }

    f32 ggx_NDF(const Vector3& m, f32 alpha2)
    {
        f32 d = m.z_;
        f32 denom = (d * d) * (alpha2 - 1.0f) + 1.0f;
        return alpha2 / (F32_PI * denom * denom);
    }

    f32 ggx_G1(const Vector3& v, f32 alpha2)
    {
        f32 dotNV = v.z_;
        f32 denom = std::sqrt(alpha2 + (1.0f - alpha2) * dotNV * dotNV) + dotNV;
        return 2.0f * dotNV / denom;
    }

    f32 ggx_G2(const Vector3& wi, const Vector3& wo, f32 alpha2)
    {
        f32 dotNI = wi.z_;
        f32 dotNO = wo.z_;
        f32 denomI = dotNO * std::sqrt(alpha2 + (1.0f - alpha2) * dotNI * dotNI);
        f32 denomO = dotNI * std::sqrt(alpha2 + (1.0f - alpha2) * dotNO * dotNO);
        return 2.0f * dotNI * dotNO / (denomI + denomO);
    }

    Vector3 ggx_VNDF(const Vector3& wo, f32 roughness, f32 eta0, f32 eta1)
    {
        // 1. Transform the view direction to the hemisphere configuration
        Vector3 v = normalize3({roughness * wo.x_, roughness * wo.y_, wo.z_});

        // 2. Construct orthonormal bais
        Vector3 t1;
        if(v.z_ < 0.999f) {
            t1 = normalize3(cross(v, {0, 0, 1}));
        } else {
            t1 = {1, 0, 0};
        }
        Vector3 t2 = cross(t1, v);

        // 3. Make parameterization of the projected area
        /*
        a := 1.0/(1.0+v.Y)
        r := math32.Sqrt(eta0)
        var phi float32
        var b float32
        if eta1<a {
            phi = eta1/a * math32.Pi
            b = 1.0
        }else{
            phi = math32.Pi + (eta1-a)/(1.0-a) * math32.Pi;
            b = v.Z
        }
        p1 := r * math32.Cos(phi)
        p2 := r * math32.Sin(phi) * b
        */
        f32 r = std::sqrt(eta0);
        f32 phi = (F32_PI * 2.0f) * eta1;
        f32 p1 = r * std::cos(phi);
        f32 p2 = r * std::sin(phi);
        f32 a = 0.5f * (1.0f + v.z_);
        p2 = (1.0f - a) * std::sqrt(1.0f - p1 * p1) + a * p2;

        // 4. Reproject onto hemisphere
        t1 = p1 * t1;
        t2 = p2 * t2;
        v = std::sqrt(std::max(0.0f, 1.0f - p1 * p1 - p2 * p2)) * v;
        Vector3 n = t1 + t2 + v;

        // 5. Transform the normal back to the elipsoid configuration
        return normalize3({roughness * n.x_, roughness * n.y_, std::max(0.0f, n.z_)});
    }

} // namespace

f32 Material::NsToRoughness(f32 Ns)
{
    Ns = std::clamp(Ns, 0.0f, 1.0f);
    return std::lerp(0.99f, 0.01f, Ns);
}

Fixed16 Material::NsToRoughness(Fixed16 Ns)
{
    Ns = clamp01(Ns);
    return lerp(Fixed16(0.99f), Fixed16(0.01f), Ns);
}

Material Material::createLambert(const Vector3& albedo)
{
    Material material;
    material.model_ = Model::Lambert;
    material.albedo_ = albedo;
    material.roughness_ = 1.0f;
    material.metallic_ = 0.0f;
    material.refIndex_ = 0.0f;
    return material;
}

Material Material::createMetal(const Vector3& albedo, f32 roughness, f32 metallic)
{
    Material material;
    material.model_ = Model::Metal;
    material.albedo_ = albedo;
    material.roughness_ = roughness;
    material.metallic_ = metallic;
    material.refIndex_ = 0.0f;
    return material;
}

Material Material::createDielectric(const Vector3& albedo, f32 refIndex)
{
    Material material;
    material.model_ = Model::Dielectric;
    material.albedo_ = albedo;
    material.roughness_ = 0.0f;
    material.metallic_ = 0.0f;
    material.refIndex_ = refIndex;
    return material;
}

MaterialSample Material::sample(const Vector3& wi, f32 eta0, f32 eta1) const
{
    switch(model_) {
    case Model::Lambert: {
        if(wi.z_ <= F32_EPSILON) {
            return {false, 0.0f, {0, 0, 0}, {0, 0, 0}};
        }
        Vector3 wm = randomOnCosineHemiSphere(eta0, eta1);
        f32 pdf = wi.z_ / F32_PI;
        return {true, pdf, albedo_, wm};
    } break;
    case Model::Metal: {
        Vector3 wm = ggx_VNDF(wi, roughness_, eta0, eta1);
        Vector3 wo = 2.0f * dot(wi, wm) * wm - wi;
        if(0.0f < wo.z_) {
            f32 f = schilick(dot(wo, wm), refIndex_);
            f32 g1 = ggx_G1(wi, roughness_);
            f32 g2 = ggx_G2(wo, wi, roughness_ * roughness_);
            f32 pdf = f * (g2 / g1);
            return {true, pdf, albedo_, wm};
        } else {
            return {false, 0.0f, albedo_, wm};
        }
        /*
        alpha2 := metal.Roughness * metal.Roughness
        theta := math32.Acos(math32.Sqrt((1.0-eta0)/((alpha2-1.0)*eta0 + 1.0)))
        phi := 2.0 * math32.Pi * eta1
        sinTheta := math32.Sin(theta)
        cosTheta := math32.Cos(theta)
        sinPhi := math32.Sin(phi)
        cosPhi := math32.Cos(phi)
        wm := Vector3{sinTheta*cosPhi, sinTheta*sinPhi, cosTheta}
        wo := SubVector3(MulVector3(2.0*DotVector3(wi, wm), wm), wi)

        if 0.0 < wo.Z  && 0.0<DotVector3(wo, wm) {
            f := Schlick(DotVector3(wo, wm), metal.RefIndex)
            g := ggx_G2(wo, wi, alpha2)
            pdf := f * g * math32.Abs(DotVector3(wi,wm))/(wi.Z * wm.Z)
            return MaterialSample{true, pdf, metal.Albedo, wm}
        }else{
            return MaterialSample{false, 0.0, metal.Albedo, wm}
        }
        */
    } break;
    case Model::Dielectric: {
        if(wi.z_ <= F32_EPSILON) {
            return {false, 0.0f, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
        }
        Vector3 normal = {0.0f, 0.0f, 1.0f};
        Vector3 direction = -wi;
        Vector3 reflected = normalize3(reflect(direction, normal));
        f32 niOverNt;
        f32 cosine;
        Vector3 n;
        if(0.0f < direction.z_) {
            n = {0.0f, 0.0f, -1.0f};
            niOverNt = refIndex_;
            cosine = refIndex_ * direction.z_;
        } else {
            n = {0.0f, 0.0f, 1.0f};
            niOverNt = 1.0f / refIndex_;
            cosine = -direction.z_;
        }
        Vector3 refracted;
        if(!refract(refracted, direction, n, niOverNt)) {
            return {true, 1.0f, {1.0f, 1.0f, 1.0f}, reflected};
        }
        f32 reflectProb = schilick(cosine, refIndex_);
        if(eta0 < reflectProb) {
            return {true, 1.0f, {1.0f, 1.0f, 1.0f}, reflected};
        } else {
            return {true, 1.0f, {1.0f, 1.0f, 1.0f}, refracted};
        }
    } break;
    default:
        return {};
    }
}

bool Material::scatter(const Ray& ray, const HitRecord& hitRecord, Vector3& attenuation, Ray& scattered) const
{
    switch(model_) {
    case Model::Lambert: {
        Coordinate coordinate = Coordinate::create(hitRecord.normal_);
        f32 eta[2];
        Context::get().getMaterialSampler().sampleN(2, eta);
        Vector3 n = randomOnHemiSphere(eta[0], eta[1]);
        scattered = {hitRecord.position_, coordinate.localToWorld(n)};
        attenuation = albedo_;
        return true;
    } break;
    case Model::Metal: {
        Vector3 reflected = normalize3(reflect(ray.direction_, hitRecord.normal_));
        scattered = {hitRecord.position_, reflected};
        attenuation = albedo_;
        return 0.0001f < dot(scattered.direction_, hitRecord.normal_);
    }
    case Model::Dielectric: {
        Vector3 reflected = normalize3(reflect(ray.direction_, hitRecord.normal_));
        attenuation = albedo_;
        f32 niOverNt;
        f32 cosine;
        Vector3 normal;
        f32 d = dot(ray.direction_, hitRecord.normal_);
        if(0.0f < d) {
            normal = -hitRecord.normal_;
            niOverNt = refIndex_;
            cosine = refIndex_ * d;
        } else {
            normal = hitRecord.normal_;
            niOverNt = 1.0f / refIndex_;
            cosine = -d;
        }
        Vector3 refracted;
        if(!refract(refracted, ray.direction_, normal, niOverNt)) {
            scattered = {hitRecord.position_, reflected};
            return true;
        }
        f32 reflectProb = schilick(cosine, refIndex_);
        f32 eta0;
        Context::get().getMaterialSampler().sampleN(1, &eta0);
        if(eta0 < reflectProb) {
            scattered = {hitRecord.position_, reflected};
        } else {
            scattered = {hitRecord.position_, refracted};
        }
        return true;
    } break;
        break;
    default:
        return false;
    }
}

f32 Material::getRoughness() const
{
    return roughness_;
}

f32 Material::getMetallic() const
{
    return metallic_;
}

const Vector3& Material::getAlbedo() const
{
    return albedo_;
}

} // namespace lray
