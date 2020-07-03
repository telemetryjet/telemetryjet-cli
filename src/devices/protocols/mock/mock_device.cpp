#include "mock_device.h"
#include "fmt/format.h"
#include "services/service_manager.h"
#include <math.h>
#include <stdlib.h>

#define PI 3.14159265

std::string MockDevice::getAddress() {
    return "dummy_address";
}

void MockDevice::open(std::string address) {
    timer = new SimpleTimer(100);
    x = 0;
    SM::getLogger()->info(fmt::format("Started " + name() + " device at port = {}", address));
}

void MockDevice::update() {
    if (timer->check()) {
        float y = 100 * sin(2 * PI * x++ / 10.0);
        float noise = (rand() % 1001 - 500) / 100.0;
        SM::getDataCache()->set<float>("test_value", y + noise);
    }
}

void MockDevice::close() {
    delete timer;
}
