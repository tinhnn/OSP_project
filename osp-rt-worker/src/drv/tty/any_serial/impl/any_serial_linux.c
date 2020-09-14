#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "any_serial_platform.h"
#include "any_serial_types.h"
#include "any_serial_linux.h"

#if defined (linux) || defined (__linux) || defined (__linux__)
#define ANY_SERIAL_DEBUG
#ifdef ANY_SERIAL_DEBUG
#   define serial_debug_log(M, ...)    printf("[INFO]    " "%s (%s:%d) "M "\n", __func__, __FILE__, __LINE__, ##__VA_ARGS__);
#else
#   define serial_debug_log do{} while(0)
#endif
ANY_SERIAL_API any_serial_ret_t any_serial_open (const any_serial_open_info_t* open_info, any_serial_obj_t* serial_obj)
{
    any_serial_ret_t ret = ANY_SERIAL_RET_ERROR_NONE;
    serial_debug_log("any_serial_open is called\n");
    return ret;
}
ANY_SERIAL_API any_serial_ret_t any_serial_close (any_serial_obj_t* serial_obj)
{
    any_serial_ret_t ret = ANY_SERIAL_RET_ERROR_NONE;
    serial_debug_log("any_serial_close is called\n");
    return ret;
}
ANY_SERIAL_API int any_serial_write_nonblocking (any_serial_obj_t* serial_obj, const uint8_t* buf, const uint16_t size)
{
    any_serial_ret_t ret = ANY_SERIAL_RET_ERROR_NONE;
    serial_debug_log("any_serial_write_nonblocking is called\n");
    return ret;
}
ANY_SERIAL_API int any_serial_write_blocking(any_serial_obj_t* serial_obj, const uint8_t* buf, const uint16_t size, unsigned int timeout_ms)
{
    any_serial_ret_t ret = ANY_SERIAL_RET_ERROR_NONE;
    serial_debug_log("any_serial_write_blocking is called\n");
    return ret;
}
ANY_SERIAL_API any_serial_ret_t any_serial_read (any_serial_obj_t* serial_obj)
{
    any_serial_ret_t ret = ANY_SERIAL_RET_ERROR_NONE;
    serial_debug_log("any_serial_read is called\n");
    return ret;
}
#endif // #if defined (_LINUX)