#pragma once

#include "utility/OJstd.h"
#include <type_traits>

template <typename T>
struct Vector3 {
    T x = T(0);
    T y = T(0);
    T z = T(0);

    Vector3() = default;

    Vector3(const T& x, const T& y, const T& z)
        : x(x)
        , y(y)
        , z(z)
    {
    }

    Vector3 operator*(const T& a) const
    {
        return Vector3(x * a, y * a, z * a);
    }

    void operator+=(const Vector3& v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
    }

    Vector3 operator+(const Vector3& v) const
    {
        return Vector3(x + v.x, y + v.y, z + v.z);
    }

    Vector3 operator-(const Vector3& v) const
    {
        return Vector3(x - v.x, y - v.y, z - v.z);
    }

    void normalize()
    {
        T len = static_cast<T>(ojstd::sqrt(static_cast<float>(x * x + y * y + z * z)));
        x /= len;
        y /= len;
        z /= len;
    }

    Vector3 normalized() const
    {
        T len = static_cast<T>(ojstd::sqrt(static_cast<float>(x * x + y * y + z * z)));
        return Vector3(x / len, y / len, z / len);
    }

    [[nodiscard]] T lenSq()
    {
        return x * x + y * y + z * z;
    }
};

template <typename T>
struct Vector2 {
    T x = T(0);
    T y = T(0);

    Vector2() = default;
    Vector2(const T& x, const T& y)
        : x(x)
        , y(y)
    {
    }

    Vector2 operator-(const Vector2& v) const
    {
        return Vector2(x - v.x, y - v.y);
    }

    Vector2 operator/(const T& s) const
    {
        return Vector2(x / s, y / s);
    }

    [[nodiscard]] std::enable_if_t<std::is_integral_v<T>, Vector2> cropToAspectRatio(float aspectRatio) const
    {
        const float currentAspectRatio = static_cast<float>(x) / y;
        if (aspectRatio > currentAspectRatio)
            return Vector2(x, ojstd::ftoi(x / aspectRatio));
        else
            return Vector2(ojstd::ftoi(y * aspectRatio), y);
    }
};

using Vector2i = Vector2<int>;
using Vector2d = Vector2<double>;
using Vector3f = Vector3<float>;
