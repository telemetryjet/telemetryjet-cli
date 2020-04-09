cmake_minimum_required(VERSION 3.11)
project(tray VERSION 0.1.0
        DESCRIPTION "Crossplatform tray library from https://github.com/zserge/tray"
        LANGUAGES CXX C)

set(header_files
    ${CMAKE_CURRENT_SOURCE_DIR}/include/tray.h
)

add_library(${PROJECT_NAME} INTERFACE)
target_sources(${PROJECT_NAME} INTERFACE "$<BUILD_INTERFACE:${header_files}>")
target_include_directories(${PROJECT_NAME} INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>)
target_link_libraries(${PROJECT_NAME} INTERFACE)
