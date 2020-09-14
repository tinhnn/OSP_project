#pragma once

#if defined (linux) || defined (__linux) || defined (__linux__)
typedef struct {
    uint8_t _;  //TODO
}any_serial_obj_t;
typedef void (*any_serial_write_cb) (void* param);
ANY_SERIAL_API any_serial_ret_t any_serial_open (const any_serial_open_info_t* open_info, any_serial_obj_t* serial_obj);
ANY_SERIAL_API any_serial_ret_t any_serial_close (any_serial_obj_t* serial_obj);
ANY_SERIAL_API int any_serial_write_nonblocking (any_serial_obj_t* serial_obj, const uint8_t* buf, const uint16_t size);
ANY_SERIAL_API int any_serial_write_blocking(any_serial_obj_t* serial_obj, const uint8_t* buf, const uint16_t size, unsigned int timeout_ms);
ANY_SERIAL_API any_serial_ret_t any_serial_read (any_serial_obj_t* serial_obj);
#else
#   error "Must be compiled in Windows environment"
#endif
/*EOF*/