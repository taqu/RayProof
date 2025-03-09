#ifndef INC_LRAY_COMMON_H_
#define INC_LRAY_COMMON_H_
#include <cassert>
#include <cmath>
#include <cstdint>
#include <numbers>
#include <immintrin.h>
#include "mimalloc.h"

#ifndef LRAY_MALLOC
#define LRAY_MALLOC(size) ::mi_malloc(size)
#endif
#ifndef LRAY_FREE
#define LRAY_FREE(ptr) ::mi_free(ptr)
#endif

#ifdef _DEBUG
#define LRAY_ASSERT(exp) assert((exp))
#else
#define LRAY_ASSERT(exp) (void)0
#endif

#ifdef _DEBUG
void* operator new(std::size_t size);
#else
void* operator new(std::size_t size, const char* file, int line);
#endif

void* operator new(std::size_t size, std::align_val_t alignment);
void* operator new(std::size_t size, const std::nothrow_t&) noexcept;
void* operator new(std::size_t size, std::align_val_t alignment, const std::nothrow_t&) noexcept;

#ifdef _DEBUG
void* operator new[](std::size_t size);
#else
void* operator new[](std::size_t size, const char* file, int line);
#endif

void* operator new[](std::size_t size, std::align_val_t alignment);
void* operator new[](std::size_t size, const std::nothrow_t&) noexcept;
void* operator new[](std::size_t size, std::align_val_t alignment, const std::nothrow_t&) noexcept;

