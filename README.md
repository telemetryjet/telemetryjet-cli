# TelemetryJet Server

The TelemetryJet Server is a lightweight, fast realtime telemetry server which handles data collection, processing, and storage from any number of distributed devices. The server comes out of the box with:

- Implementations for many common serial protocols, including NMEA 0183, Mavlink, CAN-bus, VE.Direct, Key-Length-value strings, and more.
- Configuration and data collection via a SQLite database.
- REST and Streaming API allowing other software to integrate with the server.
- Realtime data syncronization between server peers; syncronization between local and cloud server.

The server is designed to be lightweight, lightning-fast, and cross-platform. TelemetryJet can be run on a desktop computer or single-board computer such as a Raspberry Pi.

## Getting Started

TelemetryJet is built in modern C++. This project requires [Boost 1.66.0](https://www.boost.org/users/history/version_1_66_0.html) or greater to be installed on the development computer.

## Build & Run

The project uses CMake to configure dependencies and build setup. 

- **First-time Setup**: The first time you build, you must run `./devops/build_init.sh` from the project root. CMake will configure a `build/` directory containing a `make` configuration.
- **Building**: run `./devops/build.sh` to build from the project root.
- **Running**: run `./devops/run.sh` to run the latest binary from the project root.

## IDE Setup

### CLion
The preferred IDE for this project is CLion. If you use an IDE, open the root `tjet-server` directory, and the `CMakeLists.txt`  file should be loaded.

### XCode
To use XCode with this project, run `./devops/xcode_init.sh`. This will prompt CMake to configure an XCode project in the `xcode/` folder.

## Project Organization
The organization of the project roughly follows the Pitchfork Layout: "a convention for laying out source, build, and resource files in a filesystem to aide in uniformity, tooling, understandability, and compartmentalization."
For more details and an explanation of the project layout, see:
https://api.csswg.org/bikeshed/?force=1&url=https://raw.githubusercontent.com/vector-of-bool/pitchfork/develop/data/spec.bs

