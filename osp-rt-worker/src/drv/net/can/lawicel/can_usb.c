#include <stdio.h>

#include <lwlog/lwlog.h>
#include "can.h"
#include <drv/tty/any_serial/inc/any_serial.h>
#include <drv/tty/any_serial/inc/any_serial_utils.h>
#if defined _WIN32 || defined(__WIN32__) || defined(_WIN32) || defined(_MSC_VER)
// static function declare
static osp_rtw_ret_t can_usb_open(osp_rtw_can_open_opt_t* open_opt, osp_rtw_can_info_t* can_info);
static osp_rtw_ret_t can_usb_close(osp_rtw_can_info_t* can_info);
static osp_rtw_ret_t can_usb_send(osp_rtw_can_info_t* can_info, osp_rtw_can_frame_t* can_frame);
// static function implement
static osp_rtw_ret_t can_usb_open(osp_rtw_can_open_opt_t* open_opt, osp_rtw_can_info_t* can_info)
{
    osp_rtw_ret_t ret = OSP_RTW_RET_ERROR_NONE;
    lwlog_info("can_usb_open is called\n");
    can_info->can_type = OCT_USB;
    can_info->interface = open_opt->interface;
    can_info->channel = open_opt->channel;
    can_info->pid = 1234;

    lwlog_info("==> can_info->can_type: [%d]\n", can_info->can_type);
    lwlog_info("==> can_info->interface: [%s]\n", can_info->interface);
    lwlog_info("==> can_info->pid: [%d]\n", can_info->pid);

    any_serial_fops_t* any_serial_fops;
    any_serial_get_fops(&any_serial_fops);
    any_serial_open_info_t open_info;
    open_info.baudrate = 9600;
    open_info.bytesize = ANY_SERIAL_EIGHTBITS;
    open_info.flowcontrol = ANY_SERIAL_FLOWCONTROL_NONE;
    open_info.parity = ANY_SERIAL_PARITY_NONE;
    open_info.stopbits = ANY_SERIAL_STOPBITS_ONE;
    open_info.timeout = NULL;
    //  Current we set interface to port name. TODO support separate port name and interface 
    sprintf_s(open_info.port, sizeof(open_info.port)/sizeof(open_info.port[0]), "%s", can_info->interface);

    can_info->priv_data = malloc(sizeof(any_serial_obj_t));
    any_serial_obj_t* p_any_serial_obj;
    p_any_serial_obj = (any_serial_obj_t*) can_info->priv_data;
    p_any_serial_obj->fd = INVALID_HANDLE_VALUE;
    p_any_serial_obj->is_open = false;
    any_serial_ret_t r;
    r = any_serial_fops->open(&open_info, p_any_serial_obj);
    if (ANY_SERIAL_RET_ERROR_NONE == r) {
        // setting specified for Lawicel and Open the channel
        uint8_t conf_data[50];
        
        // configure bitrate
        sprintf_s(conf_data, 50, "S4\r");
        //any_serial_fops->write_nonblocking(p_any_serial_obj, conf_data, strlen(conf_data));
        any_serial_fops->write_blocking(p_any_serial_obj, conf_data, strlen(conf_data), 1000);
        // open the channel
        sprintf_s(conf_data, 50, "O\r");
        //any_serial_fops->write_nonblocking(p_any_serial_obj, conf_data, strlen(conf_data));
        any_serial_fops->write_blocking(p_any_serial_obj, conf_data, strlen(conf_data), 1000);
    }
    return ret;
}
static osp_rtw_ret_t can_usb_close(osp_rtw_can_info_t* can_info)
{
    lwlog_info("can_usb_close is called\n");
    osp_rtw_ret_t ret = OSP_RTW_RET_ERROR_NONE;
    lwlog_info("can_info.can_type [%d]\n", can_info->can_type);
    lwlog_info("can_info.interface [%s]\n", can_info->interface);
    lwlog_info("can_info.pid [%d]\n", can_info->pid);
    if (NULL != can_info->priv_data) {
        any_serial_fops_t* any_serial_fops;
        any_serial_obj_t* p_any_serial_obj = (any_serial_obj_t*)can_info->priv_data;
        any_serial_ret_t r;
        any_serial_get_fops(&any_serial_fops);

        //  Close the channel
        uint8_t conf_data[50];
        sprintf_s(conf_data, 50, "C\r");
        //any_serial_fops->write_nonblocking(p_any_serial_obj, conf_data, strlen(conf_data));
        any_serial_fops->write_blocking(p_any_serial_obj, conf_data, strlen(conf_data), 1000);
        // Close the serial port.
        r = any_serial_fops->close(p_any_serial_obj);
        free(can_info->priv_data);
        can_info->priv_data = NULL;
    }
    return ret;
}
static osp_rtw_ret_t can_usb_send(osp_rtw_can_info_t* can_info, osp_rtw_can_frame_t* can_frame)
{
    int i=0;
    lwlog_info("can_usb_send is called\n");
    osp_rtw_ret_t ret = OSP_RTW_RET_ERROR_NONE;
    lwlog_info("can_usb_send can_type: [%d]\n", can_info->can_type);
    lwlog_info("can_usb_send interface [%s]\n", can_info->interface);
    lwlog_info("can_usb_send pid [%d]\n", can_info->pid);
    lwlog_info("can_usb_send priv_data [%d]\n", can_info->priv_data);
    lwlog_info("can_usb_send can_id: [%lld]\n", can_frame->can_id);
    lwlog_info("can_usb_send dlc: [%lld]\n", can_frame->dlc);
    for(i = 0; i< can_frame->dlc; i++) {
        lwlog_info("can_usb_send data [%d]: [%d]\n", i, can_frame->data[i]);
    }
    any_serial_fops_t* any_serial_fops;
    any_serial_ret_t r;
    any_serial_get_fops(&any_serial_fops);

    any_serial_obj_t* p_any_serial_obj;
    p_any_serial_obj = (any_serial_obj_t*)can_info->priv_data;
    
    uint8_t conf_data[50];
    //sprintf_s(conf_data, 50, "t100211333\r");
    sprintf_s(conf_data, 50, "t%03x%01d",(uint32_t)can_frame->can_id,(uint8_t)can_frame->dlc);
    for (i = 0; i < can_frame->dlc; i++) {
        sprintf_s(conf_data, 50, "%s%02x", conf_data, can_frame->data[i]);
    }
    sprintf_s(conf_data, 50, "%s\r", conf_data);
    //any_serial_fops->write_nonblocking(p_any_serial_obj, conf_data, strlen(conf_data));
    any_serial_fops->write_blocking(p_any_serial_obj, conf_data, strlen(conf_data), 1000);
    lwlog_info("can_usb_send is ended!\n");
    return ret;
}
#elif defined (linux) || defined (__linux) || defined (__linux__)
// static function declare
static osp_rtw_ret_t can_usb_open(osp_rtw_can_open_opt_t* open_opt, osp_rtw_can_info_t* can_info);
static osp_rtw_ret_t can_usb_close(osp_rtw_can_info_t* can_info);
static osp_rtw_ret_t can_usb_send(osp_rtw_can_info_t* can_info, osp_rtw_can_frame_t* can_frame);
// static function implement
static osp_rtw_ret_t can_usb_open(osp_rtw_can_open_opt_t* open_opt, osp_rtw_can_info_t* can_info)
{
    osp_rtw_ret_t ret = OSP_RTW_RET_ERROR_NONE;
    lwlog_info("can_usb_open is called\n");
    return ret;
}
static osp_rtw_ret_t can_usb_close(osp_rtw_can_info_t* can_info)
{
    lwlog_info("can_usb_close is called\n");
    osp_rtw_ret_t ret = OSP_RTW_RET_ERROR_NONE;
    return ret;
}
static osp_rtw_ret_t can_usb_send(osp_rtw_can_info_t* can_info, osp_rtw_can_frame_t* can_frame)
{
    lwlog_info("can_usb_send is called\n");
    osp_rtw_ret_t ret = OSP_RTW_RET_ERROR_NONE;
    int i=0;
    return ret;
}
#else

#endif
osp_rtw_drv_can_fops_t osp_rtw_can_drv_can_usb = {
    .type = OCT_USB,
    .open = can_usb_open,
    .close = can_usb_close,
    .send = can_usb_send,
};
/*EOF*/