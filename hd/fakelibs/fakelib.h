#pragma once

#include <stdlib.h>
#include <string.h>
#include <utility>

namespace fl {

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
        return _ptr != this->_ptr;
    }

    bool operator==(shared_ptr<T> other) const
    {
        return _ptr == this->_ptr;
    }

    //private:
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

    template <class... Args>
    void emplace_back(Args&&... args)
    {
        values[length] = T(std::forward<Args>(args)...);
        length++;
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
        content = (char*)malloc(sizeof(char) * len);
        strcpy(content, str);
    }

    string()
        : content(nullptr)
        , len(0)
    {
    }
    ~string()
    {
        //free(content);
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

    //void append(const fl::string& other)
    //{
    //    char* tmp = content;
    //    content = (char*)malloc(sizeof(char) * (len + other.len));
    //    strcpy(content, tmp);
    //    strcpy(content + len, other.content);
    //    this->len = this->len + other.len;
    //    //free(tmp);
    //}

private:
    char* content;
    int len;
};

//string to_string(size_t i)
//{
//    char c = '0' + i;
//    return string(&c);
//}

} //end namespace fl
