#include "Core.h"
#include <cmath>
#include <random>
#include <thread>
#include <bit>
#ifdef _DEBUG
#include <string>
#include <unordered_map>
#endif
#include "Vector.h"
#include "AABB.h"

#ifdef _WIN32
#    include <Windows.h>
#    include <processthreadsapi.h>
#else
#    include <sched.h>
#endif
#include "Random.h"

#ifdef _DEBUG
namespace
{
}
#endif

void* operator new(std::size_t size, const char* file, int line)
{
    return mi_malloc(size);
}

void* operator new(std::size_t size, const std::nothrow_t&, const char* file, int line) noexcept
{
    return mi_malloc(size);
}

void* operator new(std::size_t size)
{
    return mi_malloc(size);
}

void* operator new(std::size_t size, const std::nothrow_t&) noexcept
{
    return mi_malloc(size);
}

void* operator new(std::size_t size, std::align_val_t alignment)
{
    return mi_malloc_aligned(size, (size_t)alignment);
}

void* operator new(std::size_t size, std::align_val_t alignment, const std::nothrow_t&) noexcept
{
    return mi_malloc_aligned(size, (size_t)alignment);
}

void* operator new[](std::size_t size, const char* file, int line)
{
    return mi_malloc(size);
}

void* operator new[](std::size_t size, const std::nothrow_t&, const char* file, int line) noexcept
{
    return mi_malloc(size);
}
void* operator new[](std::size_t size)
{
    return mi_malloc(size);
}

void* operator new[](std::size_t size, const std::nothrow_t&) noexcept
{
    return mi_malloc(size);
}

void* operator new[](std::size_t size, std::align_val_t alignment)
{
    return mi_malloc_aligned(size, (size_t)alignment);
}

void* operator new[](std::size_t size, std::align_val_t alignment, const std::nothrow_t&) noexcept
{
    return mi_malloc_aligned(size, (size_t)alignment);
}

void operator delete(void* ptr) noexcept
{
    mi_free(ptr);
}

void operator delete(void* ptr, std::size_t size) noexcept
{
    mi_free_size(ptr, size);
}

void operator delete(void* ptr, std::align_val_t alignment) noexcept
{
    mi_free_aligned(ptr, (size_t)alignment);
}

void operator delete(void* ptr, std::size_t size, std::align_val_t alignment) noexcept
{
    mi_free_size_aligned(ptr, size, (size_t)alignment);
}

void operator delete(void* ptr, const std::nothrow_t&) noexcept
{
    mi_free(ptr);
}

void operator delete(void* ptr, std::align_val_t alignment, const std::nothrow_t&) noexcept
{
     mi_free_aligned(ptr, (size_t)alignment);
}

void operator delete[](void* ptr) noexcept
{
    mi_free(ptr);
}

void operator delete[](void* ptr, std::size_t size) noexcept
{
    mi_free_size(ptr, size);
}

void operator delete[](void* ptr, std::align_val_t alignment) noexcept
{
    mi_free_aligned(ptr, (size_t)alignment);
}

void operator delete[](void* ptr, std::size_t size, std::align_val_t alignment) noexcept
{
    mi_free_size_aligned(ptr, size, (size_t)alignment);
}

void operator delete[](void* ptr, const std::nothrow_t&) noexcept
{
    mi_free(ptr);
}

void operator delete[](void* ptr, std::align_val_t alignment, const std::nothrow_t&) noexcept
{
     mi_free_aligned(ptr, (size_t)alignment);
}

namespace lray
{
namespace
{
    s32 getCurrentCore()
    {
#ifdef _WIN32
        return static_cast<s32>(GetCurrentProcessorNumber());
#else
        return sched_getcpu();
#endif
    }
} // namespace

bool isEquals(f32 x0, f32 x1, f32 epsilon)
{
    return std::abs(x0 - x1) < epsilon;
}

bool isZero(f32 x, f32 epsilon)
{
    return std::abs(x) < epsilon;
}

f32 clamp01(f32 x)
{
    s32 u = std::bit_cast<s32, f32>(x);
    s32 s = u >> 31;
    s = ~s;
    u &= s;

    x = std::bit_cast<f32, s32>(u) - 1.0f;

    u = std::bit_cast<s32, f32>(x);
    s = u >> 31;
    u &= s;
    x = std::bit_cast<f32, s32>(u) + 1.0f;
    return x;
}

f32 lerp(f32 x0, f32 x1, f32 t)
{
    assert(0.0f <= t && t <= 1.0f);
    return x0 * (1.0f - t) + x1 * t;
}

u32 leadingzero(u32 x)
{
#if defined(_MSC_VER)
    unsigned long n;
    _BitScanReverse(&n, x);
    return 31 - n;
#elif defined(__GNUC__)
    return __builtin_clz(x);
#else
    u32 n = 0;
    if(x <= 0x0000FFFFU) {
        n += 16;
        x <<= 16;
    }
    if(x <= 0x00FFFFFFU) {
        n += 8;
        x <<= 8;
    }
    if(x <= 0x0FFFFFFFU) {
        n += 4;
        x <<= 4;
    }
    if(x <= 0x3FFFFFFFU) {
        n += 2;
        x <<= 2;
    }
    if(x <= 0x7FFFFFFFU) {
        ++n;
    }
    return n;
#endif
}

namespace
{
    u32 separateBy2(u32 x)
    {
        x = (x | (x << 8) | (x << 16)) & 0x0300F00FU;
        x = (x | (x << 4) | (x << 8)) & 0x030C30C3U;
        x = (x | (x << 2) | (x << 4)) & 0x09249249U;
        return x;
    }

    u32 combineBy2(u32 x)
    {
        x &= 0x09249249U;
        x = (x | (x >> 2) | (x >> 4)) & 0x030C30C3U;
        x = (x | (x >> 4) | (x >> 8)) & 0x0300F00FU;
        x = (x | (x >> 8) | (x >> 16)) & 0x3FFU;
        return x;
    }
} // namespace

u32 mortonCode3(u32 x, u32 y, u32 z)
{
    return separateBy2(x) | (separateBy2(y) << 1) | (separateBy2(z) << 2);
}

f32 schilick(f32 cosine, f32 refIndex)
{
    f32 r0 = (1.0f - refIndex) / (1.0f + refIndex);
    r0 = r0 * r0;
    return r0 + (1.0f - r0) * std::pow(1.0f - cosine, 5.0f);
}

bool testRayAABB(
    f32 tmin,
    f32 tmax,
    const Vector3& origin,
    const Vector3& invDir,
    const u32 sign[3],
    const AABB& bbox)
{
    for(u32 i = 0; i < 3; ++i) {
        tmin = maximum(tmin, (bbox[sign[i]][i] - origin[i])*invDir[i]);
        tmax = minimum(tmax, (bbox[1-sign[i]][i] - origin[i])*invDir[i]);
    }
    return tmin<tmax;
}

s32 testRayAABB(
    __m128 tmin,
    __m128 tmax,
    __m128 origin[3],
    __m128 invDir[3],
    const u32 sign[3],
    const __m128 bbox[2][3])
{
    for(u32 i = 0; i < 3; ++i) {
        tmin = _mm_max_ps(
            tmin,
            _mm_mul_ps(_mm_sub_ps(bbox[sign[i]][i], origin[i]), invDir[i]));

        tmax = _mm_min_ps(
            tmax,
            _mm_mul_ps(_mm_sub_ps(bbox[1 - sign[i]][i], origin[i]), invDir[i]));
    }

    return _mm_movemask_ps(_mm_cmpge_ps(tmax, tmin));
}
} // namespace lray
