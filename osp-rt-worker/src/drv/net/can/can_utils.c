#include <stdio.h>
#include <lwlog/lwlog.h>
#include "can_utils.h"

//#define OSP_RTW_DRV_CAN_REGISTER(DRV, drv) extern osp_rtw_drv_can_fops_t osp_rtw_can_drv_##drv##;

//OSP_RTW_DRV_CAN_REGISTER(_RESERVE_FOR_FUTURE, can_sock);
//OSP_RTW_DRV_CAN_REGISTER(_RESERVE_FOR_FUTURE, can_stub);

extern osp_rtw_drv_can_fops_t osp_rtw_can_drv_can_sock;
extern osp_rtw_drv_can_fops_t osp_rtw_can_drv_can_stub;
extern osp_rtw_drv_can_fops_t osp_rtw_can_drv_can_usb;

static osp_rtw_drv_can_fops_t* can_mw_drv_list[OCT_MAX] = {
    &osp_rtw_can_drv_can_sock,
    &osp_rtw_can_drv_can_stub,
    &osp_rtw_can_drv_can_usb,
    NULL,
};
osp_rtw_ret_t can_utils_get_drv_by_type(osp_rtw_can_drv_type_t can_type, osp_rtw_drv_can_fops_t** fops)
{
    lwlog_info("can_utils_get_drv_by_type is called\n");
    osp_rtw_ret_t ret = OSP_RTW_RET_ERROR_NONE;
    if(can_type >= OCT_INVALID || NULL == fops) {
        lwlog_err("invalid can_type\n");
        return OSP_RTW_RET_ERROR_INVALID;
    }
    *fops = can_mw_drv_list[can_type];
    return ret;
}