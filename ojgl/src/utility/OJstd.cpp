#include "OJstd.h"
#include "Macros.h"
#include "windows.h"
#include <stdlib.h>
#include <string.h>
#include <utility>

namespace ojstd {

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
    string flString = string(str);
    free(str);
    return flString;
}

const char* string::c_str() const
{
    return this->content;
}

int string::length() const
{
    return this->len;
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

string to_string(size_t i)
{
    _ASSERTE(i < 10);
    char c[2] = { '0' + static_cast<char>(i), '\0' };
    return string(c);
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
}
