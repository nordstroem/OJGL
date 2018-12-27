#pragma once

#include "utility/OJstd.h"

namespace ojgl {

class Window {
public:
    Window(unsigned width, unsigned height, bool fullScreen);
    Window(const Window& other) = delete;
    Window& operator=(const Window& other) = delete;
    ~Window();
    void getMessages();
    ojstd::vector<unsigned int> getPressedKeys();

public:
    static constexpr int KEY_LEFT = 37;
    static constexpr int KEY_UP = 38;
    static constexpr int KEY_RIGHT = 39;
    static constexpr int KEY_DOWN = 40;
    static constexpr int KEY_ESCAPE = 27;
    static constexpr int KEY_SPACE = 32;
    static constexpr int KEY_R = 82;
    static constexpr int KEY_F1 = 112;

private:
    class Details;
    ojstd::shared_ptr<Details> _priv; // @todo make ojstd::unique_ptr
};
} //namespace ojgl