namespace lray
{
using s8 = int8_t;
using s16 = int16_t;
using s32 = int32_t;
using s64 = int64_t;

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using f32 = float;
using f64 = double;

#ifdef _MSC_VER
#    define LRAY_ALIGN16 __declspec(align(16))
#    define LRAY_ALIGN(x) __declspec(align(x))
#else
#    define LRAY_ALIGN16 __attribute__((align(16)))
#    define LRAY_ALIGN(x) __attribute__((align(x)))
#endif

inline static constexpr f32 F32_EPSILON = 1.0e-7f;
inline static constexpr f32 RAY_F32_EPSILON = 1.0e-6f;
inline static constexpr f32 F32_PI = std::numbers::pi_v<float>;
inline static constexpr f32 BBoxEpsilon = 1.0e-6f;
inline static constexpr f32 HitEpsilon = 1.0e-6f;

class Random;
class Vector3;
class AABB;

template<class T>
void swap(T& x0, T& x1)
{
    T tmp = x0;
    x0 = x1;
    x1 = tmp;
}
template<class T>
T absolute(const T& x)
{
    return std::abs(x);
}
template<class T>
T minimum(const T& x0, const T& x1)
{
    return (x0 < x1) ? x0 : x1;
}
template<class T>
T maximum(const T& x0, const T& x1)
{
    return (x0 < x1) ? x1 : x0;
}
bool isEquals(f32 x0, f32 x1, f32 epsilon = F32_EPSILON);
bool isZero(f32 x, f32 epsilon = F32_EPSILON);
f32 clamp01(f32 x);
inline f32 saturate(f32 x)
{
    return clamp01(x);
}
f32 lerp(f32 x0, f32 x1, f32 t);
u32 leadingzero(u32 x);
u32 mortonCode3(u32 x, u32 y, u32 z);
f32 schilick(f32 cosine, f32 refIndex);

template<class T, class U>
void insertionsort(u32 n, T* v, U func)
{
    for(u32 i = 1; i < n; ++i) {
        for(u32 j = i; 0 < j; --j) {
            u32 k = j - 1;
            if(func(v[j], v[k])) {
                swap(v[j], v[k]);
            } else {
                break;
            }
        }
    }
}

//--- heapsort
//------------------------------------------------
/**
 */
template<class T, class U>
void heapsort(u32 n, T* v, U func)
{
    assert(0 <= n);

    --v;
    u32 i, j;
    T x;
    for(u32 k = n >> 1; k >= 1; --k) {
        i = k;
        x = v[k];
        while((j = i << 1) <= n) {
            if(j < n && func(v[j], v[j + 1])) {
                ++j;
            }

            if(!func(x, v[j])) {
                break;
            }
            v[i] = v[j];
            i = j;
        }
        v[i] = x;
    }

    while(n > 1) {
        x = v[n];
        v[n] = v[1];
        --n;
        i = 1;
        while((j = i << 1) <= n) {
            if(j < n && func(v[j], v[j + 1])) {
                ++j;
            }

            if(!func(x, v[j])) {
                break;
            }
            v[i] = v[j];
            i = j;
        }
        v[i] = x;
    }
}

//--- quicksort
//------------------------------------------------
/**
 */
template<class T, class U>
void quicksort(u32 n, T* v, U func)
{
    static const u32 SwitchN = 47;
    if(n < SwitchN) {
        insertionsort(n, v, func);
        return;
    }

    s32 i0 = 0;
    s32 i1 = static_cast<s32>(n) - 1;

    T pivot = v[(i0 + i1) >> 1];

    for(;;) {
        while(func(v[i0], pivot)) {
            ++i0;
        }

        while(func(pivot, v[i1])) {
            --i1;
        }

        if(i1 <= i0) {
            break;
        }
        swap(v[i0], v[i1]);
        ++i0;
        --i1;
    }

    if(1 < i0) {
        quicksort(i0, v, func);
    }

    ++i1;
    n = n - i1;
    if(1 < n) {
        quicksort(n, v + i1, func);
    }
}

//--- introsort
//------------------------------------------------
/**
 */
template<class T, class U>
void introsort(u32 n, T* v, u32 depth, U func)
{
    static const u32 SwitchN = 47;
    if(n < SwitchN) {
        insertionsort(n, v, func);
        return;
    }
    if(depth <= 0) {
        heapsort(n, v, func);
        return;
    }

    s32 i0 = 0;
    s32 i1 = static_cast<s32>(n) - 1;

    T pivot = v[(i0 + i1) >> 1];

    for(;;) {
        while(func(v[i0], pivot)) {
            ++i0;
        }

        while(func(pivot, v[i1])) {
            --i1;
        }

        if(i1 <= i0) {
            break;
        }
        swap(v[i0], v[i1]);
        ++i0;
        --i1;
    }

    --depth;
    if(1 < i0) {
        introsort(i0, v, depth, func);
    }

    ++i1;
    n = n - i1;
    if(1 < n) {
        introsort(n, v + i1, depth, func);
    }
}

template<class T, class U>
void introsort(u32 n, T* v, U func)
{
    u32 depth = 0;
    u32 t = n;
    while(1 < t) {
        ++depth;
        t >>= 1;
    }
    introsort(n, v, depth, func);
}

s32 testRayAABB(
    __m128 tmin,
    __m128 tmax,
    __m128 origin[3],
    __m128 invDir[3],
    const u32 sign[3],
    const __m128 bbox[2][3]);

template<class T>
class IntrusivePtr
{
public:
    IntrusivePtr();
    IntrusivePtr(T* pointer);
    IntrusivePtr(const IntrusivePtr<T>& other);
    IntrusivePtr(IntrusivePtr<T>&& other);
    template<class U>
    IntrusivePtr(U* pointer);
    template<class U>
    IntrusivePtr(const IntrusivePtr<U>& other);
    template<class U>
    IntrusivePtr(IntrusivePtr<U>&& other);
    ~IntrusivePtr();

    IntrusivePtr<T>& operator=(const IntrusivePtr<T>& other);
    IntrusivePtr<T>& operator=(IntrusivePtr<T>&& other);
    IntrusivePtr<T>& operator=(T* other);

    template<class U>
    IntrusivePtr<T>& operator=(const IntrusivePtr<U>& rhs);
    template<class U>
    IntrusivePtr<T>& operator=(IntrusivePtr<U>&& rhs);
    template<class U>
    IntrusivePtr<T>& operator=(U* other);

    void swap(IntrusivePtr<T>& other);

    const T& operator*() const
    {
        return *pointer_;
    }

    T& operator*()
    {
        return *pointer_;
    }

    const T* operator->() const
    {
        return pointer_;
    }

    T* operator->()
    {
        return pointer_;
    }

    operator bool() const
    {
        return (nullptr != pointer_);
    }

    bool operator!() const
    {
        return (nullptr == pointer_);
    }
    const T* get() const
    {
        return pointer_;
    }

