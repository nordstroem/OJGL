#pragma once

#include <stdlib.h>
#include <string.h>
#include <utility>

namespace ojstd {

template <typename T>
T max2(const T& a, const T& b)
{
    return a >= b ? a : b;
}

template <class InputIt, class T>
T accumulate(InputIt first, InputIt last, T init)
{
    for (; first != last; ++first) {
        init = std::move(init) + *first;
    }
    return init;
}

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

template <typename T>
class shared_ptr {
public:
    shared_ptr(T* ptr)
        : _ptr(ptr)
    {
        _count = new int;
        *_count = 1;
    }
    shared_ptr()
        : shared_ptr(nullptr)
    {
    }
    shared_ptr(const shared_ptr<T>& other)
    {
        this->_ptr = other.get();
        this->_count = other._count;
        (*this->_count)++;
    }
    ~shared_ptr()
    {
        (*_count)--;
        _ASSERTE(*_count >= 0);
        if (*_count == 0) {
            if (_count)
                delete _count;
            if (_ptr)
                delete _ptr;
        }
    }
    template <typename B>
    shared_ptr& operator=(const shared_ptr<B>& other)
    {
        this->_ptr = other.get();
        this->_count = other._count;
        (*this->_count)++;
        return *this;
    }
    T* operator->() const { return _ptr; }
    T& operator*() const noexcept { return *_ptr; }
    T* get() const { return _ptr; }
    bool operator!=(const shared_ptr<T>& other) const { return _ptr != other._ptr || _count != other._count; }
    bool operator==(const shared_ptr<T>& other) const { return _ptr == other._ptr && _count == other._count; }

    int* _count; // @todo make private and thread-safe.
private:
    T* _ptr = nullptr;
};

template <typename T, typename... Args>
shared_ptr<T> make_shared(Args... args)
{
    return shared_ptr<T>(new T(args...));
}

template <typename T>
class vector {
public:
    vector()
    {
        this->values = (T*)calloc(this->capacity, sizeof(T));
    }
    vector(const vector<T>& other)
    {
        this->values = (T*)calloc(other.size(), sizeof(T));
        this->length = other.size();
        this->capacity = other.size();
        for (int i = 0; i < other.size(); i++) {
            new (values + i) T(other[i]);
        }
    }
    vector(const std::initializer_list<T>& other)
    {
        this->values = (T*)calloc(other.size(), sizeof(T));
        this->length = other.size();
        this->capacity = other.size();
        int i = 0;
        for (auto& x : other) {
            new (values + i) T(x);
            i++;
        }
    }
    ~vector()
    {
        if (this->values) {
            for (int i = 0; i < this->length; i++)
                this->values[i].~T();
            free(this->values);
        }
    }

public:
    vector<T>& operator=(const vector<T>& other)
    {
        this->clear();
        if (this->values)
            free(this->values);
        this->values = (T*)calloc(other.size(), sizeof(T));
        this->length = other.size();
        this->capacity = other.size();
        for (int i = 0; i < other.size(); i++) {
            new (values + i) T(other[i]);
        }
        return *this;
    }
    T& operator[](int index) const { return values[index]; }

public:
    int size() const { return length; }
    T* begin() const { return values; }
    T* end() const { return values + length; }
    const T* cbegin() const { return values; }
    const T* cend() const { return values + length; }
    bool empty() const { return length == 0; }

    template <class... Args>
    void emplace_back(Args&&... args)
    {
        if (length == capacity)
            this->resize(max2(MIN_CAPACITY, this->capacity * 2));
        new (values + length) T(std::forward<Args>(args)...);
        length++;
    }
    void push_back(const T& val)
    {
        if (length == capacity)
            this->resize(max2(MIN_CAPACITY, this->capacity * 2));
        new (values + length) T(val);
        length++;
    }
    T* erase(T* it)
    {
        if (it >= this->begin() && it < this->end()) {
            it->~T();
            for (auto itt = it; itt < (this->end() - 1); itt++)
                *itt = *(itt + 1);
            this->length--;
            if ((it + 1) == this->end())
                return this->end();
            else
                return it;
        } else
            return this->end();
    }
    void clear()
    {
        for (int i = 0; i < this->length; i++)
            this->values[i].~T();
        length = 0;
    }
    void resize(int size)
    {
        T* newBuffer = (T*)calloc(size, sizeof(T));
        _ASSERTE(size >= this->length);
        memcpy(newBuffer, this->values, sizeof(T) * this->length);
        if (this->values)
            free(this->values);
        this->values = newBuffer;
        this->capacity = size;
    }

private:
    static int constexpr MIN_CAPACITY = 10;
    int length = 0;
    int capacity = MIN_CAPACITY;
    T* values;
};

template <typename First, typename Second>
struct Pair {
    Pair(const First& first, const Second& second)
        : first(first)
        , second(second)
    {
    }
    First first;
    Second second;
};

template <typename K, typename V>
class unordered_map {
public:
    unordered_map()
    {
    }

