#pragma once

#include <stdlib.h>
#include <string.h>

namespace fl {

template <typename T>
class shared_ptr {
public:
    shared_ptr(T* ptr)
        : _ptr(ptr)
    {
    }

    T* operator->() const
    {
        return _ptr;
    }

    T& operator*() const noexcept
    {
        return *_ptr;
    }

    T* get() const
    {
        return _ptr;
    }

    bool operator!=(shared_ptr<T> other) const
    {
        return _ptr != this->_ptr;
    }

private:
    T* _ptr;
};

template <typename T>
shared_ptr<T> make_shared(T value)
{
    return shared_ptr<T>(new T(value));
}

template <typename T>
class vector {
public:
    vector()
    {
        this->values = (T*)malloc(sizeof(T) * MAX_SIZE);
    }
    ~vector()
    {
        free(this->values);
    }
    void push_back(const T& val)
    {
        values[length] = val;
        length++;
    }
    int size() const
    {
        return length;
    }
    T& operator[](int index) const
    {
        return values[index];
    }
    T* begin()
    {
        return values;
    }

    T* end()
    {
        return values + length;
    }

    const T* cbegin() const
    {
        return values;
    }

    const T* cend() const
    {
        return values + length;
    }

    void insert(T val)
    {
    }

    T* find(T val)
    {
        return nullptr;
    }

    T* erase(T* it)
    {
        return nullptr;
    }

    size_t erase(T val)
    {
        return 0;
    }

    bool empty()
    {
        return length == 0;
    }

    void clear()
    {
        length = 0;
    }

private:
    const int MAX_SIZE = 100;
    int length = 0;
    T* values;
};

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

//template <typename T>
//class unordered_set {
//public:
//    T* begin()
//    {
//        return v.begin();
//    }
//
//    T* end()
//    {
//        return values + length;
//    }
//
//    void insert(T val)
//    {
//    }
//
//    T* find(T val)
//    {
//        return nullptr;
//    }
//
//    T* erase(T* it)
//    {
//        return nullptr;
//    }
//
//    size_t erase(T val)
//    {
//        return 0;
//    }
//
//    const T* cbegin() const
//    {
//        return values; // TODO
//    }
//
//    const T* cend() const
//    {
//        return values + length; // TODO
//    }
//
//    bool empty()
//    {
//        return true;
//    }
//
//private:
//    vector v;
//};

class string {
public:
    string(const char* str)
        : content(str)
        , len(strlen(str))
    {
    }

    string()
        : content("")
        , len(0)
    {
    }

    bool operator==(const string& other) const
    {
        return strcmp(this->content, other.content) == 0;
    }

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

template <class ForwardIt, class UnaryPredicate>
ForwardIt find_if(ForwardIt first, ForwardIt last, UnaryPredicate p)
{
    for (auto it = first; it != last; it++) {
        if (p(*it)) {
            return it;
        }
    }
    return last;
}
} //end namespace fl
