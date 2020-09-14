cmake_minimum_required(VERSION 2.8.7)

project(osp_rtw)

#include_directories
include_directories(${AVM_ROOT_DIRECTORY}/src/inc)
include_directories(${OSP_RT_WORKER_ROOT_SRC_DIR})
include_directories(${OSP_RT_WORKER_EXTS_DIR})
include_directories(${OSP_RT_WORKER_EXTS_DIR}/ProgramOptions)
#set
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)
set(USE_FOLDERS TRUE)

#file
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.c)
file(GLOB_RECURSE SOURCES_CPP ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

#add_executable
add_executable(osp_rtw_app ${HEADERS} ${SOURCES} ${SOURCES_CPP})

#target_link_libraries
target_link_libraries(osp_rtw_app avm osp_mw_can)