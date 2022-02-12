#pragma once

struct MemoryBitmap
{
    struct MyBitmapInfo
    {
        BITMAPINFOHEADER bmiHeader;
        RGBQUAD color[256]; // from header.biClrUsed
    };
    std::unique_ptr<MyBitmapInfo> m_myBitmapInfo;

    int m_stride;

public:

    void Initialize(int width, int height, HWND hwnd);
    void SetPixel(int x, int y, unsigned int value);
    void Commit();

    std::vector<unsigned char> m_bits;
    HBITMAP m_bmp;
    HDC m_hdc;

    BITMAP m_bitmapDesc{};

};