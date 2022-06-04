#include "OJstd.h"
#include "Macros.h"
#include "windows.h"
#include <stdlib.h>
#include <string.h>
#include <utility>

#pragma warning(disable : 6011 6387)

namespace ojstd {

float pow(float x, int h)
{
    float result = 1.0;
    for (int i = 0; i < h; i++) {
        result *= x;
    }
    return result;
}

double inline __declspec(naked) __fastcall sin_asm(double)
{
    _asm {
		fld qword ptr[esp + 4]
		fsin
		ret 8
    }
}

double inline __declspec(naked) __fastcall atan_asm(double)
{
    _asm {
		push ebp
        mov ebp, esp
        fld qword ptr[ebp+8]
        fld1
        fpatan
        pop ebp
		ret
    }
}

float atan2(float y, float x)
{
    if (x > 0) {
        return static_cast<float>(atan_asm(y / x));
    } else if (x < 0 && y >= 0) {
        return static_cast<float>(atan_asm(y / x)) + ojstd::pi;
    } else if (x < 0 && y < 0) {
        return static_cast<float>(atan_asm(y / x)) - ojstd::pi;
    } else if (x == 0 && y > 0) {
        return ojstd::pi / 2;
    } else if (x == 0 && y < 0) {
        return -ojstd::pi / 2;
    } else {
        return 0.f; // undefined
    }
}

float acos(float a)
{
    double ap = a / ojstd::sqrt(1 - a * a);
    return static_cast<float>(atan_asm(ap));
}

Pair<float, float> modf(float value)
{
    float base = static_cast<float>(ftoi(value));
    float fraction = value - base;

    return { fraction < 0 ? 1.f - fraction : fraction, base };
}

float sin(float angle)
{
    return static_cast<float>(sin_asm(angle));
}

float cos(float angle)
{
    return sin(angle + pi / 2);
}

float tan(float angle)
{
    return sin(angle) / cos(angle);
}

string::string(const char* str)
    : _len(strlen(str))
{
    _content = (char*)malloc(sizeof(char) * (_len + 1));
    _ASSERTE(_content != nullptr);
    strcpy(_content, str);
}

string::string(const string& str)
    : _len(str.length())
{
    _content = (char*)malloc(sizeof(char) * (_len + 1));
    _ASSERTE(_content != nullptr);
    strcpy(_content, str.c_str());
}

string::string(string&& str) noexcept
{
    _content = str._content;
    _len = str._len;
    str._content = nullptr;
    str._len = 0;
}

string::string(char*&& str)
    : _content(str)
    , _len(strlen(str))
{
}

string::string()
    : string("")
{
}

string::~string()
{
    if (this->_content != nullptr)
        free(_content);
}

bool string::operator==(const string& other) const
{
    return strcmp(this->_content, other._content) == 0;
}

bool string::operator!=(const string& other) const
{
    return !(*this == other);
}

string& string::operator=(const string& other)
{
    if (this->_content != nullptr) {
        free(this->_content);
    }
    this->_content = (char*)malloc(sizeof(char) * (other.length() + 1));
    _ASSERTE(_content != nullptr);
    strcpy(_content, other.c_str());
    _len = other.length();
    return *this;
}

string operator+(const string& first, const string& second)
{
    char* str = (char*)malloc(sizeof(char) * (first._len + second._len + 1));
    _ASSERTE(str != nullptr);
    strcpy(str, first._content);
    strcpy(str + first._len, second._content);
    return string(std::move(str));
}

char string::operator[](size_t index) const
{
    _ASSERTE(static_cast<int>(index) < _len);
    return _content[index];
}

const char* string::c_str() const
{
    return _content;
}

int string::length() const
{
    return _len;
}

string string::substring(int start, int end) const
{
    _ASSERTE(end >= start);
    _ASSERTE(start > 0);
    _ASSERTE(end <= _len);
    int length = end - start;
    char* content = (char*)malloc(sizeof(char) * (length + 1));
    _ASSERTE(_content != nullptr);
    memcpy(content, _content + start, sizeof(char) * length);
    content[length] = '\0';
    return string(std::move(content));
}

void string::append(const string& other)
{
    char* tmp = _content;
    _content = (char*)malloc(sizeof(char) * (_len + other._len + 1));
    _ASSERTE(_content != nullptr);
    strcpy(_content, tmp);
    strcpy(_content + _len, other._content);
    _len = _len + other._len;
    free(tmp);
}

int string::find(const string& str, int startPos) const
{
    for (; startPos < (_len - str.length() + 1); startPos++) {
        bool foundStr = true;
        for (int i = 0; i < str.length(); i++) {
            if (_content[startPos + i] != str[i]) {
                foundStr = false;
                break;
            }
        }
        if (foundStr)
            return startPos;
    }
    return -1;
}

string string::replaceFirst(const string& oldStr, const string& newStr) const
{
    _ASSERTE(oldStr.length() > 0);

    int startPos = this->find(oldStr);

    if (startPos != -1) {
        int newLength = _len + newStr.length() - oldStr.length();
        _ASSERTE(newLength >= 0);
        char* newContent = (char*)malloc(sizeof(char) * (newLength + 1));
        _ASSERTE(newContent != nullptr);

        for (int i = 0; i < newLength; i++) {
            if (i >= startPos) {
                if (i < (startPos + newStr.length()))
                    newContent[i] = newStr[i - startPos];
                else
                    newContent[i] = _content[i - newStr.length() + oldStr.length()];
            } else {
                newContent[i] = _content[i];
            }
        }
        newContent[newLength] = '\0';
        return string(std::move(newContent));
    } else {
        return *this;
    }
}

string to_string(int i)
{
    _ASSERTE(i >= 0);
    string s;
    do {
        const char c[2] = { '0' + static_cast<char>(i % 10), '\0' };
        s = string(c) + s;
        i /= 10;
    } while (i != 0);

    return s;
}

class mutex::details {
public:
    HANDLE w_mutex;
};

mutex::mutex()
    : _priv(make_unique<details>())
{
    _priv->w_mutex = CreateMutex(NULL, FALSE, NULL);
    _ASSERTE(_priv->w_mutex != NULL);
}

mutex::~mutex()
{
    CloseHandle(_priv->w_mutex);
}

void mutex::lock()
{
    auto wait_result = WaitForSingleObject(_priv->w_mutex, INFINITE);
    // @todo some check for what wait_result is?
    OJ_UNUSED(wait_result);
}

void mutex::unlock()
{
    [[maybe_unused]] auto release_result = ReleaseMutex(_priv->w_mutex);
    _ASSERTE(release_result);
}

wstringWrapper::wstringWrapper(const string& str)
{
    _ASSERTE(str.length() < wstringWrapper::maxStringLength);
    int num = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, ptr, num);
}

