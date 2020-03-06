#pragma once

#include "utility/OJstd.h"

namespace ojgl {

class Window {
public:
    Window(unsigned width, unsigned height, ojstd::string title, bool fullScreen, bool showCursor);
    Window(const Window& other) = delete;
    Window& operator=(const Window& other) = delete;
    ~Window();
    void getMessages();
    ojstd::vector<unsigned int> getPressedKeys();
    ojstd::vector<unsigned int> getDownKeys() const;
    bool isClosePressed() const;
    bool isLeftMouseButtonDown() const;
    ojstd::Pair<int, int> getCursorPosition() const;

public:
    static constexpr int KEY_LEFT = 37;
    static constexpr int KEY_UP = 38;
    static constexpr int KEY_RIGHT = 39;
    static constexpr int KEY_DOWN = 40;
    static constexpr int KEY_ESCAPE = 27;
    static constexpr int KEY_SPACE = 32;
    static constexpr int KEY_R = 82;
    static constexpr int KEY_F1 = 112;
    static constexpr int KEY_W = 0x57;
    static constexpr int KEY_S = 0x53;
    static constexpr int KEY_D = 0x44;
    static constexpr int KEY_A = 0x42;

private:
    class Details;
    ojstd::shared_ptr<Details> _priv; // @todo make ojstd::unique_ptr
};
} //namespace ojgl
