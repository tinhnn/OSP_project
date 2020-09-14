#pragma once
// #ifndef __CAN_EG_H__
// #   define __CAN_EG_H__
#include <any/platform.h>
#ifdef __cplusplus
    extern "C" {
#endif
ANY_API void anet_lib_add_can_eg(aloader_t* l, void* ud);

#ifdef __cplusplus
    } // extern "C"
#endif
// #endif