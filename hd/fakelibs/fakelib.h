#pragma once

namespace fl {

template <typename T>
class shared_ptr {
public:
    shared_ptr(T* ptr)
        : _ptr(ptr)
    {
    }

    const T* operator->() const
    {
        return _ptr;
    }

    T* get() const
    {
        return _ptr;
    }

private:
    T* _ptr;
};

template <typename T>
shared_ptr<T> make_shared(T value)
{
    return shared_ptr<T>(new T(value));
}

template <typename K, typename V>
class unordered_map {
public:
    unordered_map()
    {
    }

    V operator[](K key)
    {
        return values[0]; // TODO
    }

    int find(K key)
    {
        return 0; // TODO
    }

private:
    V* values;
};

template <typename T>
class vector {
public:
    vector()
    {
    }

    int size() const
    {
        return 0;
    }

    T& operator[](int index) const
    {
        return values[index]; // TODO
    }

    int begin()
    {
        return 0; // TODO
    }

    int end()
    {
        return 0; // TODO
    }

    int cbegin() const
    {
        return 0; // TODO
    }

    int cend() const
    {
        return 0; // TODO
    }

private:
    T* values;
};

class string {
public:
    const char* c_str() const
    {
        return this->content;
    }

    int length() const
    {
        return this->len;
    }

private:
    const char* content;
    int len;
};
}
