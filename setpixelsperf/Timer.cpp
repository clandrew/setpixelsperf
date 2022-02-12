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

std::wstring Timer::GetReport()
{
    LONGLONG diff = m_end.QuadPart - m_start.QuadPart;
    float time = (float)diff / (float)m_freq.QuadPart;

    std::wstringstream strm;
    strm << L"Time: " << time << L"s";
    return strm.str();
}