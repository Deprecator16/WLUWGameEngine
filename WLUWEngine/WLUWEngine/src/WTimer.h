#pragma once

#include <chrono>

class WTimer
{
public:
    WTimer() :
        startTime(timer::now())
    {
    
    }

    void reset()
    {
        startTime = timer::now();
    }

    float elapsed() const
    {
        return std::chrono::duration_cast<second>
            (timer::now() - startTime).count();
    }

private:
    typedef std::chrono::high_resolution_clock timer;
    typedef std::chrono::duration<float, std::ratio<1> > second;
    std::chrono::time_point<timer> startTime;
};
