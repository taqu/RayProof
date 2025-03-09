#ifndef INC_LRAY_ARRAY_H_
#define INC_LRAY_ARRAY_H_
#include "Core.h"
#include <type_traits>

namespace lray
{
template<class T>
class Array
{
public:
    inline static constexpr u32 Expand = 64;
    Array();
    ~Array();
    Array(const Array& other);
    Array(Array&& other);
    Array& operator=(const Array& other);
    Array& operator=(Array&& other);

    u32 capacity() const;
    u32 size() const;
    void reserve(u32 capacity);
    void resize(u32 size);
    const T& operator[](u32 index) const;
    T& operator[](u32 index);

    void clear();
    void push_back(const T& x);
    void pop_back();
    void removeAt(u32 index);

private:
    void expand(u32 capacity);
    void destroy();
    u32 capacity_;
    u32 size_;
    T* items_;
};

template<class T>
Array<T>::Array()
    : capacity_(0)
    , size_(0)
    , items_(nullptr)
{
}

template<class T>
Array<T>::~Array()
{
    destroy();
    size_ = 0;
    capacity_ = 0;
}

template<class T>
Array<T>::Array(const Array& other)
    : capacity_(other.capacity_)
    , size_(other.size_)
    , items_(nullptr)
{
    items_ = static_cast<T*>(::malloc(sizeof(T)*capacity_));
    for(u32 i=0; i<size_; ++i){
        new(&items_[i]) T(other.items_[i]);
    }
}

template<class T>
Array<T>::Array(Array&& other)
    : capacity_(other.capacity_)
    , size_(other.size_)
    , items_(other.items_)
{
    other.capacity_ = 0;
    other.size_ = 0;
    other.items_ = nullptr;
}

template<class T>
Array<T>& Array<T>::operator=(const Array& other)
{
    if(this == &other) {
        return *this;
    }
    destroy();
    capacity_ = other.capacity_;
    size_ = other.size_;
    items_ = static_cast<T*>(::malloc(sizeof(T)*capacity_));
    for(u32 i=0; i<size_; ++i){
        new(&items_[i]) T(other.items_[i]);
    }
    return *this;
}

template<class T>
Array<T>& Array<T>::operator=(Array&& other)
{
    if(this == &other) {
        return *this;
    }
    destroy();
    capacity_ = other.capacity_;
    size_ = other.size_;
    items_ = other.items_;
    other.items_ = nullptr;
    other.capacity_ = 0;
    other.size_ = 0;
    return *this;
}

template<class T>
u32 Array<T>::capacity() const
{
    return capacity_;
}

template<class T>
u32 Array<T>::size() const
{
    return size_;
}

template<class T>
void Array<T>::reserve(u32 capacity)
{
    if(capacity_<capacity){
        capacity = (capacity+Expand-1)&~(Expand-1);
        expand(capacity);
    }
}

template<class T>
    void Array<T>::resize(u32 size)
{
    if(capacity_ < size) {
        size = (size + Expand - 1) & ~(Expand - 1);
        expand(size);
    }
    if(size<size_){
        for(u32 i=size; i<size_; ++i){
            items_[i].~T();
        }
    }else{
        for(u32 i=size_; i<size; ++i){
            new(&items_[i]) T();
        }
    }
    size_ = size;
}

template<class T>
const T& Array<T>::operator[](u32 index) const
{
    assert(index < size_);
    return items_[index];
}

template<class T>
T& Array<T>::operator[](u32 index)
{
    assert(index < size_);
    return items_[index];
}

template<class T>
void Array<T>::clear()
{
    size_ = 0;
}

template<class T>
void Array<T>::push_back(const T& x)
{
    if(capacity_<=size_){
        expand(capacity_+Expand);
    }
    new(&items_[size_]) T(x);
    ++size_;
}

template<class T>
void Array<T>::pop_back()
{
    assert(0<size_);
    --size_;
    items_[size_].~T();
}

template<class T>
void Array<T>::removeAt(u32 index)
{
    assert(index<size_);
    for(u32 i=index+1; i<size_; ++i){
        items_[i-1] = items_[i];
    }
    --size_;
    items_[size_].~T();
}

template<class T>
void Array<T>::expand(u32 capacity)
{
    while(capacity_ < capacity) {
        capacity_ += Expand;
    }
    T* items = static_cast<T*>(::malloc(sizeof(T)*capacity_));
    for(u32 i=0; i<size_; ++i){
        new(&items[i]) T(items_[i]);
        items_[i].~T();
    }
    ::free(items_);
    items_ = items;
}

template<class T>
void Array<T>::destroy()
{
    for(u32 i=0; i<size_; ++i){
        items_[i].~T();
    }
    ::free(items_);
    items_ = nullptr;
}
} // namespace lray
#endif // INC_LRAY_ARRAY_H_