    V& operator[](const K& key)
    {
        auto it = find_if(this->keyValuePairs.begin(), this->keyValuePairs.end(), [&key](const auto& kvp) { return kvp.first == key; });
        if (it == this->keyValuePairs.end()) {
            this->keyValuePairs.emplace_back(key, V());
            return this->keyValuePairs[this->keyValuePairs.size() - 1].second;
        } else {
            return it->second;
        }
    }
    Pair<K, V>* find(const K& key)
    {
        return find_if(this->keyValuePairs.begin(), this->keyValuePairs.end(), [&key](const auto& kvp) { return kvp.first == key; });
    }
    Pair<K, V>* begin() { return keyValuePairs.begin(); }
    Pair<K, V>* end() { return keyValuePairs.end(); }

private:
    vector<Pair<K, V>> keyValuePairs;
};

template <typename T>
class unordered_set {
public:
    void insert(const T& val)
    {
        auto it = find_if(this->begin(), this->end(), [&val](const auto& v) { return v == val; });
        if (it == this->end())
            this->values.push_back(val);
    }
    T* find(const T& val)
    {
        return find_if(this->begin(), this->end(), [&val](const auto& v) { return v == val; });
    }
    size_t erase(const T& val)
    {
        int nprev = this->values.size();
        auto it = find_if(this->begin(), this->end(), [&val](const auto& v) { return v == val; });
        this->erase(it);
        int npost = this->values.size();
        return nprev - npost;
    }
    T* begin() { return values.begin(); }
    T* end() { return values.end(); }
    const T* cbegin() const { return values.cbegin(); }
    const T* cend() const { return values.cend(); }
    bool empty() { return this->values.size() == 0; }
    T* erase(T* it) { return this->values.erase(it); }

private:
    vector<T> values;
};

class string {
public:
    string(const char* str)
        : len(strlen(str))
    {
        content = (char*)malloc(sizeof(char) * (len + 1));
        strcpy(content, str);
    }
    string(const string& str)
        : len(str.length())
    {
        content = (char*)malloc(sizeof(char) * (len + 1));
        strcpy(content, str.c_str());
    }
    string()
        : string("")
    {
    }
    ~string()
    {
        if (this->content != nullptr)
            free(content);
    }
    bool operator==(const string& other) const
    {
        return strcmp(this->content, other.content) == 0;
    }
    string& operator=(const string& other)
    {
        if (this->content != nullptr) {
            free(this->content);
        }
        this->content = (char*)malloc(sizeof(char) * (other.length() + 1));
        strcpy(content, other.c_str());
        this->len = other.length();
        return *this;
    }

    string operator+(const string& other)
    {
        char* str = (char*)malloc(sizeof(char) * (len + other.len + 1));
        strcpy(str, this->content);
        strcpy(str + len, other.content);
        string flString = string(str);
        free(str);
        return flString;
    }

    const char* c_str() const
    {
        return this->content;
    }

    int length() const
    {
        return this->len;
    }

    void append(const string& other)
    {
        char* tmp = content;
        content = (char*)malloc(sizeof(char) * (len + other.len + 1));
        strcpy(content, tmp);
        strcpy(content + len, other.content);
        this->len = this->len + other.len;
        free(tmp);
    }

private:
    char* content = nullptr;
    int len = 0;
};

inline string to_string(size_t i)
{
    _ASSERTE(i < 10);
    char c[2] = { '0' + static_cast<char>(i), '\0' };
    return string(c);
}

// @todo move implementation of this to cpp file and use windows.h mutex.
class mutex {
public:
    void lock()
    {
    }

    void unlock()
    {
    }
};

template <typename T>
class lock_guard {
public:
    lock_guard(T& v)
        : _v(v)
    {
        _v.lock();
    }
    ~lock_guard()
    {
        _v.unlock();
    }

private:
    T& _v;
};

} //end namespace fl
