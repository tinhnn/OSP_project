# project(osp_rtw)
#include_directoies
include_directories(${AVM_ROOT_DIRECTORY}/src/inc)
# message("====> OSP_RT_WORKER_EXTS_DIR [${OSP_RT_WORKER_EXTS_DIR}]")
include_directories(${OSP_RT_WORKER_EXTS_DIR})
include_directories(${OSP_RT_WORKER_ROOT_DIRECTORY}/src)
#file
file(GLOB_RECURSE PUB_HEADERS ${AVM_ROOT_DIRECTORY}/src/inc/*.h)
file(GLOB_RECURSE HEADERS 	  ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
file(GLOB_RECURSE SOURCES 	  ${CMAKE_CURRENT_SOURCE_DIR}/*.c)

#add_library
add_library(osp_mw_can SHARED ${PUB_HEADERS} ${HEADERS} ${SOURCES})

#add_definitions
add_definitions(-DANY_EXPORT)

if(SANITIZE_ADDRESS)
    add_definitions(-DANY_USE_ASAN)
endif()

# target_link_libraries
target_link_libraries(osp_mw_can osp_drv_can avm)
