#ifndef H_SIMPLE_TIMER
#define H_SIMPLE_TIMER

#include <chrono>
#include <cstdint>
#include <iostream>
using namespace std::chrono;

class SimpleTimer {
public:
    SimpleTimer(uint64_t interval);
    bool check();

private:
    uint64_t m_lastTime;
    uint64_t m_interval;
};

#endif