    T* get()
    {
        return pointer_;
    }

private:
    T* pointer_;
};

template<class T>
IntrusivePtr<T>::IntrusivePtr()
    : pointer_(nullptr)
{
}

template<class T>
IntrusivePtr<T>::IntrusivePtr(T* pointer)
    : pointer_(pointer)
{
    if(nullptr != pointer_) {
        intrusive_ptr_addref(pointer_);
    }
}

template<class T>
IntrusivePtr<T>::IntrusivePtr(const IntrusivePtr<T>& other)
    : pointer_(other.pointer_)
{
    if(nullptr != pointer_) {
        intrusive_ptr_addref(pointer_);
    }
}

template<class T>
IntrusivePtr<T>::IntrusivePtr(IntrusivePtr<T>&& other)
    : pointer_(other.pointer_)
{
    other.pointer_ = nullptr;
}

template<class T>
template<class U>
IntrusivePtr<T>::IntrusivePtr(U* pointer)
    : pointer_(pointer)
{
    if(nullptr != pointer_) {
        intrusive_ptr_addref(pointer_);
    }
}

template<class T>
template<class U>
IntrusivePtr<T>::IntrusivePtr(const IntrusivePtr<U>& other)
    : pointer_(other.pointer_)
{
    if(nullptr != pointer_) {
        intrusive_ptr_addref(pointer_);
    }
}

template<class T>
template<class U>
IntrusivePtr<T>::IntrusivePtr(IntrusivePtr<U>&& other)
    : pointer_(other.pointer_)
{
    other.pointer_ = nullptr;
}

template<class T>
IntrusivePtr<T>::~IntrusivePtr()
{
    if(nullptr != pointer_) {
        intrusive_ptr_release(pointer_);
        pointer_ = nullptr;
    }
}
template<class T>
IntrusivePtr<T>& IntrusivePtr<T>::operator=(const IntrusivePtr<T>& other)
{
    IntrusivePtr<T>(other).swap(*this);
    return *this;
}

template<class T>
IntrusivePtr<T>& IntrusivePtr<T>::operator=(IntrusivePtr<T>&& other)
{
    if(this == &other) {
        return *this;
    }
    if(nullptr != pointer_) {
        intrusive_ptr_release(pointer_);
    }
    pointer_ = other.pointer_;
    other.pointer_ = nullptr;
    return *this;
}

template<class T>
IntrusivePtr<T>& IntrusivePtr<T>::operator=(T* other)
{
    IntrusivePtr<T>(other).swap(*this);
    return *this;
}

template<class T>
template<class U>
IntrusivePtr<T>& IntrusivePtr<T>::operator=(const IntrusivePtr<U>& other)
{
    IntrusivePtr<T>(other).swap(*this);
    return *this;
}

template<class T>
template<class U>
IntrusivePtr<T>& IntrusivePtr<T>::operator=(IntrusivePtr<U>&& other)
{
    if(this == &other) {
        return *this;
    }
    if(nullptr != pointer_) {
        intrusive_ptr_release(pointer_);
    }
    pointer_ = other.pointer_;
    other.pointer_ = nullptr;
    return *this;
}

template<class T>
template<class U>
IntrusivePtr<T>& IntrusivePtr<T>::operator=(U* other)
{
    IntrusivePtr<T>(other).swap(*this);
    return *this;
}

template<class T>
void IntrusivePtr<T>::swap(IntrusivePtr<T>& other)
{
    T* pointer = pointer_;
    pointer_ = other.pointer_;
    other.pointer_ = pointer;
}

template<class T, class U>
inline bool operator!=(const IntrusivePtr<T>& left, const IntrusivePtr<U>& right)
{
    return left.get() != right.get();
}

template<class T, class U>
inline bool operator==(const IntrusivePtr<T>& left, const U* right)
{
    return left.get() == right;
}

template<class T, class U>
inline bool operator!=(const IntrusivePtr<T>& left, const U* right)
{
    return left.get() != right;
}

template<class T, class U>
inline bool operator==(const T* left, const IntrusivePtr<U>& right)
{
    return left == right.get();
}

template<class T, class U>
inline bool operator!=(const T* left, const IntrusivePtr<U>& right)
{
    return left != right.get();
}
} // namespace lray
#endif // INC_LRAY_COMMON_H_
