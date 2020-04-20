cmake_minimum_required(VERSION 3.12)

project(TelemetryServer VERSION 0.0.1
                        DESCRIPTION "Telemetry Server"
                        LANGUAGES CXX)

### Require out-of-source builds
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
    message(FATAL_ERROR "You cannot build in a source directory (or any directory with a CMakeLists.txt file). Please make a build subdirectory. Feel free to remove CMakeCache.txt and CMakeFiles.")
endif()

# Configuration if this is the main project
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(CMAKE_CXX_EXTENSIONS OFF)
    set (CMAKE_CXX_FLAGS "-Wall -Wno-ignored-attributes")
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif()

#--------------- SOURCE & HEADER FILES --------------------

set(SOURCES
        src/main.cpp
        src/devices/device_manager.cpp
        src/devices/protocols/nmea_0183/nmea_0183_device.cpp
        src/model/config_item.cpp
        src/model/dashboard.cpp
        src/model/data_frame.cpp
        src/model/data_point.cpp
        src/model/device.cpp
        src/model/log.cpp
        src/model/system.cpp
        src/services/config/json_env_config.cpp
        src/services/config/persisted_config.cpp
        src/services/service_manager.cpp
        src/services/logger/console_logger.cpp
        src/services/database/sqlite/sqlite_database.cpp
        src/services/database/sqlite/sqlite_database_systems.cpp
        src/services/database/sqlite/sqlite_database_devices.cpp
        src/services/database/sqlite/sqlite_database_logs.cpp
        src/services/database/sqlite/sqlite_database_config_items.cpp
        src/services/database/sqlite/sqlite_database_dashboards.cpp
        src/services/database/sqlite/sqlite_database_data_frames.cpp
        src/services/database/sqlite/sqlite_database_data_points.cpp
        src/services/data_cache/in_memory_cache.cpp
        src/services/api/rest/rest_api_server.cpp
        src/services/api/streaming/streaming_server.cpp
        src/utility/simple_timer.cpp
        src/utility/serial_wrapper.cpp
)

# Platform-specific sources
if (WIN32)
    set(SOURCES
            ${SOURCES}
            src/ui/tray/tray_ui_winapi.cpp
    )
endif()
if (APPLE)
    set(SOURCES
            ${SOURCES}
            src/ui/tray/tray_ui_appkit.cpp
    )
else()
    if (UNIX)
        set(SOURCES
                ${SOURCES}
                src/ui/tray/tray_ui_gtk.cpp
        )
    endif()
endif()

set(HEADERS_PRIVATE
        src/constants.h
        src/devices/device_manager.h
        src/devices/protocols/nmea_0183/nmea_0183_device.h
        src/devices/device.h
        src/model/config_item.h
        src/model/dashboard.h
        src/model/data_frame.h
        src/model/data_point.h
        src/model/device.h
        src/model/log.h
        src/model/system.h
        src/services/service_manager.h
        src/services/config/config.h
        src/services/config/json_env_config.h
        src/services/config/persisted_config.h
        src/services/logger/logger.h
        src/services/logger/console_logger.h
        src/services/database/database.h
        src/services/database/sqlite/sqlite_database.h
        src/services/data_cache/cache.h
        src/services/data_cache/in_memory_cache.h
        src/services/api/rest/rest_api_server.h
        src/services/api/streaming/streaming_server.h
        src/utility/time_utils.h
        src/utility/path_utils.h
        src/utility/json_utils.h
        src/utility/simple_timer.h
        src/utility/serial_wrapper.h
        src/ui/tray/tray_ui.h
)

#------------------- BUILD TARGETS ------------------------

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS_PRIVATE})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PUBLIC /usr/local/opt/sqlite/include)

# Dependencies for tray UI
if (WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE -DTRAY_WINAPI=1)
endif()
if (APPLE)
    target_compile_definitions(${PROJECT_NAME} PRIVATE -DTRAY_APPKIT=1)
    target_link_libraries(${PROJECT_NAME} PRIVATE "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} PRIVATE "-framework AppKit")
    target_link_libraries(${PROJECT_NAME} PRIVATE "-framework Foundation")
else()
    if (UNIX)
        target_compile_definitions(${PROJECT_NAME} PRIVATE -DTRAY_APPINDICATOR=1)
        # Todo: Add link to appindicator library
        # TRAY_CFLAGS := -DTRAY_APPINDICATOR=1 $(shell pkg-config --cflags appindicator3-0.1)
        # TRAY_LDFLAGS := $(shell pkg-config --libs appindicator3-0.1)
    endif()
endif()

#--------------- EXTERNAL DEPENDENCIES --------------------

include(FetchContent)

set(Boost_DEBUG 1)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_ARCHITECTURE -x32)
find_package(Boost COMPONENTS log log_setup system filesystem date_time REQUIRED)

FetchContent_Declare(
  fmtlib
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG        5.3.0
)
FetchContent_MakeAvailable(fmtlib)

FetchContent_Declare(
        SQLiteCpp
        GIT_REPOSITORY https://github.com/SRombauts/SQLiteCpp.git
        GIT_TAG        3.0.0
)
FetchContent_MakeAvailable(SQLiteCpp)

FetchContent_Declare(
        Simple-Web-Server
        GIT_REPOSITORY https://gitlab.com/eidheim/Simple-Web-Server.git
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

add_subdirectory(lib)

link_directories(${PROJECT_NAME} PUBLIC /usr/local/opt/sqlite/lib)

target_link_libraries(${PROJECT_NAME} PRIVATE SQLiteCpp)
target_link_libraries(${PROJECT_NAME} PRIVATE simple-web-server)
target_link_libraries(${PROJECT_NAME} PRIVATE simple-websocket-server)
target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt)
target_link_libraries(${PROJECT_NAME} PRIVATE libserialport-static)
target_link_libraries(${PROJECT_NAME} PRIVATE sqlite3)
target_link_libraries(${PROJECT_NAME} PRIVATE NemaTode)
target_link_libraries(${PROJECT_NAME} PRIVATE Boost::log_setup Boost::log Boost::system Boost::filesystem Boost::date_time)

#--------------- EXTERNAL DEPENDENCIES --------------------
# IDEs should put the headers in a nice place
#source_group(TREE "${PROJECT_SOURCE_DIR}/include" PREFIX "Header Files" FILES ${HEADERS})
