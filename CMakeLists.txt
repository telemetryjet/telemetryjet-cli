cmake_minimum_required(VERSION 3.18)

project(TelemetryJetCli VERSION 0.0.1
                        DESCRIPTION "TelemetryJet CLI"
                        LANGUAGES CXX)

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
        #src/devices/device_manager.cpp
        #src/devices/protocols/mock/mock_device.cpp
        #src/devices/protocols/nmea_0183/nmea_0183_device.cpp
        #src/devices/protocols/system_usage/system_usage_device.cpp
        #src/model/data_point.cpp
        #src/services/config/json_env_config.cpp
        #src/services/config/persisted_config.cpp
        #src/services/service_manager.cpp
        #src/services/logger/console_logger.cpp
        #src/services/database/sqlite/sqlite_database.cpp
        #src/services/database/sqlite/sqlite_database_data_points.cpp
        #src/services/data_cache/cache.cpp
        #src/services/api/rest/rest_api_server.cpp
        #src/services/api/streaming/streaming_server.cpp
        #src/utility/simple_timer.cpp
        #src/utility/serial_wrapper.cpp
)

# Platform-specific sources
if (WIN32)
    message(STATUS "Adding Windows-specific sources")
    #set(SOURCES
    #        ${SOURCES}
    #        src/devices/protocols/system_usage/system_usage_statistics_win32.cpp
    #)
else()
    if (APPLE)
        message(STATUS "Adding Mac-specific sources")
        #set(SOURCES
        #        ${SOURCES}
        #        src/devices/protocols/system_usage/system_usage_statistics_mac.cpp
        #        )
    else()
        if (UNIX)
            message(STATUS "Adding Linux-specific sources")
            #set(SOURCES
            #        ${SOURCES}
            #        src/devices/protocols/system_usage/system_usage_statistics_linux.cpp
            #        )
        else()
            message(FATAL_ERROR "Unknown platform!")
        endif()
    endif()
endif()


set(HEADERS_PRIVATE
        #src/constants.h
        #src/devices/device_manager.h
        #src/devices/protocols/mock/mock_device.h
        #src/devices/protocols/nmea_0183/nmea_0183_device.h
        #src/devices/protocols/system_usage/system_usage_statistics.h
        #src/devices/protocols/system_usage/system_usage_device.h
        #src/devices/device.h
        #src/model/data_point.h
        #src/services/service_manager.h
        #src/services/config/config.h
        #src/services/config/json_env_config.h
        #src/services/config/persisted_config.h
        #src/services/logger/logger.h
        #src/services/logger/console_logger.h
        #src/services/database/database.h
        #src/services/database/sqlite/sqlite_database.h
        #src/services/data_cache/cache.h
        #src/utility/time_utils.h
        #src/utility/path_utils.h
        #src/utility/simple_timer.h
        #src/utility/serial_wrapper.h
        #src/ui/tray/tray_ui.h
)

#------------------- BUILD TARGETS ------------------------

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS_PRIVATE})
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "jet")
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)

#--------------- EXTERNAL DEPENDENCIES --------------------

include(FetchContent)

set(Boost_DEBUG 1)
set(Boost_USE_STATIC_LIBS ON)
add_definitions(-DBOOST_BIND_GLOBAL_PLACEHOLDERS)
find_package(Boost COMPONENTS log log_setup system filesystem date_time REQUIRED)

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

target_link_libraries(${PROJECT_NAME} PRIVATE CLI11::CLI11)
target_link_libraries(${PROJECT_NAME} PRIVATE SQLiteCpp)
target_link_libraries(${PROJECT_NAME} PRIVATE simple-web-server)
target_link_libraries(${PROJECT_NAME} PRIVATE simple-websocket-server)
target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt)
target_link_libraries(${PROJECT_NAME} PRIVATE NemaTode)
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
