#ifndef SIMPLE_TIMER_H
#define SIMPLE_TIMER_H

#include <chrono>
#include <cstdint>
#include <iostream>
#include "utility/time_utils.h"

using namespace std::chrono;

class SimpleTimer {
public:
    explicit SimpleTimer(uint64_t interval);
    bool check();
    void wait();
    void reset();

private:
    bool zeroInterval = false;
    uint64_t m_lastTime;
    uint64_t m_interval;
};

#endif
