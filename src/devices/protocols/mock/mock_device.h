#ifndef TELEMETRYSERVER_MOCK_DEVICE_H
#define TELEMETRYSERVER_MOCK_DEVICE_H

#include "devices/device.h"
#include <utility/simple_timer.h>

/**
 * Mock device that generates a sine wave with some noise sampled at 10Hz
 */
class MockDevice : public Device {
private:
    SimpleTimer* timer;
    int x;

public:
    void open(std::string address) override;
    void update() override;
    void close() override;
    std::string getAddress() override;

    static std::string name() {
        return "Mock Device";
    }
};

#endif  // TELEMETRYSERVER_MOCK_DEVICE_H
