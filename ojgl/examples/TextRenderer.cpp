#include "TextRenderer.hpp"
#include "utility/Log.h"
#include <Windows.h>

using namespace ojgl;

TextRenderer& TextRenderer::instance()
{
    static TextRenderer instance;
    return instance;
}

void TextRenderer::setHDC(void* hdc)
{
    _hdcBackend = hdc;
}

ojstd::shared_ptr<Texture> TextRenderer::get(const ojstd::string& text)
{
    int w = 830;
    int h = 830;

    HDC vhdc = CreateCompatibleDC((HDC)_hdcBackend);
    if (!vhdc)
        throw "error with vhdc";
    HBITMAP hbmp = CreateCompatibleBitmap((HDC)_hdcBackend, w, h);
    BITMAPINFO bmpi = { { sizeof(BITMAPINFOHEADER), w, h, 1, 32, BI_RGB, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0 } };

    SelectObject(vhdc, hbmp);
    ExtFloodFill(vhdc, 0, 0, 0x00FFFFFF, FLOODFILLBORDER);

    auto hFont = CreateFont(96, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
        CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Segoe Script"));

    SelectObject(vhdc, hFont);
    TextOut(vhdc, 0, 0, text.c_str(), text.length());

    GetDIBits(vhdc, hbmp, 0, h, nullptr, &bmpi, BI_RGB);

    BYTE* buf = new BYTE[bmpi.bmiHeader.biSizeImage];
    GetDIBits(vhdc, hbmp, 0, h, buf, &bmpi, BI_RGB);

    auto result = Texture::construct(w, h, 4, buf);

    delete[] buf;

    return result;
}
