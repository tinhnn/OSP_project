cmake_minimum_required(VERSION 2.8.7)

project(osp_rtw)

#set
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)
set(USE_FOLDERS TRUE)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(OSP_RT_WORKER_ROOT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
set(OSP_RT_WORKER_ROOT_SRC_DIR ${OSP_RT_WORKER_ROOT_DIRECTORY}/src)
set(OSP_RT_WORKER_MW_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/mw)
set(OSP_RT_WORKER_DRV_DIRECTORY, ${CMAKE_CURRENT_SOURCE_DIR}/drv)
set(AVM_ROOT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/avm)
set(OSP_RT_WORKER_EXTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/externals)

#add_definitions
# add_definitions(-Dstd=c++11)
add_definitions(-DPROGRAMOPTIONS_NO_COLORS)
add_definitions(-DNDEBUG)
if (CMAKE_VERSION VERSION_LESS "3.1")
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set (CMAKE_CXX_FLAGS "-std=gnu++11 ${CMAKE_CXX_FLAGS}")
    endif ()
else ()
    set (CMAKE_CXX_STANDARD 11)
endif ()

#add_subdirectory
add_subdirectory(avm)
add_subdirectory(externals)
add_subdirectory(src)

#include_directories
# include_directories(${CMAKE_CURRENT_SOURCE_DIR}/avm/src/inc)
# include_directories(${CMAKE_CURRENT_SOURCE_DIR}/externals/inc)
# include_directories(${OSP_RT_WORKER_MW_DIRECTORY})

#file
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h)
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

#add_executable
# add_executable(osp_rtw_app ${HEADERS} ${SOURCES})

#target_link_libraries
# target_link_libraries(osp_rtw_app osp_rtw osp_rtw_mw )
