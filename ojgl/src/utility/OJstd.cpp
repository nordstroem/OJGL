#include "OJstd.h"
#include "Macros.h"
#include "windows.h"

namespace ojstd {

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
