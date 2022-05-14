#pragma once

#include "utility/OJstd.h"
#include "utility/Vector.h"

namespace ojgl {

class Window {
public:
    Window(Vector2i size, ojstd::string title, bool fullScreen, bool showCursor);
    Window(const Window& other) = delete;
    Window& operator=(const Window& other) = delete;
    ~Window();
    void getMessages();
    ojstd::vector<unsigned int> getPressedKeys();
    ojstd::unordered_set<unsigned int> getDownKeys() const;
    bool isClosePressed() const;
    bool isLeftMouseButtonDown() const;
    Vector2i getCursorPosition() const;
    void setTitle(const ojstd::string& title);
    Vector2i size() const;
    void* hdcBackend() const;

public:
    static constexpr int KEY_LEFT = 37;
    static constexpr int KEY_UP = 38;
    static constexpr int KEY_RIGHT = 39;
    static constexpr int KEY_DOWN = 40;
    static constexpr int KEY_ESCAPE = 27;
    static constexpr int KEY_SPACE = 32;
    static constexpr int KEY_R = 82;
    static constexpr int KEY_P = 80;
    static constexpr int KEY_F1 = 112;
    static constexpr int KEY_W = 0x57;
    static constexpr int KEY_S = 0x53;
    static constexpr int KEY_D = 0x44;
    static constexpr int KEY_A = 0x41;
    static constexpr int KEY_Z = 0x5A;
    static constexpr int KEY_X = 0x58;
    static constexpr int KEY_C = 0x43;
    static constexpr int KEY_K = 0x4b;

private:
    class Details;
    ojstd::unique_ptr<Details> _priv;
};
} //namespace ojgl
