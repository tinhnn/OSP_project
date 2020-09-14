#pragma once
#include <any/platform.h>
#if 0
typedef enum {
    RET_ERROR_NONE = 0,
    RET_ERROR_UNKNOWN
} can_mw_ret_t;
#endif
typedef enum {
    CAN_TYPE_SOCK = 0,
    CAN_TYPE_STUB,
    CAN_TYPE_UNKNOWN
}can_type_t;
#ifdef __cplusplus
    extern "C" {
#endif
ANY_API void anet_lib_add_gen_can_mw(aloader_t* l, void* ud);
#ifdef __cplusplus
    }   // extern "C"
#endif