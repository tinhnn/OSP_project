#include <stdio.h>

#include <lwlog/lwlog.h>
#include "can.h"

// static function declare
static osp_rtw_ret_t can_stub_open(osp_rtw_can_open_opt_t* open_opt, osp_rtw_can_info_t* can_info);
static osp_rtw_ret_t can_stub_close(osp_rtw_can_info_t* can_info);
static osp_rtw_ret_t can_stub_send(osp_rtw_can_info_t* can_info, osp_rtw_can_frame_t* can_frame);
// static function implement
static osp_rtw_ret_t can_stub_open(osp_rtw_can_open_opt_t* open_opt, osp_rtw_can_info_t* can_info)
{
    osp_rtw_ret_t ret = OSP_RTW_RET_ERROR_NONE;
    lwlog_info("can_stub_open is called\n");
    can_info->can_type = OCT_STUB;
    can_info->interface = open_opt->interface;
    can_info->pid = 1234;
    //can_info->priv_data = NULL;
    can_info->priv_data = malloc(10);
    sprintf(can_info->priv_data, "=>abc");

    lwlog_info("==> can_info->can_type: [%d]\n", can_info->can_type);
    lwlog_info("==> can_info->interface: [%s]\n", can_info->interface);
    lwlog_info("==> can_info->pid: [%d]\n", can_info->pid);
    lwlog_info("==> can_info->priv: [%d]\n", can_info->priv_data);
    return ret;
}
static osp_rtw_ret_t can_stub_close(osp_rtw_can_info_t* can_info)
{
    lwlog_info("can_stub_close is called\n");
    osp_rtw_ret_t ret = OSP_RTW_RET_ERROR_NONE;
    lwlog_info("can_info.can_type [%d]\n", can_info->can_type);
    lwlog_info("can_info.interface [%s]\n", can_info->interface);
    lwlog_info("can_info.pid [%d]\n", can_info->pid);
    if (NULL != can_info->priv_data) {
        free(can_info->priv_data);
    }
    return ret;
}
static osp_rtw_ret_t can_stub_send(osp_rtw_can_info_t* can_info, osp_rtw_can_frame_t* can_frame)
{
    int i=0;
    lwlog_info("can_stub_send is called\n");
    osp_rtw_ret_t ret = OSP_RTW_RET_ERROR_NONE;
    lwlog_info("can_stub_send can_type: [%d]\n", can_info->can_type);
    lwlog_info("can_stub_send interface [%s]\n", can_info->interface);
    lwlog_info("can_stub_send pid [%d]\n", can_info->pid);
    lwlog_info("can_stub_send priv_data [%s]\n", (const char*)can_info->priv_data);
    lwlog_info("can_stub_send can_id: [%lld]\n", can_frame->can_id);
    lwlog_info("can_stub_send dlc: [%lld]\n", can_frame->dlc);
    for(i = 0; i< can_frame->dlc; i++) {
        lwlog_info("can_stub_send data [%d]: [%d]\n", i, can_frame->data[i]);
    }
    lwlog_info("can_stub_send is ended!\n");
    return ret;
}

osp_rtw_drv_can_fops_t osp_rtw_can_drv_can_stub = {
    .type = OCT_STUB,
    .open = can_stub_open,
    .close = can_stub_close,
    .send = can_stub_send,
};
/*EOF*/
