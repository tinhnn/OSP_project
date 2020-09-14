cmake_minimum_required(VERSION 2.8.3)
project(any_serial)

set(${PROJECT_NAME}_SRCS
    any_serial.c
    any_serial_utils.c)
set(${PROJECT_NAME}_HEADERS
    inc/any_serial.h 
    inc/any_serial_platform.h 
    inc/any_serial_types.h
    inc/any_serial_utils.h)
#include_directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc)

# specified OS environment
if(UNIX AND NOT APPLE)
    list(APPEND ${PROJECT_NAME}_SRCS 
        impl/any_serial_linux.c)
    list(APPEND ${PROJECT_NAME}_HEADERS
        inc/any_serial_linux.h)
elseif(WIN32)
    list(APPEND ${PROJECT_NAME}_SRCS 
        impl/any_serial_win.c)
    list(APPEND ${PROJECT_NAME}_HEADERS
        inc/any_serial_win.h)
else()
    message(FATAL_ERROR "Do not support this environment")
endif()

# message(FATAL_ERROR "${${PROJECT_NAME}_HEADERS}")
#add_library
add_library(${PROJECT_NAME} SHARED ${${PROJECT_NAME}_HEADERS} ${${PROJECT_NAME}_SRCS})
# add_library(${PROJECT_NAME} SHARED ${${PROJECT_NAME}_SRCS})

# add_definitions
add_definitions(-DANY_SERIAL_SHARED)
add_definitions(-DANY_SERIAL_EXPORT)
if(WIN32)
    add_definitions(-DANY_SERIAL_MSVC)
# add_definitions(-DANY_SERIAL_OVERLAPPED)
endif()
if(UNIX AND NOT APPLE)
    # target_link_libraries()
elseif(WIN32)
    # target_link_libraries()
else()
    message(FATAL_ERROR "Do not support this environment")
endif()