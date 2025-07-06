#pragma once
#include "render/Texture.h"
#include "utility/OJstd.h"

namespace ojgl {

class TextRenderer {
public:
    static TextRenderer& instance();
    void setHDC(void* hdc);
    ojstd::shared_ptr<Texture> get(const ojstd::string& textl, const ojstd::string& font = "Arial Black");

private:
    void* _hdcBackend;
};

}
