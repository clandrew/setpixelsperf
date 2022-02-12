#pragma once
#include "Mode.h"

class Timer
{
    LARGE_INTEGER m_freq;
    LARGE_INTEGER m_start;
    LARGE_INTEGER m_end;

public:
    void Initialize();

    void Start();

    void Stop();

    std::wstring GetBriefReport();
    std::wstring GetFullReport(int width, int height, Mode mode);
};