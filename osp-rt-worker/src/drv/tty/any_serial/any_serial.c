#include "inc/any_serial.h"
any_serial_fops_t any_serial_fops = {
    .open = any_serial_open,
    .close = any_serial_close,
    .write_nonblocking = any_serial_write_nonblocking,
    .write_blocking = any_serial_write_blocking,
    .read = any_serial_read,
};
/*EOF*/