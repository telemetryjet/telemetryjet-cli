cmake_minimum_required(VERSION 3.11)
project(libserialport VERSION 0.1.1
                      DESCRIPTION "libserialport library"
                      LANGUAGES CXX C)

include(FetchContent)

FetchContent_Declare(
  libserialport
  GIT_REPOSITORY https://github.com/sigrokproject/libserialport.git
  GIT_TAG        libserialport-0.1.1
)
FetchContent_Populate(libserialport)

add_compile_options(-fdeclspec)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-ignored-attributes")

if (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c99 -Wall -Wextra -pedantic -Wmissing-prototypes -Wshadow")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --enable-auto-import")
endif()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${CMAKE_CXX_FLAGS} -s -Os")

set(SOURCE_FILES
    ${libserialport_SOURCE_DIR}/serialport.c 
    ${libserialport_SOURCE_DIR}/libserialport_internal.h
    ${PROJECT_SOURCE_DIR}/libserialport.h
    ${PROJECT_SOURCE_DIR}/config.h)

if (WIN32)
    set (SOURCE_FILES
        ${SOURCE_FILES}
        ${libserialport_SOURCE_DIR}/windows.c)
endif(WIN32)
if(APPLE)
    set (SOURCE_FILES
        ${SOURCE_FILES}
        ${libserialport_SOURCE_DIR}/macosx.c)
else()
    if (UNIX)
        set (SOURCE_FILES
            ${SOURCE_FILES}
            ${libserialport_SOURCE_DIR}/linux.c
            ${libserialport_SOURCE_DIR}/linux_termios.c
            ${libserialport_SOURCE_DIR}/linux_termios.h)
    endif()
endif()
if (CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
    set (SOURCE_FILES
        ${SOURCE_FILES}
        ${libserialport_SOURCE_DIR}/freebsd.c)
endif (CMAKE_SYSTEM_NAME MATCHES "FreeBSD")

add_library(${PROJECT_NAME}-static STATIC ${SOURCE_FILES})
add_library(${PROJECT_NAME}-shared SHARED ${SOURCE_FILES})

set_target_properties(${PROJECT_NAME}-static PROPERTIES OUTPUT_NAME ${PROJECT_NAME}) 
set_target_properties(${PROJECT_NAME}-shared PROPERTIES OUTPUT_NAME ${PROJECT_NAME}) 

target_include_directories(${PROJECT_NAME}-static PUBLIC ${PROJECT_SOURCE_DIR})
target_include_directories(${PROJECT_NAME}-shared PUBLIC ${PROJECT_SOURCE_DIR})
target_include_directories(${PROJECT_NAME}-static PUBLIC ${libserialport_SOURCE_DIR})
target_include_directories(${PROJECT_NAME}-shared PUBLIC ${libserialport_SOURCE_DIR})

if(WIN32)
    target_link_libraries(${PROJECT_NAME}-static cfgmgr32 SetupAPI)
    target_link_libraries(${PROJECT_NAME}-shared cfgmgr32 SetupAPI)
endif()

if(APPLE)
    target_link_libraries(${PROJECT_NAME}-shared
        "-framework Foundation"
        "-framework IOKit"
        )
    target_link_libraries(${PROJECT_NAME}-static
        "-framework Foundation"
        "-framework IOKit"
        )
endif()