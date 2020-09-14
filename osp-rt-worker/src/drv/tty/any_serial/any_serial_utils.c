#include "inc/any_serial_utils.h"

extern any_serial_fops_t any_serial_fops;
ANY_SERIAL_API any_serial_ret_t any_serial_get_fops(any_serial_fops_t** fops)
{
    any_serial_ret_t ret = ANY_SERIAL_RET_ERROR_NONE;
    if(NULL == fops) {
        return ANY_SERIAL_RET_ERROR_INVALID;
    }
    *fops = &any_serial_fops;
    return ret;
}
