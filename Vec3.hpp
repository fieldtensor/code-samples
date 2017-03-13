#ifndef STONE_VEC3_HPP
#define STONE_VEC3_HPP

#include <Stone/Types.hpp>
#include <stddef.h>
#include <cmath>

namespace SK
{

#define Template template<class num, size_t N>
#define TA num,N

template<class num, size_t N = alignof(num)>
class alignas(N) Vec3T
{
public:
    num x;
    num y;
    num z;

private:
    num padding;

public:
    Vec3T(num xi = 0.0, num yi = 0.0, num zi = 0.0);

    Vec3T operator+(const Vec3T& v) const;
    Vec3T operator-(const Vec3T& v) const;
    Vec3T operator*(num k) const;
    Vec3T operator/(num k) const;

    Vec3T& operator+=(const Vec3T& v);
    Vec3T& operator-=(const Vec3T& v);
    Vec3T& operator*=(num k);
    Vec3T& operator/=(num k);

    num operator*(const Vec3T& v) const;
    Vec3T cross(const Vec3T& v) const;

    num length() const;
    num length2() const;

    Vec3T normal() const;
    void normalize();

    num& operator[](size_t idx);
    num operator[](size_t idx) const;

    operator const num*() const;
    operator num*();
};

#ifdef SK_MATH_PURE_CPP

Template
SK::Vec3T<TA>::Vec3T(num xi, num yi, num zi) :
x(xi),
y(yi),
z(zi)
{
}

Template
SK::Vec3T<TA>
SK::Vec3T<TA>::operator+(const Vec3T<TA>& v) const
{
    return Vec3T(x + v.x, y + v.y, z + v.z);
}

Template
SK::Vec3T<TA>
SK::Vec3T<TA>::operator-(const Vec3T<TA>& v) const
{
    return Vec3T(x - v.x, y - v.y, z - v.z);
}

Template
SK::Vec3T<TA>
SK::Vec3T<TA>::operator*(num k) const
{
    return Vec3T(x * k, y * k, z * k);
}

Template
SK::Vec3T<TA>
SK::Vec3T<TA>::operator/(num k) const
{
    return Vec3T(x / k, y / k, z / k);
}

Template
SK::Vec3T<TA>&
SK::Vec3T<TA>::operator+=(const Vec3T<TA>& v)
{
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
}

Template
SK::Vec3T<TA>&
SK::Vec3T<TA>::operator-=(const Vec3T<TA>& v)
{
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
}

Template
SK::Vec3T<TA>&
SK::Vec3T<TA>::operator*=(num k)
{
    x *= k;
    y *= k;
    z *= k;
    return *this;
}

Template
SK::Vec3T<TA>&
SK::Vec3T<TA>::operator/=(num k)
{
    x /= k;
    y /= k;
    z /= k;
    return *this;
}

Template
num
SK::Vec3T<TA>::operator*(const Vec3T<TA>& v) const
{
    return x * v.x + y * v.y + z * v.z;
}

Template
SK::Vec3T<TA>
SK::Vec3T<TA>::cross(const Vec3T<TA>& v) const
{
    return Vec3T(
        y * v.z - z * v.y,
        z * v.x - x * v.z,
        x * v.y - y * v.x);
}

Template
num
SK::Vec3T<TA>::length() const
{
    return std::sqrt(length2());
}

Template
num
SK::Vec3T<TA>::length2() const
{
    return x*x + y*y + z*z;
}

Template
void
SK::Vec3T<TA>::normalize()
{
    *this /= length();
}

Template
SK::Vec3T<TA>
SK::Vec3T<TA>::normal() const
{
    return *this / length(); 
}

Template
num&
SK::Vec3T<TA>::operator[](size_t idx)
{
    return (&x)[idx];
}

Template
num
SK::Vec3T<TA>::operator[](size_t idx) const
{
    return (&x)[idx];
}

Template
SK::Vec3T<TA>::operator const num*() const
{
    return &x;
}

Template
SK::Vec3T<TA>::operator num*()
{
    return &x;
}

#endif

typedef Vec3T<float, 16> Vec3F;
typedef Vec3T<double, 32> Vec3D;

}

#endif /* STONE_VEC3_HPP */
