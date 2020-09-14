#include_directories
include_directories(${AVM_ROOT_DIRECTORY}/src/inc)
include_directories(${OSP_RT_WORKER_EXTS_DIR})
include_directories(${OSP_RT_WORKER_ROOT_DIRECTORY}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

#add_subdirectory
# add_subdirectory(lawicel)

#file headers
file(GLOB_RECURSE PUB_HEADERS ${AVM_ROOT_DIRECTORY}/src/inc/*.h)
file(GLOB_RECURSE HEADERS 	  ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
file(GLOB_RECURSE LAWICEL_HEADERS     ${CMAKE_CURRENT_SOURCE_DIR}/lawicel/*.h)
file(GLOB_RECURSE DRV_TTY_HEADERS ${OSP_RT_WORKER_ROOT_DIRECTORY}/src/drv/tty/any_serial/inc/*.h)

#file sources
file(GLOB_RECURSE SOURCES 	  ${CMAKE_CURRENT_SOURCE_DIR}/*.c)
file(GLOB_RECURSE LAWICEL_SOURCES     ${CMAKE_CURRENT_SOURCE_DIR}/lawicel/*.c)
#list driver headers
list(APPEND HEADERS 
    ${PUB_HEADERS}
    ${DRV_TTY_HEADERS}
    ${LAWICEL_HEADERS})

#list driver sources
list(APPEND SOURCES
    ${LAWICEL_SOURCES})

#add_library
add_library(osp_drv_can SHARED ${HEADERS} ${SOURCES})

#add_definitions
add_definitions(-DANY_EXPORT)
if(WIN32)
    add_definitions(-DANY_SERIAL_MSVC)
endif()
add_definitions(-DANY_SERIAL_SHARED)

if(SANITIZE_ADDRESS)
    add_definitions(-DANY_USE_ASAN)
endif()

target_link_libraries(osp_drv_can 
                        avm
                        any_serial)
