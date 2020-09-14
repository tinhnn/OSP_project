#pragma once
#if defined _WIN32 || defined(__WIN32__) || defined(_WIN32) || defined(_MSC_VER)
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "any_serial_platform.h"
#include "any_serial_types.h"
#include "windows.h"
#define ANY_SERIAL_OVERLAPPED
#ifdef __cplusplus
extern "C" {
#endif
typedef struct {
    HANDLE fd;
    bool is_open;
    any_serial_open_info_t opened_info;

    // Mutex used to lock the write functions
    //HANDLE write_mutex;
#ifdef ANY_SERIAL_OVERLAPPED
    //HANDLE	m_hevtOverlapped;	// Event handle for internal overlapped operations
    OVERLAPPED ovl_write;
#endif
    bool is_writing;
}any_serial_obj_t;
typedef void (*any_serial_write_cb) (void* param);
ANY_SERIAL_API any_serial_ret_t any_serial_open (const any_serial_open_info_t* open_info, any_serial_obj_t* serial_obj);
ANY_SERIAL_API any_serial_ret_t any_serial_close (any_serial_obj_t* serial_obj);
ANY_SERIAL_API int any_serial_write_nonblocking (any_serial_obj_t* serial_obj, const uint8_t* buf, const uint16_t size);
ANY_SERIAL_API int any_serial_write_blocking(any_serial_obj_t* serial_obj, const uint8_t* buf, const uint16_t size, unsigned int timeout_ms);
ANY_SERIAL_API any_serial_ret_t any_serial_read (any_serial_obj_t* serial_obj);
#ifdef __cplusplus
} // extern "C"
#endif
#else
#   error "Must be compiled in Windows environment"
#endif
/*EOF*/