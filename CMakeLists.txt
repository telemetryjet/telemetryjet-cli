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
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif()

#--------------- SOURCE & HEADER FILES --------------------

set(SOURCES
    src/main.cpp
    src/deviceManager.cpp
    src/simpleTimer.cpp
)

set(HEADERS_PRIVATE
    src/deviceManager.hpp
    src/simpleTimer.hpp
    src/logger.hpp
)

#------------------- BUILD TARGETS ------------------------

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS_PRIVATE})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)

#--------------- EXTERNAL DEPENDENCIES --------------------

include(FetchContent)

SET(Boost_USE_STATIC_LIBS ON)
find_package(Boost COMPONENTS log log_setup system REQUIRED)

FetchContent_Declare(
  fmtlib
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG        5.3.0
)
FetchContent_MakeAvailable(fmtlib)

target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt)
target_link_libraries(${PROJECT_NAME} PRIVATE Boost::log_setup Boost::log Boost::system)

#--------------- EXTERNAL DEPENDENCIES --------------------
# IDEs should put the headers in a nice place
#source_group(TREE "${PROJECT_SOURCE_DIR}/include" PREFIX "Header Files" FILES ${HEADERS})
