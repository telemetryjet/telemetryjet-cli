cmake_minimum_required(VERSION 3.16)
cmake_policy(SET CMP0074 NEW)
cmake_policy(SET CMP0083 NEW)

project(TelemetryJetCli VERSION 0.1.0
                        DESCRIPTION "TelemetryJet CLI"
                        LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

### Require out-of-source builds
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
    message(FATAL_ERROR "You cannot build in a source directory (or any directory with a CMakeLists.txt file). Please make a build subdirectory. Feel free to remove CMakeCache.txt and CMakeFiles.")
endif()

# Configuration if this is the main project
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(CMAKE_CXX_EXTENSIONS OFF)
    if (WIN32)
    else()
        set (CMAKE_CXX_FLAGS "-Wall -Wno-ignored-attributes")
    endif()
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif()

#--------------- SOURCE & HEADER FILES --------------------

set(SOURCES
        src/main.cpp
        src/core/data_cache.cpp
        src/core/data_point.cpp
        src/core/network.cpp
        src/utility/glob/glob.cpp
        src/utility/serial/serial_wrapper.cpp
        src/utility/timer/simple_timer.cpp
        src/utility/messagepack/messagepack.cpp
        src/utility/key_value/key_value_utils.cpp
        src/utility/sdl/sdl.cpp
        src/services/service_manager.cpp
        src/services/logger/logger.cpp
        src/services/config/config.cpp

        src/core/data_sources/base/file-input/file_input.cpp
        src/core/data_sources/base/file-output/file_output.cpp
        src/core/data_sources/base/serial-stream/serial_stream.cpp
        src/core/data_sources/bidirectional/key-value-stream/key_value_stream.cpp
        src/core/data_sources/input/test-input/test_input.cpp
        src/core/data_sources/output/console-output/console_output.cpp
        src/core/data_sources/output/csv-file-output/csv_file_output.cpp
        src/core/data_sources/output/key-value-file-output/key_value_file_output.cpp
        src/core/data_sources/input/key-value-file-input/key_value_file_input.cpp
        src/core/data_sources/input/nmea-0183-file-input/nmea_0183_file_input.cpp
        src/core/data_sources/input/nmea-0183-stream/nmea_0183_stream.cpp
        src/core/data_sources/input/system-stats/system_stats.cpp
        src/core/data_sources/input/joystick/joystick.cpp
)

# Platform-specific sources
if (WIN32)
    message(STATUS "Adding Windows-specific sources")
    set(SOURCES
            ${SOURCES}
            src/core/data_sources/input/system-stats/system_stats_win32.cpp
    )
else()
    if (APPLE)
        message(STATUS "Adding Mac-specific sources")
        set(SOURCES
                ${SOURCES}
                src/core/data_sources/input/system-stats/system_stats_mac.cpp
        )
        set(CMAKE_THREAD_LIBS_INIT "-lpthread")
        set(CMAKE_HAVE_THREADS_LIBRARY 1)
        set(CMAKE_USE_WIN32_THREADS_INIT 0)
        set(CMAKE_USE_PTHREADS_INIT 1)
        set(THREADS_PREFER_PTHREAD_FLAG ON)
    else()
        if (UNIX)
            message(STATUS "Adding Linux-specific sources")
            set(SOURCES
                    ${SOURCES}
                    src/core/data_sources/input/system-stats/system_stats_linux.cpp
            )
        else()
            message(FATAL_ERROR "Unknown platform!")
        endif()
    endif()
endif()

set(HEADERS_PRIVATE
        src/constants.h
        src/core/data_cache.h
        src/core/data_point.h
        src/core/data_source.h
        src/core/network.h
        src/utility/glob/glob.h
        src/utility/serial/serial_wrapper.h
        src/utility/timer/simple_timer.h
        src/utility/path_utils.h
        src/utility/time_utils.h
        src/utility/messagepack/messagepack.h
        src/utility/key_value/key_value_utils.h
        src/utility/sdl/sdl.h
        src/services/service_manager.h
        src/services/config/config.h
        src/services/logger/logger.h

        src/core/data_sources/base/file-input/file_input.h
        src/core/data_sources/base/file-output/file_output.h
        src/core/data_sources/base/serial-stream/serial_stream.h
        src/core/data_sources/bidirectional/key-value-stream/key_value_stream.h
        src/core/data_sources/input/test-input/test_input.h
        src/core/data_sources/output/console-output/console_output.h
        src/core/data_sources/output/csv-file-output/csv_file_output.h
        src/core/data_sources/output/key-value-file-output/key_value_file_output.h
        src/core/data_sources/input/key-value-file-input/key_value_file_input.h
        src/core/data_sources/input/nmea-0183-file-input/nmea_0183_file_input.h
        src/core/data_sources/input/nmea-0183-stream/nmea_0183_stream.h
        src/core/data_sources/input/system-stats/system_stats.h
        src/core/data_sources/input/joystick/joystick.h
)

# Put version numbers into header
configure_file(src/version.h.in src/version.h)

#------------------- BUILD TARGETS ------------------------

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS_PRIVATE})

# Build static on linux and windows but not macos
if (WIN32)
    target_link_libraries(${PROJECT_NAME} PUBLIC -static)
    SET(BUILD_SHARED_LIBS OFF)
    SET(CMAKE_EXE_LINKER_FLAGS "-static")
