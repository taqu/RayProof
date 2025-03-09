#include "Object.h"
#include <filesystem>
#include "Ray.h"
#include "cppobj.h"
#include "cppimg.h"
#include "Texture.h"

namespace lray
{
namespace
{
    f32 correctTexcoord(f32 x)
    {
        while(1.0f<x){
            x -= 1.0f;
        }
        while(x<0.0f){
            x += 1.0f;
        }
        return x;
    }

    Vector3 calcNormal(const Vector3& p0, const Vector3& p1, const Vector3& p2)
    {
        Vector3 d0 = p1 - p0;
        Vector3 d1 = p2 - p1;
        return normalize_safe(cross(d0, d1));
    }

    void assign(lray::Vector3& dst, const cppobj::Vector3& src)
    {
        dst.x_ = src.x_;
        dst.y_ = src.y_;
        dst.z_ = src.z_;
    }

    void assign(lray::Vector2& dst, const cppobj::Vector2& src)
    {
        dst.x_ = src.x_;
        dst.y_ = src.y_;
    }

    f32 conv2d(u32 size, const f32* x0, const f32* x1)
    {
        f32 t = 0.0f;
        for(u32 i=0; i<size; ++i){
            t += x0[i] * x1[i];
        }
        return t;
    }

    Color16 convertDisolveToTransparency(u32 x,u32 y, const Texture& tex)
    {
        Color32 c = tex.get(x, y);
        c.r_ = clamp01(1.0f-c.r_);
        c.g_ = clamp01(1.0f-c.g_);
        c.b_ = clamp01(1.0f-c.b_);
        c.a_ = clamp01(c.a_);
        return toColor16(c);
    }

    void disolveToTransparency(IntrusivePtr<Texture>& tex)
    {
        if(!tex){
            return;
        }
        tex->convert(convertDisolveToTransparency);
    }

    Color16 convertNsToRoughness(u32 x,u32 y, const Texture& tex)
    {
        Color32 c = tex.get(x, y);
        c.r_ = clamp01(Material::NsToRoughness(c.r_*256.0f));
        c.g_ = clamp01(Material::NsToRoughness(c.g_*256.0f));
        c.b_ = clamp01(Material::NsToRoughness(c.b_*256.0f));
        c.a_ = clamp01(c.a_);
        return toColor16(c);
    }

    void NsToRoughness(IntrusivePtr<Texture>& tex)
    {
        if(!tex){
            return;
        }
        tex->convert(convertNsToRoughness);
    }

    Color16 convertBumpToNormap(u32 x,u32 y, const Texture& tex)
    {
        Color32 pixels[9];
        tex.get9pixels(pixels, x, y);
        f32 grays[9];
        for(u32 i=0; i<9; ++i){
            grays[i] = toGray(pixels[i]);
        }
        const f32 sobelx[9] = {
            1.0f, 0.0f, -1.0f,
            2.0f, 0.0f, -2.0f,
            1.0f, 0.0f, -1.0f,
        };
        const f32 sobely[9] = {
            1.0f, 2.0f, 1.0f,
            0.0f, 0.0f, 0.0f,
            -1.0f, -2.0f, -1.0f,
        };
        f32 gx = conv2d(9, sobelx, grays);
        f32 gy = conv2d(9, sobely, grays);
        Vector3 n(gx,gy,1.0f);
        n = 0.5f*normalize(n) + Vector3(0.5f,0.5f,0.5f);
        Color32 c = {clamp01(n.x_), clamp01(n.y_), clamp01(n.z_), 1.0f};
        return toColor16(c);
    }

    void bumpToNormal(IntrusivePtr<Texture>& tex)
    {
        if(!tex){
            return;
        }
        tex->convert(convertBumpToNormap);
    }
    
    IntrusivePtr<Texture> loadTexture(const std::string& path)
    {
        std::filesystem::path filepath(path);
        std::filesystem::path fullpath = std::filesystem::current_path();
        fullpath /= filepath;
        return Texture::load(fullpath.string().c_str());
    }

