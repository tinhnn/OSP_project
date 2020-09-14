#pragma once
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "any_serial_platform.h"
#include "any_serial_types.h"
#if defined _WIN32 || defined(__WIN32__) || defined(_WIN32) || defined(_MSC_VER)
#include "any_serial_win.h"
#elif defined (linux) || defined (__linux) || defined (__linux__)
#include "any_serial_linux.h"
#endif

struct any_serial_obj_t;
typedef struct {
    any_serial_ret_t (*open) (const any_serial_open_info_t* open_info, any_serial_obj_t* serial_obj);
    any_serial_ret_t (*close) (any_serial_obj_t* serial_obj);
    int (*write_nonblocking) (any_serial_obj_t* serial_obj, const uint8_t* buf, const uint16_t size);
    int (*write_blocking) (any_serial_obj_t* serial_obj, const uint8_t* buf, const uint16_t size, unsigned int timeout_ms);
    any_serial_ret_t (*read) (any_serial_obj_t* serial_obj);
} any_serial_fops_t;

// extern any_serial_fops_t any_serial_fops;
// any_serial_ret_t any_serial_create_new()
/*EOF*/