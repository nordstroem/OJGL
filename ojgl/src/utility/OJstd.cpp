#include "Macros.h"
#include "OJstd.h"
#include "windows.h"
#include <stdlib.h>
#include <string.h>
#include <utility>

namespace ojstd {

static constexpr float pow(float x, int h)
{
    float result = 1.0;
    for (int i = 0; i < h; i++) {
        result *= x;
    }
    return result;
}

double inline __declspec(naked) __fastcall sin_asm(double n)
{
    _asm {
		fld qword ptr[esp + 4]
		fsin
		ret 8
    }
}

float sin(float angle)
{
    return sin_asm(angle);
    /*int k = ftoi(angle / pi);

    if ((k > 0 ? k : -k) % 2 == 1)
        k = k + (k > 0 ? 1 : -1);

    float sAngle = angle - k * pi;
    return sAngle - pow(sAngle, 3) / 6.f + pow(sAngle, 5) / 120.f - pow(sAngle, 7) / 5040.f + pow(sAngle, 9) / 362880.f;*/
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
    : len(strlen(str))
{
    content = (char*)malloc(sizeof(char) * (len + 1));
    strcpy(content, str);
}

string::string(const string& str)
    : len(str.length())
{
    content = (char*)malloc(sizeof(char) * (len + 1));
    strcpy(content, str.c_str());
}

string::string(string&& str)
{
    this->content = str.content;
    this->len = str.len;
    str.content = nullptr;
    str.len = 0;
}

string::string(char*&& str)
    : content(str)
    , len(strlen(str))
{
}

string::string()
    : string("")
{
}

string::~string()
{
    if (this->content != nullptr)
        free(content);
}

bool string::operator==(const string& other) const
{
    return strcmp(this->content, other.content) == 0;
}

string& string::operator=(const string& other)
{
    if (this->content != nullptr) {
        free(this->content);
    }
    this->content = (char*)malloc(sizeof(char) * (other.length() + 1));
    strcpy(content, other.c_str());
    this->len = other.length();
    return *this;
}

string string::operator+(const string& other)
{
    char* str = (char*)malloc(sizeof(char) * (len + other.len + 1));
    strcpy(str, this->content);
    strcpy(str + len, other.content);
    return string(std::move(str));
}

char string::operator[](size_t index) const
{
    _ASSERTE(index < this->len);
    return this->content[index];
}

const char* string::c_str() const
{
    return this->content;
}

int string::length() const
{
    return this->len;
}

string string::substring(int start, int end) const
{
    _ASSERTE(end >= start);
    _ASSERTE(start > 0);
    _ASSERTE(end <= this->len);
    int length = end - start;
    char* content = (char*)malloc(sizeof(char) * (length + 1));
    memcpy(content, this->content + start, sizeof(char) * length);
    content[length] = '\0';
    return string(std::move(content));
}

void string::append(const string& other)
{
    char* tmp = content;
    content = (char*)malloc(sizeof(char) * (len + other.len + 1));
    strcpy(content, tmp);
    strcpy(content + len, other.content);
    this->len = this->len + other.len;
    free(tmp);
}

int string::find(const string& str, int startPos) const
{
    for (; startPos < (this->len - str.length() + 1); startPos++) {
        bool foundStr = true;
        for (int i = 0; i < str.length(); i++) {
            if (this->content[startPos + i] != str[i]) {
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
        int newLength = this->len + newStr.length() - oldStr.length();
        char* newContent = (char*)malloc(sizeof(char) * (newLength + 1));

        for (int i = 0; i < newLength; i++) {
            if (i >= startPos) {
                if (i < (startPos + newStr.length()))
                    newContent[i] = newStr[i - startPos];
                else
                    newContent[i] = this->content[i - newStr.length() + oldStr.length()];
            } else {
                newContent[i] = this->content[i];
            }
        }
        newContent[newLength] = '\0';
        return string(std::move(newContent));
    } else {
        return *this;
    }
}

string to_string(size_t i)
{
    _ASSERTE(i >= 0);
    int len = 1;
    string s;
    do {
        char c[2] = { '0' + static_cast<char>(i % 10), '\0' };
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
    : _priv(make_shared<details>())
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
    auto release_result = ReleaseMutex(_priv->w_mutex);
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
sign:
        mov  eax,value;
        and  eax,80000000h;
        cmp  eax,80000000h;
        je     putsign;
    }

    return i;

putsign:
    return -i;
}
}
