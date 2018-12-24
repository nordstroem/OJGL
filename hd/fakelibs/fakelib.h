#pragma once

#include <stdlib.h>
#include <string.h>
#include <utility>

namespace fl {

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
    }

    template <typename B>
    shared_ptr& operator=(const shared_ptr<B>& other)
    {
        _ptr = other._ptr;
        return *this;
    }

    shared_ptr()
    {
        this->_ptr = nullptr;
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
        return _ptr != other._ptr;
    }

    bool operator==(shared_ptr<T> other) const
    {
        return _ptr == other._ptr;
    }

    //private:
    T* _ptr;
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
        this->values = (T*)calloc(MAX_SIZE, sizeof(T));
    }
    ~vector()
    {
        free(this->values);
    }
    vector(const vector<T>& other)
    {
        this->values = (T*)calloc(MAX_SIZE, sizeof(T));
        this->length = other.size();
        for (int i = 0; i < other.size(); i++) {
            this->values[i] = other[i];
        }
    }

    vector(const std::initializer_list<T>& other)
    {
        this->values = (T*)calloc(MAX_SIZE, sizeof(T));
        this->length = other.size();
        int i = 0;
        for (auto& x : other) {
            this->values[i] = x;
            i++;
        }
    }

    template <class... Args>
    void emplace_back(Args&&... args)
    {
        new (values + length) T(std::forward<Args>(args)...);
        length++;
    }
    void push_back(const T& val)
    {
        new (values + length) T(val);
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

    T* begin() const
    {
        return values;
    }

    T* end() const
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

    T* erase(T* it)
    {
        if (it >= this->begin() && it < this->end()) {
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

    bool empty() const
    {
        return length == 0;
    }

    void clear()
    {
        length = 0;
    }

    vector<T>& operator=(const vector<T>& other)
    {
        if (this->values != nullptr) {
            free(this->values);
        }
        this->values = (T*)calloc(MAX_SIZE, sizeof(T));
        this->length = other.size();
        for (int i = 0; i < other.size(); i++) {
            this->values[i] = other[i];
        }
        return *this;
    }

private:
    static constexpr int MAX_SIZE = 5000;
    int length = 0;
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

    Pair<K, V>* begin()
    {
        return keyValuePairs.begin();
    }

    Pair<K, V>* end()
    {
        return keyValuePairs.end();
    }

private:
    vector<Pair<K, V>> keyValuePairs;
};

template <typename T>
class unordered_set {
public:
    T* begin()
    {
        return values.begin();
    }

    T* end()
    {
        return values.end();
    }

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

    T* erase(T* it)
    {
        return this->values.erase(it);
    }

    size_t erase(const T& val)
    {
        int nprev = this->values.size();
        auto it = find_if(this->begin(), this->end(), [&val](const auto& v) { return v == val; });
        this->erase(it);
        int npost = this->values.size();
        return nprev - npost;
    }

    const T* cbegin() const
    {
        return values.cbegin();
    }

    const T* cend() const
    {
        return values.cend();
    }

    bool empty()
    {
        return this->values.size() == 0;
    }

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
    const char* c_str() const
    {
        return this->content;
    }

    int length() const
    {
        return this->len;
    }

    void append(const fl::string& other)
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
    char c[2] = { '0' + i, '\0' };
    return string(c);
}

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
