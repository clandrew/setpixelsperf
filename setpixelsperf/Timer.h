#pragma once

class Timer
{
    LARGE_INTEGER m_freq;
    LARGE_INTEGER m_start;
    LARGE_INTEGER m_end;

public:
    void Initialize();

    void Start();

    void Stop();

    std::wstring GetReport();
};