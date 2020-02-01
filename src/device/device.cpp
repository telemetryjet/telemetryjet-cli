#include "device.hpp"

struct Packet {
    uint8_t  startByte;
    uint16_t dataId;
    uint32_t data;
    uint8_t  checksum;
};

uint8_t generateChecksum(struct Packet* p) {
    uint8_t* p8 = (uint8_t*)p;
    uint8_t  s  = 0;
    for (size_t i = 0; i < 8; i++)
        s += p8[i];
    return 0xff - s;
}

uint8_t validateChecksum(uint8_t* p) {
    uint8_t s = 0;
    for (size_t i = 0; i < 8; i++)
        s += p[i];
    return s;
}

// Opens/initialized the device.
Device::Device(std::string portName)
    : _portName(portName)
    , ppsTimer(1000) {
    std::cout << "[Device] Initializing device with portName=" << portName << "\n";
    sp_return serialStatus = sp_get_port_by_name(_portName.c_str(), &_port);
    if (serialStatus == SP_OK) {
        serialStatus = sp_open(_port, SP_MODE_READ_WRITE);
        if (serialStatus == SP_OK) {
            sp_set_baudrate(_port, 115200);
            _isOpen = true;
        }
    }
    numPackets = 0;
}

// Closes the device.
void Device::close() {
    std::cout << "[Device] Closing device with portName=" << _portName << "\n";
    sp_close(_port);
    _isOpen = false;
}

void Device::getData() {
    // Add bytes to the buffer in chunks of 8
    uint8_t nextBytes[8];
    while (sp_input_waiting(_port) > 0) {
        int num_bytes = sp_nonblocking_read(_port, nextBytes, 8);
        for (int i = 0; i < num_bytes; i++) {
            buffer.push_back(nextBytes[i]);
        }
        // std::cout << "read bytes: " << num_bytes << "\n";
    }

    // Delete from the buffer until length is zero or we find a start byte
    // This will remove invalid packets, old data, etc. After this operation, the first byte is
    // GUARANTEED to be a 0xF0 header, or the buffer is empty.
    while (buffer.size() > 0 && buffer.front() != 0xF0) {
        buffer.pop_front();
    }

    // If the buffer still has data, then it must be a packet!
    // Consume it and read it
    if (buffer.size() >= 8) {
        uint8_t byteBuf[8];
        for (int i = 0; i < 8; i++) {
            byteBuf[i] = buffer.front();
            buffer.pop_front();
        }

        uint8_t checksum = validateChecksum(byteBuf);

        // Check that checksum is valid
        // std::cout << "Complete packet with checksum: " << (int)checksum << "\n";
        numPackets++;
    }

    // Debug: Evaluate packets/second
    if (ppsTimer.check()) {
        std::cout << "packets/second: " << numPackets << "\n";
        numPackets = 0;
    }
}