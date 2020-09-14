#pragma once
#include <any/rt_types.h>
#include <inc/osp_rtw/osp_rtw_types.h>
#include <inc/osp_rtw/osp_rtw_can.h>

typedef struct {
    osp_rtw_can_drv_type_t type;
    osp_rtw_ret_t (*open)(osp_rtw_can_open_opt_t*, osp_rtw_can_info_t*);
    osp_rtw_ret_t (*close)(osp_rtw_can_info_t*);
    osp_rtw_ret_t (*send)(osp_rtw_can_info_t*, osp_rtw_can_frame_t*);
    osp_rtw_ret_t(*setopt)(osp_rtw_can_info_t*, osp_rtw_can_opt_t*);
} osp_rtw_drv_can_fops_t;
#ifdef __cplusplus
    extern "C" {
#endif
ANY_API osp_rtw_ret_t osp_rtw_drv_can_register_all(void);
#ifdef __cplusplus
    }   // extern "C"
#endif
/*EOF*/
