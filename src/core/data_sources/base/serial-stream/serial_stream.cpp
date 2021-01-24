#include "serial_stream.h"
#include "utility/path_utils.h"

namespace fs = boost::filesystem;

SerialStreamDataSource::SerialStreamDataSource(const json &definition): DataSource(definition) {
    if (options.is_null()) {
        throw std::runtime_error(fmt::format("[{}] data source type '{}' requires an options object", id, type));
    }
    if (!options.contains("port") || !options["port"].is_string()) {
        throw std::runtime_error(fmt::format("[{}] data source type '{}' requires option 'port' of type String", id, type));
    }
    if (!options.contains("baudrate") || !options["baudrate"].is_number_integer()) {
        throw std::runtime_error(fmt::format("[{}] data source type '{}' requires option 'baudrate' of type Integer", id, type));
    }
    portName = options["port"];
    baudRate = options["baudrate"];
    assertDependency("serialport", portName, fmt::format("[{}] Multiple data sources cannot share the same serial port: {}", id, portName));

    if (options.contains("filename") && options["filename"].is_string()) {
        filename = options["filename"];
        rawFileOut = true;
    }
    if (options.contains("mode") && options["mode"].is_string()) {
        modeString = options["mode"];
    }

    if (rawFileOut) {
        if (modeString == "overwrite") {
            mode = std::ios::trunc;
        } else if (modeString == "append") {
            mode = std::ios::app;
        } else if (modeString == "new") {
            mode = std::ios::app;
        } else {
            throw std::runtime_error(fmt::format("[{}] data source type '{}' has invalid value for 'mode': {}", id, type, modeString));
        }
        assertDependency("file", filename, fmt::format("[{}] Multiple data sources cannot share the same input/output filename: {}", id, filename));
    }
}

void SerialStreamDataSource::open() {
    serial = std::make_unique<SerialWrapper>(portName, baudRate);
    reconnectTimer = std::make_unique<SimpleTimer>(5000);

    if (rawFileOut) {
        // If we are in new file mode, increment filename until the file does not exist
        std::string absPath = resolveRelativePathHome(filename);
        fs::path pathObj(absPath);
        std::string fileName = pathObj.stem().string();
        std::string fileExt = pathObj.extension().string();
        int newFileId = 0;
        if (modeString == "new") {
            do {
                std::string newFilename = fmt::format("{}_{}{}",fileName, newFileId++, fileExt);
                pathObj.remove_filename();
                pathObj.append(newFilename);
            } while (fs::exists(pathObj));
        }
        filename = pathObj.string();
        outputFile = std::ofstream();
        outputFile.open(filename, mode);
        if (!outputFile.is_open()){
            SM::getLogger()->error(fmt::format("[{}] Failed to open file {}", id, pathObj.filename().string()));
            throw std::runtime_error(fmt::format("[{}] Failed to open file {}", id, pathObj.filename().string()));
        } else {
            SM::getLogger()->info(fmt::format("[{}] Opened file {}", id, pathObj.filename().string()));
        }
        flushTimer = std::make_unique<SimpleTimer>(1000);
    }
    DataSource::open();
}

void SerialStreamDataSource::close() {
    if (serial->isOpen()) {
        serial->close();
    }

    if (rawFileOut) {
        if (outputFile.is_open()) {
            outputFile.close();
        }
        flushTimer.reset();
    }

    reconnectTimer.reset();
    DataSource::close();
}

void SerialStreamDataSource::update() {
    if (isOnline) {
        if (serial->isOpen()) {
            hasError = false;
            isOnline = true;
            serial->pollBlocking();
        } else {
            hasError = true;
            isOnline = false;
            serial->close();
        }
    } else {
        if (reconnectTimer->check()) {
            // Periodically try to open serial port
            serial->open();
            // Set error flag if we are unable to connect
            hasError = !serial->isOpen();
            isOnline = serial->isOpen();
        }
    }

}