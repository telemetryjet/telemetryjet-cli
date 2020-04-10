cmake_minimum_required(VERSION 3.11)
project(tray VERSION 0.1.0
        DESCRIPTION "Crossplatform tray library from https://github.com/zserge/tray"
        LANGUAGES CXX C)

set(header_files
    ${CMAKE_CURRENT_SOURCE_DIR}/include/tray.h
)

add_library(${PROJECT_NAME} INTERFACE)

if (WIN32)
    target_compile_definitions(${PROJECT_NAME} INTERFACE -DTRAY_WINAPI=1)
endif()
if (APPLE)
    target_compile_definitions(${PROJECT_NAME} INTERFACE -DTRAY_APPKIT=1)
    target_link_libraries(${PROJECT_NAME} INTERFACE "-framework Cocoa")
else()
    if (UNIX)
        add_definitions(-DTRAY_APPINDICATOR=1)
        # Todo: Add link to appindicator library
        # TRAY_CFLAGS := -DTRAY_APPINDICATOR=1 $(shell pkg-config --cflags appindicator3-0.1)
        # TRAY_LDFLAGS := $(shell pkg-config --libs appindicator3-0.1)
    endif()
endif()

target_sources(${PROJECT_NAME} INTERFACE "$<BUILD_INTERFACE:${header_files}>")
target_include_directories(${PROJECT_NAME} INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>)
target_link_libraries(${PROJECT_NAME} INTERFACE)
