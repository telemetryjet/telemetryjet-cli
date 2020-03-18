cmake_minimum_required(VERSION 3.11)

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
        src/services/api/rest/rest_api_server.cpp
        src/services/model/system_record_manager.cpp
        src/services/model/log_record_manager.cpp
        src/services/model/device_record_manager.cpp
        src/services/model/dashboard_record_manager.cpp
        src/services/model/data_point_record_manager.cpp
        src/services/model/data_frame_record_manager.cpp
        src/utility/simple_timer.cpp
)

set(HEADERS_PRIVATE
        src/constants.h
        src/services/service_manager.h
        src/services/config/config.h
        src/services/config/json_env_config.h
        src/services/config/persisted_config.h
        src/services/logger/logger.h
        src/services/logger/console_logger.h
        src/services/database/database.h
        src/services/database/sqlite/sqlite_database.h
        src/services/model/system_record_manager.h
        src/services/model/log_record_manager.h
        src/services/model/device_record_manager.h
        src/services/model/dashboard_record_manager.h
        src/services/model/data_point_record_manager.h
        src/services/model/data_frame_record_manager.h
        src/services/api/rest/rest_api_server.h
        src/utility/time_utils.h
        src/utility/path_utils.h
        src/utility/json_utils.h
        src/utility/simple_timer.h
        src/model/records.h
)

#------------------- BUILD TARGETS ------------------------

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS_PRIVATE})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PUBLIC /usr/local/opt/sqlite/include)

#--------------- EXTERNAL DEPENDENCIES --------------------

include(FetchContent)

SET(Boost_USE_STATIC_LIBS ON)
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

add_subdirectory(lib)

link_directories(${PROJECT_NAME} PUBLIC /usr/local/opt/sqlite/lib)

target_link_libraries(${PROJECT_NAME} PRIVATE SQLiteCpp)
target_link_libraries(${PROJECT_NAME} PRIVATE simple-web-server)
target_link_libraries(${PROJECT_NAME} PRIVATE simple-websocket-server)
target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt)
target_link_libraries(${PROJECT_NAME} PRIVATE libserialport-static)
target_link_libraries(${PROJECT_NAME} PRIVATE sqlite3)
target_link_libraries(${PROJECT_NAME} PRIVATE Boost::log_setup Boost::log Boost::system Boost::filesystem Boost::date_time)

#--------------- EXTERNAL DEPENDENCIES --------------------
# IDEs should put the headers in a nice place
#source_group(TREE "${PROJECT_SOURCE_DIR}/include" PREFIX "Header Files" FILES ${HEADERS})