else()
    if (APPLE)
    else()
        if (UNIX)
            target_link_libraries(${PROJECT_NAME} PUBLIC -static)
            SET(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
            SET(BUILD_SHARED_LIBS OFF)
            SET(CMAKE_EXE_LINKER_FLAGS "-static")
        else()
        endif()
    endif()
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "jet")
set_target_properties(${PROJECT_NAME} PROPERTIES SKIP_BUILD_RPATH TRUE)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)

# Add the build directory to the search path so version header can be found
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_BINARY_DIR})

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:${PROJECT_NAME}>)

#--------------- EXTERNAL DEPENDENCIES --------------------

include(FetchContent)

set(Boost_DEBUG 1)
set(Boost_USE_STATIC_LIBS ON)

if(WIN32 AND "x${CMAKE_CXX_COMPILER_ARCHITECTURE_ID}" STREQUAL "x")
    message(WARNING "WIN32 compiler does not specify CMAKE_CXX_COMPILER_ARCHITECTURE_ID -- filling in manually")
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(CMAKE_CXX_COMPILER_ARCHITECTURE_ID "x64")
    else()
        set(CMAKE_CXX_COMPILER_ARCHITECTURE_ID "x86")
    endif()
    message(STATUS "Compiler architecture: ${CMAKE_CXX_COMPILER_ARCHITECTURE_ID}")
endif()

add_definitions(-DBOOST_BIND_GLOBAL_PLACEHOLDERS)
find_package(Boost COMPONENTS log log_setup system filesystem date_time REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

FetchContent_Declare(
        CLI11
        GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
)
FetchContent_MakeAvailable(CLI11)

FetchContent_Declare(
  fmtlib
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG 5.3.0
)
FetchContent_MakeAvailable(fmtlib)

option(SQLITECPP_USE_STACK_PROTECTION "USE Stack Protection hardening." OFF)
FetchContent_Declare(
        SQLiteCpp
        GIT_REPOSITORY https://github.com/SRombauts/SQLiteCpp.git
)
FetchContent_MakeAvailable(SQLiteCpp)

FetchContent_Declare(
        Simple-Web-Server
        GIT_REPOSITORY https://gitlab.com/eidheim/Simple-Web-Server.git
        GIT_TAG v3.1
)
FetchContent_MakeAvailable(Simple-Web-Server)

FetchContent_Declare(
        Simple-WebSocket-Server
        GIT_REPOSITORY https://gitlab.com/eidheim/Simple-WebSocket-Server.git
)
FetchContent_MakeAvailable(Simple-WebSocket-Server)

FetchContent_Declare(
        NemaTode
        GIT_REPOSITORY https://github.com/ckgt/NemaTode
)
FetchContent_MakeAvailable(NemaTode)

FetchContent_Declare(
        SDL2
        GIT_REPOSITORY https://github.com/chrisdalke/SDL.git
)
FetchContent_MakeAvailable(SDL2)

FetchContent_Declare(json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.7.3)

FetchContent_GetProperties(json)
if(NOT json_POPULATED)
    FetchContent_Populate(json)
    add_subdirectory(${json_SOURCE_DIR} ${json_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

FetchContent_Declare(
        serialport
        GIT_REPOSITORY https://github.com/chrisdalke/libserialport-cmake.git
)
if(NOT serialport_POPULATED)
    FetchContent_Populate(serialport)
    add_subdirectory(${serialport_SOURCE_DIR} ${serialport_BINARY_DIR} EXCLUDE_FROM_ALL)
    target_include_directories(${PROJECT_NAME} PUBLIC ${serialport_SOURCE_DIR})
    message(STATUS "Serialport source directory ${serialport_SOURCE_DIR}")
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE CLI11::CLI11)
target_link_libraries(${PROJECT_NAME} PRIVATE SQLiteCpp)
target_link_libraries(${PROJECT_NAME} PRIVATE simple-web-server)
target_link_libraries(${PROJECT_NAME} PRIVATE simple-websocket-server)
target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt)
target_link_libraries(${PROJECT_NAME} PRIVATE NemaTode)
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(${PROJECT_NAME} PRIVATE serialport-static)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2-static)
target_link_libraries(${PROJECT_NAME} PRIVATE Boost::log_setup Boost::log Boost::system Boost::filesystem Boost::date_time)
#--------------- PLATFORM-SPECIFIC DEPENDENCIES & FLAGS --------------------

# Dependencies and build flags for individual platforms
if (WIN32)
    message(STATUS "Adding Windows-specific libraries")
    add_definitions(-DPLATFORM_WIN32)
else()
    if (APPLE)
        message(STATUS "Adding Apple-specific libraries")
        add_definitions(-DPLATFORM_APPLE)

        # General-purpose header location for mac installed libraries
        target_include_directories(${PROJECT_NAME} PUBLIC /usr/local/include)
    else()
        if (UNIX)
            message(STATUS "Adding Linux-specific libraries")
            add_definitions(-DPLATFORM_LINUX)

            # General-purpose header location for linux installed libraries
            target_include_directories(${PROJECT_NAME} PUBLIC /usr/local/include)

        else()
            message(FATAL_ERROR "Unknown platform!")
        endif()
    endif()
endif()

#--------------- VARIOUS PROJECT ORGANIZATION --------------------
# IDEs should put the headers in a nice place
source_group(TREE "${PROJECT_SOURCE_DIR}/include" PREFIX "Header Files" FILES ${HEADERS})
