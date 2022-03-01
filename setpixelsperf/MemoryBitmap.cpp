#include "pch.h"
#include "MemoryBitmap.h"

void MemoryBitmap::Initialize(int width, int height, HWND hwnd)
{
    HDC windowDC = GetDC(hwnd);

    int numChannels = 3;

    BITMAPINFOHEADER header{};
    header.biSize = sizeof(header);
    header.biWidth = width;
    header.biHeight = -height; // Negative to mean origin is top left
    header.biPlanes = 1;
    header.biBitCount = 24; // numChannels * 8
    header.biCompression = BI_RGB;
    header.biXPelsPerMeter = 96;
    header.biYPelsPerMeter = 96;
    header.biClrUsed = 256;

    m_bits.resize(width * height * numChannels);
    memset(m_bits.data(), 0xA0, m_bits.size());
    m_stride = (width * 3 + 3)& (~3); // It's rounded up to 4 bytes

    m_myBitmapInfo.reset(new MyBitmapInfo());
    m_myBitmapInfo->bmiHeader = header;

    // Creates a device-dependent bitmap from device-independent information.
    m_bmp = CreateDIBitmap(windowDC, &header, 0, m_bits.data(), (BITMAPINFO*)m_myBitmapInfo.get(), DIB_RGB_COLORS);
    GetObject(m_bmp, sizeof(BITMAP), &m_bitmapDesc);
    m_hdc = CreateCompatibleDC(windowDC);
    SelectObject(m_hdc, m_bmp);
}

void MemoryBitmap::SetPixel(int x, int y, unsigned int value)
{
    assert(value <= 0xFFFFFF);
    assert(x >= 0 && y >= 0);
    assert(x < m_bitmapDesc.bmWidth&& y < m_bitmapDesc.bmHeight);

    unsigned char* pPixelData = m_bits.data() + (m_stride * y) + (x * 3);
    unsigned int* pPixel = (unsigned int*)pPixelData;
    *pPixel &= 0xFF000000;
    *pPixel |= value;
}

void MemoryBitmap::Commit()
{
    SetDIBits(m_hdc, m_bmp, 0, m_bitmapDesc.bmHeight, m_bits.data(), (BITMAPINFO*)m_myBitmapInfo.get(), DIB_RGB_COLORS);
}