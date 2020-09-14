#ifndef __OSP_RTW_CAN_H__
#define __OSP_RTW_CAN_H__
#include <any/rt_types.h>

#define OSP_RTW_CAN_MAX_DLEN 8
// typedef enum
typedef enum {
    OCT_SOCK = 0,
    OCT_STUB,       // 1
    OCT_USB,        // 2
    OCT_INVALID,
    OCT_MAX
} osp_rtw_can_drv_type_t;

typedef enum {
    OCL_TYPE_CAN = 0,
    OCL_TYPE_CAN_FD,
    OCL_TYPE_CAN_INVALID,
    OCL_TYPE_MAX
} osp_rtw_can_layer_type_t;

typedef enum {
    OSP_RTW_CAN_OPEN_TYPE_A     = 0,
    OSP_RTW_CAN_OPEN_TYPE_B     = 1,
    OSP_RTW_CAN_OPEN_TYPE_AB    = 2,
    OSP_RTW_CAN_OPEN_TYPE_BA    = 3
} osp_rtw_can_open_type_t;
// typedef struct
typedef struct __osp_rtw_can_open_opt_t {
    osp_rtw_can_drv_type_t can_type;
    osp_rtw_can_open_type_t open_type;
    const char* interface;
    const char* channel;
} osp_rtw_can_open_opt_t;

typedef struct __osp_rtw_can_info_t {
    osp_rtw_can_drv_type_t can_type;
    const char* channel;
    const char* interface;
    int pid;
    void* priv_data;
} osp_rtw_can_info_t;

typedef struct __osp_rtw_can_frame_t {
    aint_t can_id;
    aint_t dlc;
    // uint8_t data[OSP_RTW_CAN_MAX_DLEN];
    uint8_t* data;
    //aint_t data[OSP_RTW_CAN_MAX_DLEN];
}osp_rtw_can_frame_t;

typedef struct __osp_rtw_can_opt_t {
    osp_rtw_can_layer_type_t type;
}osp_rtw_can_opt_t;
#endif