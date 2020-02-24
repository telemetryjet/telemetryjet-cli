#include "simple_timer.h"
#include <iostream>

SimpleTimer::SimpleTimer(uint64_t interval)
    : m_interval(interval * 1000) {
}

bool SimpleTimer::check() {
    long long currentTime = (system_clock::now().time_since_epoch()).count();
    long long delta       = (currentTime - m_lastTime);
    if (delta >= m_interval) {
        m_lastTime = currentTime;
        return true;
    } else {
        return false;
    }
}
