#include "simple_timer.h"
#include <iostream>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>

SimpleTimer::SimpleTimer(uint64_t interval)
    : m_interval(interval) {
    if (interval == 0) {
        zeroInterval = true;
    }
}

bool SimpleTimer::check() {
    if (zeroInterval) {
        return true;
    }
    uint64_t currentTime = getCurrentMillis();
    uint64_t delta       = (currentTime - m_lastTime);
    if (delta >= m_interval) {
        m_lastTime = currentTime;
        return true;
    } else {
        return false;
    }
}

// Sleep until timer expires
void SimpleTimer::wait() {
    if (zeroInterval) {
        return;
    }
    uint64_t currentTime = getCurrentMillis();
    uint64_t delta       = (currentTime - m_lastTime);
    uint64_t timeRemaining = m_interval - delta;

    if (timeRemaining > 0) {
        boost::this_thread::sleep_for(boost::chrono::milliseconds(timeRemaining));
    }
    m_lastTime = getCurrentMillis();
}

void SimpleTimer::reset() {
    // Reset the last time value so the next check will be true
    m_lastTime = 0;
}