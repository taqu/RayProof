#ifndef INC_LRAY_FIXED16_H_
#define INC_LRAY_FIXED16_H_
#include "Core.h"

namespace lray
{
struct Fixed16
{
    inline static constexpr uint16_t Frac = 6;

    Fixed16();
    explicit Fixed16(uint16_t x);
    Fixed16(const Fixed16&) = default;
    Fixed16& operator=(const Fixed16&) = default;
    explicit Fixed16(float x);
    Fixed16& operator=(float x);

    operator float() const;

    friend bool operator==(const Fixed16& x0, const Fixed16& x1);
    friend bool operator!=(const Fixed16& x0, const Fixed16& x1);
    friend bool operator<(const Fixed16& x0, const Fixed16& x1);
    friend bool operator<=(const Fixed16& x0, const Fixed16& x1);
    friend bool operator>(const Fixed16& x0, const Fixed16& x1);
    friend bool operator>=(const Fixed16& x0, const Fixed16& x1);

    friend Fixed16 operator+(const Fixed16& x0, const Fixed16& x1);
    friend Fixed16 operator-(const Fixed16& x0, const Fixed16& x1);

    friend Fixed16 operator*(const Fixed16& x0, const Fixed16& x1);
    friend Fixed16 operator/(const Fixed16& x0, const Fixed16& x1);

    uint16_t x_;
};

bool operator==(const Fixed16& x0, const Fixed16& x1);
bool operator!=(const Fixed16& x0, const Fixed16& x1);

bool operator<(const Fixed16& x0, const Fixed16& x1);
bool operator<=(const Fixed16& x0, const Fixed16& x1);
bool operator>(const Fixed16& x0, const Fixed16& x1);
bool operator>=(const Fixed16& x0, const Fixed16& x1);

Fixed16 operator+(const Fixed16& x0, const Fixed16& x1);
Fixed16 operator-(const Fixed16& x0, const Fixed16& x1);

Fixed16 operator*(const Fixed16& x0, const Fixed16& x1);
Fixed16 operator/(const Fixed16& x0, const Fixed16& x1);
} // namespace ray
#endif // INC_LRAY_FIXED16_H_
