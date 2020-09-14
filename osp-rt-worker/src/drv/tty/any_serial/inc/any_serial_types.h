#pragma once
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "any_serial_platform.h"
typedef enum {
    ANY_SERIAL_RET_ERROR_NONE       = 0,
    ANY_SERIAL_RET_ERROR_INVALID    = -1,
    ANY_SERIAL_RET_ERROR_NOMEM      = -2,
    ANY_SERIAL_RET_ERROR_EFAULT     = -3,
    ANY_SERIAL_RET_ERROR_TIMEOUT    = -4,
    ANY_SERIAL_RET_ERROR_UNKNOWN    = -5
} any_serial_ret_t;

typedef enum {
    ANY_SERIAL_FIVEBITS = 5,
    ANY_SERIAL_SIXBITS = 6,
    ANY_SERIAL_SEVENBITS = 7,
    ANY_SERIAL_EIGHTBITS = 8
} any_serial_bytesize_t;

typedef enum {
    ANY_SERIAL_PARITY_NONE = 0,
    ANY_SERIAL_PARITY_ODD = 1,
    ANY_SERIAL_PARITY_EVEN = 2,
    ANY_SERIAL_PARITY_MARK = 3,
    ANY_SERIAL_PARITY_SPACE = 4
} any_serial_parity_t;

#if 0
typedef enum {
    PARITY_NONE = 0,
    PARITY_ODD = 1,
    PARITY_EVEN = 2,
    ARITY_MARK = 3,
    ARITY_SPACE = 4
} any_serial_parity_t;
#endif

typedef enum {
    ANY_SERIAL_STOPBITS_ONE = 1,
    ANY_SERIAL_STOPBITS_TWO = 2,
    ANY_SERIAL_STOPBITS_ONE_POINT_FIVE
} any_serial_stopbits_t;
typedef enum {
    ANY_SERIAL_FLOWCONTROL_NONE = 0,
    ANY_SERIAL_FLOWCONTROL_SOFTWARE,
    ANY_SERIAL_FLOWCONTROL_HARDWARE
} any_serial_flowcontrol_t;
typedef struct {
    int _reserved;
} any_serial_timeout_t;

typedef struct {
    char port[16]; // we fix the maximum path is 16. TODO
    any_serial_timeout_t* timeout;
    uint32_t baudrate;
    any_serial_bytesize_t bytesize;
    any_serial_parity_t parity;
    any_serial_stopbits_t stopbits;
    any_serial_flowcontrol_t flowcontrol;
} any_serial_open_info_t;

/*EOF*/