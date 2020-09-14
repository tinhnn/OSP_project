#pragma once
#include <inc/osp_rtw/osp_rtw_can.h>
#include <inc/osp_rtw/osp_rtw_types.h>
#include <drv/net/can/can.h>

#ifdef __cplusplus
    extern "C" {
#endif
ANY_API extern osp_rtw_ret_t can_utils_get_drv_by_type(osp_rtw_can_drv_type_t can_type, osp_rtw_drv_can_fops_t** fops);
#ifdef __cplusplus
    }   // extern "C"
#endif
/*EOF*/