void sleep(int milliseconds)
{
    Sleep(milliseconds);
}

#pragma warning(disable : 4740)
int ftoi(float value)
{
    int i;
    _asm {
        mov  eax,value; //loaded mem to acc
        rcl  eax,1; //left shift acc to remove the sign
        mov  ebx,eax; //save the acc
        mov  edx,4278190080; //clear reg edx;
        and  eax,edx; //and acc to retrieve the exponent
        shr  eax,24;
        sub  eax,7fh; //subtract 7fh(127) to get the actual power
        mov  edx,eax; //save acc val power
        mov  eax,ebx; //retrieve from ebx
        rcl  eax,8; //trim the left 8 bits that contain the power
        mov  ebx,eax; //store
        mov  ecx, 1fh; //subtract 17 h
        sub  ecx,edx;
        mov  edx,00000000h;
        cmp  ecx,0;
        je   loop2;
        shr  eax,1;
        or   eax,80000000h;
loop1:
        shr  eax,1; //shift (total bits - power bits);
        sub  ecx,1;
        add  edx,1;
        cmp  ecx,0;
        ja   loop1;
loop2:
        mov  i, eax;

        //check sign +/-
        mov  eax,value;
        and  eax,80000000h;
        cmp  eax,80000000h;
        je     putsign;
    }

    return i;

putsign:
    return -i;
}

// https://www.gamedev.net/forums/topic/671079-fast-sqrt-for-64bit/
double __declspec(naked) __fastcall sqrt_asm(double)
{
    _asm {
    fld qword ptr[esp + 4]
    fsqrt
    ret 8
    }
}

float sqrt(float n)
{
    return static_cast<float>(sqrt_asm(static_cast<double>(n)));
}

float abs(float value)
{
    return value < 0 ? -value : value;
}

float floor(float value)
{
    const int res = value >= 0 ? ftoi(value) : ftoi(value) - 1;
    return static_cast<float>(res);
}

float fract(float value)
{
    return value - floor(value);
}

float hash1(float value)
{
    return fract(sin(value * 727.1f) * 435.545f);
}

static float randState = 0.f;
float rand()
{
    randState += 0.01f;
    return hash1(randState);
}

float clamp(float x, float lower, float upper)
{
    if (x < lower)
        x = lower;
    if (x > upper)
        x = upper;
    return x;
}

float smoothstep(float edge0, float edge1, float x)
{
    x = clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
    return x * x * (3 - 2 * x);
}

float lerp(float left, float right, float amount)
{
    return (1 - amount) * left + amount * right;
}

}
