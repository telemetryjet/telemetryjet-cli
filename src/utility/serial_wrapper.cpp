#include "serial_wrapper.h"
#include "services/service_manager.h"
#include "fmt/format.h"

SerialWrapper::SerialWrapper(const std::string& portName, int baudRate) {
    serialPortName = portName;
    serialBaudRate = baudRate;

    sp_return serialStatus = sp_get_port_by_name(serialPortName.c_str(), &serialPort);
    if (serialStatus == SP_OK) {
        serialStatus = sp_open(serialPort, SP_MODE_READ_WRITE);
        if (serialStatus == SP_OK) {
            sp_set_baudrate(serialPort, serialBaudRate);
            serialPortOpen = true;
        }
    }
    serialPortNumBytes = 0;
    if (!serialPortOpen) {
        SM::getLogger()->warning(fmt::format("Failed to initialize serial port [port = {}, rate = {}]", serialPortName, serialBaudRate));
    } else {
        SM::getLogger()->debug(fmt::format("Initialized serial port [port = {}, rate = {}]", serialPortName, serialBaudRate));
    }
}

SerialWrapper::~SerialWrapper() {
    sp_close(serialPort);
    serialPortOpen = false;
    SM::getLogger()->debug(fmt::format("Closed serial port [port = {}, rate = {}]", serialPortName, serialBaudRate));
}

void SerialWrapper::poll() {
    if (serialPortOpen) {
        // Add bytes to the buffer in chunks of 8
        uint8_t nextBytes[8];
        while (sp_input_waiting(serialPort) > 0) {
            int num_bytes = sp_nonblocking_read(serialPort, nextBytes, 8);
            for (int i = 0; i < num_bytes; i++) {
                serialPortBuffer.push_back(nextBytes[i]);
                serialPortNumBytes++;
            }
        }
    }
}

int SerialWrapper::getNumBytes() {
    return serialPortNumBytes;
}

std::string SerialWrapper::getPortName() {
    return serialPortName;
}

int SerialWrapper::getBaudRate() {
    return serialBaudRate;
}

bool SerialWrapper::isOpen() {
    return serialPortOpen;
}

std::list<uint8_t> &SerialWrapper::getBuffer() {
    return serialPortBuffer;
}

void SerialWrapper::clearBuffer() {
    serialPortBuffer.clear();
}