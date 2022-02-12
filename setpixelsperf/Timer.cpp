#include "pch.h"
#include "Timer.h"

void Timer::Initialize()
{
    QueryPerformanceFrequency(&m_freq);
}

void Timer::Start()
{
    QueryPerformanceCounter(&m_start);
}

void Timer::Stop()
{
    QueryPerformanceCounter(&m_end);
}

std::wstring Timer::GetBriefReport()
{
    LONGLONG diff = m_end.QuadPart - m_start.QuadPart;
    float time = (float)diff / (float)m_freq.QuadPart;

    std::wstringstream strm;
    strm << L"Time: " << time << L"s";
    return strm.str();
}

std::wstring Timer::GetFullReport(int width, int height, Mode mode)
{
    LONGLONG diff = m_end.QuadPart - m_start.QuadPart;
    float time = (float)diff / (float)m_freq.QuadPart;

    int totalPixelCount = width * height;

    std::wstringstream strm;
    strm << L"Width: " << width;
    strm << L"\tHeight: " << height;
    strm << L"\tPixel Count: " << totalPixelCount;
    strm << L"\tMode: " << (mode == Mode::SetDIBits ? L"SetDIBits" : L"SetPixel");
    strm << L"\tTime: " << time << L"s";
    strm << L"\n";
    return strm.str();
}