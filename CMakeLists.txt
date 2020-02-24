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
        src/services/ServiceManager.cpp
        src/services/logger/ConsoleLogger.cpp
)

set(HEADERS_PRIVATE
        src/services/ServiceManager.h
        src/services/config/Config.h
        src/services/logger/Logger.h
        src/services/logger/ConsoleLogger.h
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
