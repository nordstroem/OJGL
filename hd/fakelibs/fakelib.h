#pragma once

namespace fl {

template <typename T>
class shared_ptr {
public:
    shared_ptr(T* ptr)
        : _ptr(ptr)
    {
    }

private:
    const T* _ptr;
};

template <typename K, typename V>
class unordered_map {
public:
    unordered_map()
    {
    }

private:
};
}
