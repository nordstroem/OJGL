#pragma once
#include <cstring>
#include <utility>

namespace ojstd {

constexpr float pi = 3.14159265f;

float sin(float angle);
float cos(float angle);
float tan(float angle);
int ftoi(float value);

template <typename T>
T max(const T& a, const T& b)
{
    return a >= b ? a : b;
}

template <typename T>
T min(const T& a, const T& b)
{
    return a <= b ? a : b;
}

template <typename T>
T abs(const T& a)
{
    return a < 0 ? -a : a;
}

template <class InputIt, class T>
T accumulate(InputIt first, InputIt last, T init)
{
    for (; first != last; ++first) {
        init = std::move(init) + *first;
    }
    return init;
}

template <typename T, typename K = int>
K sign(const T& a)
{
    if (a == T(0))
        return K(0);
    return a < T(0) ? K(-1) : K(1);
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

template <class ForwardIt, class Value>
ForwardIt find(ForwardIt first, ForwardIt last, const Value& val)
{
    for (auto it = first; it != last; it++) {
        if (*it == val) {
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
        try_free();
    }
    shared_ptr& operator=(const shared_ptr& other)
    {
        if (*this != other) {
            (*_count)--;
            try_free();
            this->_ptr = other.get();
            this->_count = other._count;
            (*this->_count)++;
        }
        return *this;
    }
    template <typename B>
    shared_ptr<T>& operator=(const shared_ptr<B>& other)
    {
        if (*this != other) {
            (*_count)--;
            try_free();
            this->_ptr = other.get();
            this->_count = other._count;
            (*this->_count)++;
        }
        return *this;
    }

    template <typename B, typename = std::enable_if_t<std::is_base_of<B, T>::value>>
    operator shared_ptr<B>()
    {
      return *reinterpret_cast<shared_ptr<B>*>(this);
    }

    T* operator->() const { return _ptr; }
    T& operator*() const noexcept { return *_ptr; }
    T* get() const { return _ptr; }
    bool operator==(const shared_ptr<T>& other) const { return _ptr == other._ptr && _count == other._count; }
    bool operator!=(const shared_ptr<T>& other) const { return !(*this == other); }

    template <typename B>
    bool operator==(const shared_ptr<B>& other) const { return _ptr == other._ptr && _count == other._count; }
    template <typename B>
    bool operator!=(const shared_ptr<B>& other) const { return !(*this == other); }

    bool operator==(std::nullptr_t other) const { return _ptr == other; }
    bool operator!=(std::nullptr_t other) const { return _ptr != other; }

    template <typename B, typename = std::enable_if_t<std::is_base_of<B, T>::value>>
    operator shared_ptr<B>()
    {
        return *reinterpret_cast<shared_ptr<B>*>(this);
    }

    int* _count = nullptr; // @todo make private and thread-safe.
    T* _ptr = nullptr;

private:
    void try_free()
    {
        _ASSERTE(*_count >= 0);
        if (*_count == 0) {
            if (_count)
                delete _count;
            if (_ptr)
                delete _ptr;
        }
    }
};

template <typename T, typename... Args>
shared_ptr<T> make_shared(Args... args)
{
    return shared_ptr<T>(new T(std::move(args)...));
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
        this->capacity = max(MIN_CAPACITY, other.size());
        this->values = (T*)calloc(this->capacity, sizeof(T));
        this->length = other.size();
        for (int i = 0; i < other.size(); i++) {
            new (values + i) T(other[i]);
        }
    }
    vector(const std::initializer_list<T>& other)
    {
        this->capacity = max(MIN_CAPACITY, static_cast<int>(other.size()));
        this->values = (T*)calloc(this->capacity, sizeof(T));
        this->length = other.size();
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
        this->capacity = max(MIN_CAPACITY, other.size());
        this->values = (T*)calloc(this->capacity, sizeof(T));
        this->length = other.size();
        for (int i = 0; i < other.size(); i++) {
            new (values + i) T(other[i]);
        }
        return *this;
    }
    T& operator[](int index) const
    {
        _ASSERTE(index >= 0);
        _ASSERTE(index < length);
        return values[index];
    }

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
            this->resize(max(MIN_CAPACITY, this->capacity * 2));
        new (values + length) T(std::forward<Args>(args)...);
        length++;
    }
    void push_back(const T& val)
    {
        if (length == capacity)
            this->resize(max(MIN_CAPACITY, this->capacity * 2));
        new (values + length) T(val);
        length++;
    }
    T* erase(T* it)
    {
        if (it >= this->begin() && it < this->end()) {
            it->~T();
            memmove(it, it + 1, sizeof(T) * (this->end() - it - 1));
            this->length--;
            if ((it + 1) == this->end())
                return this->end();
            else
                return it;
        } else
            return this->end();
    }
    void erase(const T& val)
    {
        auto* it = find(this->begin(), this->end(), val);
        if (it != this->end())
            this->erase(it);
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
    bool operator==(const Pair<First, Second>& other) const { return first == other.first && second == other.second; }

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
    bool contains(const K& key)
    {
        return this->find(key) != this->end();
    }

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
    bool contains(const T& value)
    {
        return this->find(value) != this->end();
    }

private:
    vector<T> values;
};

class string {
public:
    string(const char* str);
    string(const string& str);
    explicit string(char*&& str);
    explicit string(string&& str) noexcept;

    string();
    ~string();
    bool operator==(const string& other) const;
    string& operator=(const string& other);
    friend string operator+(const string& first, const string& second);

    char operator[](size_t index) const;
    const char* c_str() const;
    int length() const;
    void append(const string& other);
    // Returns the position of the first occurence of str in this string after startPos. -1 if not found.
    int find(const string& str, int startPos = 0) const;
    // Returns a new string with the first occurence of the substring oldStr replaced with newStr.
    string replaceFirst(const string& oldStr, const string& newStr) const;
    string substring(int start, int end) const;

private:
    char* _content = nullptr;
    int _len = 0;
};

string to_string(int i);

class mutex {
public:
    mutex();
    mutex(const mutex& other) = delete;
    mutex& operator=(const mutex& other) = delete;
    ~mutex();
    void lock();
    void unlock();

private:
    class details;
    shared_ptr<details> _priv; // @todo make ojstd::unique_ptr.
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

class wstringWrapper {
    static constexpr size_t maxStringLength = 100;

public:
    wstringWrapper(const string& str);
    wstringWrapper(const wstringWrapper& other) = delete;
    wstringWrapper& operator=(const wstringWrapper other) = delete;

public:
    wchar_t ptr[maxStringLength];
};

void sleep(int milliseconds);

float sqrt(float n);

// https://www.gamedev.net/forums/topic/671079-fast-sqrt-for-64bit/
double __fastcall sqrt_asm(double);

Pair<float, float> modf(float value);

float abs(float value);
float pow(float x, int h);
float floor(float value);
float fract(float value);
float hash1(float value);
float rand();
float clamp(float x, float lower, float upper);
float smoothstep(float edge0, float edge1, float x);
float lerp(float left, float right, float amount);

template <typename ReturnType, typename... Args>
class callable_base {
public:
    virtual ~callable_base() = default;
    virtual ReturnType invoke(Args... args) = 0;
};

template <typename F, typename ReturnType, typename... Args>
class callable : public callable_base<ReturnType, Args...> {
public:
    callable(F&& f)
        : _function(std::forward<F>(f)) {};
    ReturnType invoke(Args... args) override
    {
        return _function(std::forward<Args>(args)...);
    }

private:
    F _function;
};

template <typename T>
class function;

template <typename ReturnType, typename... Args>
class function<ReturnType(Args...)> {
public:
    function() = default;
    template <typename F>
    function(F&& f)
        : _callable_ptr(make_callable_ptr(std::forward<F>(f)))
    {
        static_assert(!std::is_same_v<std::remove_reference_t<F>, function>);
    }
    template <typename F>
    function& operator=(F&& f)
    {
        static_assert(!std::is_same_v<std::remove_reference_t<F>, function>);
        _callable_ptr = make_callable_ptr(std::forward<F>(f));
        return *this;
    }
    auto operator()(Args... args)
    {
        return _callable_ptr->invoke(std::forward<Args>(args)...);
    }
    operator bool()
    {
        return _callable_ptr != nullptr;
    }

private:
    template <class T>
    auto static make_callable_ptr(T&& t)
    {
        return ojstd::make_shared<callable<T, ReturnType, Args...>>(std::forward<T>(t));
    }

private:
    ojstd::shared_ptr<callable_base<ReturnType, Args...>> _callable_ptr;
};

} //end namespace ojstd
