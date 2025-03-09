#ifndef INC_LRAY_OBJECT_H_
#define INC_LRAY_OBJECT_H_
#include "Core.h"
#include "Array.h"
#include "Vertex.h"
#include "Face.h"
#include "Material.h"
#include "AABB.h"
#include "mikktspace.h"

namespace lray
{
    class Ray;

class Object
{
public:
    static IntrusivePtr<Object> load(const char* filename);

    Object();
    ~Object();
    const AABB& getAABB() const;
    u32 getNumFaces() const;
    AABB getAABB(u32 face) const;
    void recalcNormals();
    void generateTangents();
    bool testRay(f32& t, u32 face, const Ray& ray, f32 tmin, f32 tmax) const;
    bool testRayBoth(f32& t, u32 face, const Ray& ray, f32 tmin, f32 tmax) const;
private:
    Object(const Object&) = delete;
    Object& operator=(const Object&) = delete;

    friend void intrusive_ptr_addref(Object*);
    friend void intrusive_ptr_release(Object*);

    u32 getVertex(const int face, const int vert) const;
    static int getNumFaces(const SMikkTSpaceContext* context);
    static int getNumVerticesOfFace(const SMikkTSpaceContext* context, const int face);
    static void getPosition(const SMikkTSpaceContext * context, float fvPosOut[], const int face, const int vert);
	static void getNormal(const SMikkTSpaceContext * context, float fvNormOut[], const int face, const int vert);
	static void getTexCoord(const SMikkTSpaceContext * context, float fvTexcOut[], const int face, const int vert);
    static void setTSpaceBasic(const SMikkTSpaceContext * context, const float fvTangent[], const float fSign, const int face, const int vert);


    s32 refCount_;
    AABB bounds_;
    Array<Face> faces_;
    Array<Vertex> vertices_;
    Array<Material> materials_;
};

void intrusive_ptr_addref(Object* pointer);
void intrusive_ptr_release(Object* pointer);
}
#endif //INC_LRAY_OBJECT_H_