    void loadTextures(Material& dst, cppobj::Material& src)
    {
        dst.texAmbient_ = loadTexture(src.map_Ka_);
        dst.texAlbedo_ = loadTexture(src.map_Kd_);
        dst.texSpecular_ = loadTexture(src.map_Ks_);
        dst.texEmissive_ = loadTexture(src.map_Ke_);
        dst.texRoughness_ = loadTexture(src.map_Ns_);
        dst.texTransparency_ = loadTexture(src.map_d_);
        dst.texNormal_ = loadTexture(src.map_bump_);

        NsToRoughness(dst.texRoughness_);
        disolveToTransparency(dst.texTransparency_);
        bumpToNormal(dst.texNormal_);
    }
}

IntrusivePtr<Object> Object::load(const char* filename)
{
    assert(nullptr != filename);
    cppobj::Parser parser;
    if(!parser.parse(filename)){
        return nullptr;
    }
    IntrusivePtr<Object> object = new Object;
    object->faces_.resize(parser.getNumFaces());
    object->vertices_.resize(parser.getNumVertices());
    object->materials_.resize(parser.getNumMaterials());

    for(u32 i=0; i<parser.getNumFaces(); ++i){
        const auto& f = parser.getFace(i);
        object->faces_[i].material_ = f.material_;
        object->faces_[i].v0_ = f.p0_;
        object->faces_[i].v1_ = f.p1_;
        object->faces_[i].v2_ = f.p2_;
    }

    for(u32 i=0; i<parser.getNumVertices(); ++i){
        const auto& v = parser.getVertex(i);
        assign(object->vertices_[i].position_, v.position_);
        assign(object->vertices_[i].texcoord_, v.texcoord_);
        assign(object->vertices_[i].normal_, v.normal_);
        object->vertices_[i].binormal0_ = Vector3::Zero;
        object->vertices_[i].texcoord_.x_ = correctTexcoord(object->vertices_[i].texcoord_.x_);
        object->vertices_[i].texcoord_.y_ = correctTexcoord(object->vertices_[i].texcoord_.y_);
    }

    for(u32 i=0; i<parser.getNumMaterials(); ++i){
        const auto& m = parser.getMaterial(i);
        assign(object->materials_[i].ambient_, m.Ka_);
        assign(object->materials_[i].albedo_, m.Kd_);
        assign(object->materials_[i].specular_, m.Ks_);
        assign(object->materials_[i].emissive_, m.Ke_);
        object->materials_[i].transparency_ = 1.0f - m.d_;
        object->materials_[i].roughness_ = Material::NsToRoughness(m.Ns_);
        object->materials_[i].metallic_ = 0.0f;
        object->materials_[i].refIndex_ = m.Ni_;
    }

    if(0<object->vertices_.size()){
        object->bounds_.setInvalid();
        for(u32 i=0; i<object->vertices_.size(); ++i){
            object->bounds_.min_ = minimum(object->bounds_.min_, object->vertices_[i].position_);
            object->bounds_.max_ = maximum(object->bounds_.max_, object->vertices_[i].position_);
        }
    }else{
        object->bounds_.setZero();
    }
    return object;
}

Object::Object()
    :refCount_(0)
{
	bounds_.setZero();
}

Object::~Object()
{
}

const AABB& Object::getAABB() const
{
	return bounds_;
}

u32 Object::getNumFaces() const
{
	return faces_.size();
}

AABB Object::getAABB(u32 face) const
{
	u32 v0 = faces_[face].v0_;
	u32 v1 = faces_[face].v1_;
	u32 v2 = faces_[face].v2_;
	AABB aabb;
	aabb.min_ = minimum(vertices_[v0].position_, vertices_[v1].position_);
	aabb.max_ = maximum(vertices_[v0].position_, vertices_[v1].position_);
	aabb.min_ = minimum(vertices_[v2].position_, aabb.min_);
	aabb.max_ = maximum(vertices_[v2].position_, aabb.max_);
	return aabb;
}

void Object::recalcNormals()
{
    for(u32 i=0; i<vertices_.size(); ++i){
        vertices_[i].normal_ = Vector3::Zero;
    }

    for(u32 i=0; i<faces_.size(); ++i){
        u32 v0 = faces_[i].v0_;
        u32 v1 = faces_[i].v1_;
        u32 v2 = faces_[i].v2_;
        Vector3 n = calcNormal(vertices_[v0].position_, vertices_[v1].position_, vertices_[v2].position_);
        vertices_[v0].normal_ += n;
        vertices_[v1].normal_ += n;
        vertices_[v2].normal_ += n;
    }
    for(u32 i=0; i<vertices_.size(); ++i){
        vertices_[i].normal_ = normalize_safe(vertices_[i].normal_);
    }
}

void Object::generateTangents()
{
    SMikkTSpaceInterface interface;
    interface.m_getNumFaces = Object::getNumFaces;
    interface.m_getNumVerticesOfFace = Object::getNumVerticesOfFace;
    interface.m_getPosition = Object::getPosition;
    interface.m_getNormal = Object::getNormal;
    interface.m_getTexCoord = Object::getTexCoord;
    interface.m_setTSpaceBasic = Object::setTSpaceBasic;
    SMikkTSpaceContext context;
    context.m_pInterface = &interface;
    context.m_pUserData = this;
    genTangSpaceDefault(&context);
}

u32 Object::getVertex(const int face, const int vert) const
{
    switch(vert){
    case 0:
        return faces_[face].v0_;
    case 1:
        return faces_[face].v1_;
    case 2:
        return faces_[face].v2_;
    default:
        assert(false);
        return 0;
    }
}

int Object::getNumFaces(const SMikkTSpaceContext* context)
{
    const Object* obj = reinterpret_cast<const Object*>(context->m_pUserData);
    return static_cast<int>(obj->faces_.size());
}

int Object::getNumVerticesOfFace(const SMikkTSpaceContext* context, const int face)
{
    return 3;
}

void Object::getPosition(const SMikkTSpaceContext* context, float fvPosOut[], const int face, const int vert)
{
    const Object* obj = reinterpret_cast<const Object*>(context->m_pUserData);
    u32 v = obj->getVertex(face, vert);
    fvPosOut[0] = obj->vertices_[v].position_.x_;
    fvPosOut[1] = obj->vertices_[v].position_.y_;
    fvPosOut[2] = obj->vertices_[v].position_.z_;
}

void Object::getNormal(const SMikkTSpaceContext* context, float fvNormOut[], const int face, const int vert)
{
    const Object* obj = reinterpret_cast<const Object*>(context->m_pUserData);
    u32 v = obj->getVertex(face, vert);
    fvNormOut[0] = obj->vertices_[v].normal_.x_;
    fvNormOut[1] = obj->vertices_[v].normal_.y_;
    fvNormOut[2] = obj->vertices_[v].normal_.z_;
}

void Object::getTexCoord(const SMikkTSpaceContext* context, float fvTexcOut[], const int face, const int vert)
{
    const Object* obj = reinterpret_cast<const Object*>(context->m_pUserData);
    u32 v = obj->getVertex(face, vert);
    fvTexcOut[0] = obj->vertices_[v].texcoord_.x_;
    fvTexcOut[1] = obj->vertices_[v].texcoord_.y_;
}

void Object::setTSpaceBasic(const SMikkTSpaceContext * context, const float fvTangent[], const float fSign, const int face, const int vert)
{
    Object* obj = const_cast<Object*>(reinterpret_cast<const Object*>(context->m_pUserData));
    u32 v;
    switch(vert){
    case 0:
        v = obj->faces_[face].v0_;
        break;
    case 1:
        v = obj->faces_[face].v1_;
        break;
    case 2:
        v = obj->faces_[face].v2_;
        break;
    default:
        return;
    }
    Vector3 tangent = {fvTangent[0], fvTangent[1], fvTangent[2]};
    Vector3 bitangent = fSign * cross(obj->vertices_[v].normal_, tangent);
    obj->vertices_[v].binormal0_ = cross(bitangent, obj->vertices_[v].normal_);
}

bool Object::testRay(f32& t, u32 face, const Ray& ray, f32 tmin, f32 tmax) const
{
    const Vector3& v0 = vertices_[faces_[face].v0_].position_;
    const Vector3& v1 = vertices_[faces_[face].v1_].position_;
    const Vector3& v2 = vertices_[faces_[face].v2_].position_;

    Vector3 d0 = v1 - v0;
    Vector3 d1 = v2 - v0;
    Vector3 c = cross(ray.direction_, d1);

    Vector3 tvec;
    f32 discr = dot(c, d0);
    Vector3 qvec;
    if(Epsilon < discr) {
        // front
        tvec = ray.origin_ - v0;
        f32 v = dot(tvec, c);
        if(v < 0.0f || discr < v) {
            return false;
        }
        qvec = cross(tvec, d0);
        f32 w = dot(qvec, ray.direction_);
        if(w < 0.0f || discr < (v + w)) {
            return false;
        }
    } else {
        return false;
    }

    f32 invDiscr = 1.0f / discr;

    t = dot(d1, qvec);
    t *= invDiscr;
    return true;
}

bool Object::testRayBoth(f32& t, u32 face, const Ray& ray, f32 tmin, f32 tmax) const
{
    const Vector3& v0 = vertices_[faces_[face].v0_].position_;
    const Vector3& v1 = vertices_[faces_[face].v1_].position_;
    const Vector3& v2 = vertices_[faces_[face].v2_].position_;

    Vector3 d0 = v1 - v0;
    Vector3 d1 = v2 - v0;
    Vector3 c = cross(ray.direction_, d1);

    Vector3 tvec;
    f32 discr = dot(c, d0);
    Vector3 qvec;
    if(Epsilon < discr) {
        // front
        tvec = ray.origin_ - v0;
        f32 v = dot(tvec, c);
        if(v < 0.0f || discr < v) {
            return false;
        }
        qvec = cross(tvec, d0);
        f32 w = dot(qvec, ray.direction_);
        if(w < 0.0f || discr < (v + w)) {
            return false;
        }

    } else if(discr < -Epsilon) {
        // behind
        tvec = ray.origin_ - v0;
        f32 v = dot(tvec, c);
        if(0.0f < v || v < discr) {
            return false;
        }
        qvec = cross(tvec, d0);
        f32 w = dot(qvec, ray.direction_);
        if(0.0f < w || (v + w) < discr) {
            return false;
        }

    } else {
        return false;
    }

    f32 invDiscr = 1.0f / discr;

    t = dot(d1, qvec);
    t *= invDiscr;
    return true;
}

void intrusive_ptr_addref(Object* pointer)
{
    pointer->refCount_ += 1;
}

void intrusive_ptr_release(Object* pointer)
{
    pointer->refCount_ -= 1;
    if(pointer->refCount_ <= 0) {
        delete pointer;
    }
}

} // namespace lray
