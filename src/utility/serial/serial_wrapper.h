#ifndef SERIAL_WRAPPER_H
#define SERIAL_WRAPPER_H

#include <string>
#include <list>
#include <cstdint>
#include <iostream>
#include <libserialport.h>

/**
 * Serial Wrapper
 * Contains common functionality for interacting with a serial port.
 * Used in many of the protocol implementations.
 */
class SerialWrapper {
private:
    bool serialPortOpen;
    int serialPortNumBytes;
    int serialBaudRate;
    std::string serialPortName;
    sp_port* serialPort;
    std::list<uint8_t> serialPortBuffer;
public:
    SerialWrapper(std::string portName, int baudRate);
    ~SerialWrapper();

    // Core
    void poll();
    void pollBlocking();
    void close();
    void open();

    // Getters
              int getNumBytes();
    std::string   getPortName();
            int   getBaudRate();
            bool       isOpen();

    // Serial Buffer
    std::list<uint8_t>  &getBuffer();
                  void clearBuffer();
};


#endif