#pragma once

template <typename T>
struct Vector3 {
    T x = T(0);
    T y = T(0);
    T z = T(0);
};

template <typename T>
struct Vector2 {
    T x = T(0);
    T y = T(0);
};

using Vector2i = Vector3<int>;
using Vector3f = Vector3<float>;
