#include "simple_timer.h"
#include <iostream>

SimpleTimer::SimpleTimer(uint64_t interval)
    : m_interval(interval) {
}

bool SimpleTimer::check() {
    uint64_t currentTime = getCurrentMillis();
    uint64_t delta       = (currentTime - m_lastTime);
    if (delta >= m_interval) {
        m_lastTime = currentTime;
        return true;
    } else {
        return false;
    }
}

void SimpleTimer::reset() {
    // Reset the last time value so the next check will be true
    m_lastTime = 0;
}