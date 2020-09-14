#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <poll.h>
#include <ctype.h>
#include <libgen.h>
#include <time.h>
#include <errno.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/uio.h>
#include <net/if.h>

#include <linux/can.h>
#include <linux/can/raw.h>

#include <lwlog/lwlog.h>
#include "can.h"

#if defined _WIN32 || defined(__WIN32__) || defined(_WIN32) || defined(_MSC_VER)
// static function declare
static osp_rtw_ret_t can_sock_open(osp_rtw_can_open_opt_t* open_opt, osp_rtw_can_info_t* can_info);
static osp_rtw_ret_t can_sock_close(osp_rtw_can_info_t* can_info);
static osp_rtw_ret_t can_sock_send(osp_rtw_can_info_t* can_info, osp_rtw_can_frame_t* can_frame);
// static function implement
static osp_rtw_ret_t can_sock_open(osp_rtw_can_open_opt_t* open_opt, osp_rtw_can_info_t* can_info)
{
    osp_rtw_ret_t ret = OSP_RTW_RET_ERROR_NONE;
    lwlog_info("can_sock_open is called\n");
    can_info->can_type = OCT_STUB;
    can_info->interface = open_opt->interface;
    can_info->pid = 1234;
    can_info->priv_data = NULL;
    return ret;
}
static osp_rtw_ret_t can_sock_close(osp_rtw_can_info_t* can_info)
{
    lwlog_info("can_sock_close is called\n");
    osp_rtw_ret_t ret = OSP_RTW_RET_ERROR_NONE;
    lwlog_info("can_info.can_type [%d]\n", can_info->can_type);
    lwlog_info("can_info.interface [%s]\n", can_info->interface);
    lwlog_info("can_info.pid [%d]\n", can_info->pid);
    return ret;
}
static osp_rtw_ret_t can_sock_send(osp_rtw_can_info_t* can_info, osp_rtw_can_frame_t* can_frame)
{
    lwlog_info("can_sock_send is called\n");
    osp_rtw_ret_t ret = OSP_RTW_RET_ERROR_NONE;
    lwlog_info("can_sock_send can_type: [%d]\n", can_info->can_type);
    lwlog_info("can_sock_send interface [%s]\n", can_info->interface);
    lwlog_info("can_sock_send pid [%d]\n", can_info->pid);
    lwlog_info("can_sock_send can_id: [%d]\n", can_frame->can_id);
    lwlog_info("can_sock_send dlc: [%d]\n", can_frame->dlc);
    lwlog_info("can_sock_send data: [%s]\n", can_frame->data);
    lwlog_info("can_sock_send is ended!\n");
    return ret;
}
#elif defined (linux) || defined (__linux) || defined (__linux__)
#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <linux/can.h>
#include <linux/can/raw.h>
typedef struct __can_sock_info_t {
    int s;
    struct pollfd fds;
    unsigned long polltimeout;
} can_sock_info_t;
// static function declare
static osp_rtw_ret_t can_sock_open(osp_rtw_can_open_opt_t* open_opt, osp_rtw_can_info_t* can_info);
static osp_rtw_ret_t can_sock_close(osp_rtw_can_info_t* can_info);
static osp_rtw_ret_t can_sock_send(osp_rtw_can_info_t* can_info, osp_rtw_can_frame_t* can_frm);
// static function implement
static osp_rtw_ret_t can_sock_open(osp_rtw_can_open_opt_t* open_opt, osp_rtw_can_info_t* can_info)
{
    lwlog_info("can_sock_open is called\n");
    osp_rtw_ret_t ret = OSP_RTW_RET_ERROR_NONE;
    struct ifreq ifr;
    struct sockaddr_can addr;
    can_sock_info_t* can_sock_info = (can_sock_info_t*)malloc(sizeof(can_sock_info_t));
    if(NULL == can_sock_info) {
        lwlog_err("cannot allocate resources\n");
        ret = OSP_RTW_RET_ERROR_NOMEM;
        goto result;
    }
    can_sock_info->s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    can_sock_info->polltimeout = 1000;
    can_sock_info->fds.fd = can_sock_info->s;
    can_sock_info->fds.events = POLLOUT;
    if(can_sock_info->s < 0) {
        lwlog_err("cannot open socket\n");
        ret = OSP_RTW_RET_ERROR_EFAULT;
        goto result;
    }
    //strcpy(ifr.ifr_name, ifname);
    //ioctl(s, SIOCGIFINDEX, &ifr);
    strncpy(ifr.ifr_name, open_opt->interface, IFNAMSIZ - 1);
    ifr.ifr_name[IFNAMSIZ - 1] = '\0';
    ifr.ifr_ifindex = if_nametoindex(ifr.ifr_name);
    if(!ifr.ifr_ifindex) {
        lwlog_err("cannot initialize interface\n");
        ret = OSP_RTW_RET_ERROR_EFAULT;
        goto result;
    }

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    setsockopt(can_sock_info->s, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);
    if(bind(can_sock_info->s, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        lwlog_err("cannot bind socket to address\n");
        ret = OSP_RTW_RET_ERROR_EFAULT;
        goto result;
    }
#if 0
#include <sys/socket.h>
    int sendbuff;
    socklen_t optlen;
    optlen = sizeof(sendbuff);
    int res = getsockopt(can_sock_info->s, SOL_SOCKET, SO_SNDBUF, &sendbuff, &optlen);
    if(res == -1)
        printf("Error getsockopt one");
    else
        printf("send buffer size = %d\n", sendbuff);
    // getchar();
#endif
result:
    if(ret == OSP_RTW_RET_ERROR_NONE) {
        can_info->can_type = OCT_SOCK;
        can_info->interface = open_opt->interface;
        can_info->pid = 1234;
        can_info->priv_data = can_sock_info;
        can_info->channel = open_opt->channel;
    } else {
        lwlog_err("Error !!!\n");
        if(NULL != can_sock_info) {
            free(can_sock_info);
        }
    }
    return ret;
}
static osp_rtw_ret_t can_sock_close(osp_rtw_can_info_t* can_info)
{
    lwlog_info("can_sock_close is called\n");
    osp_rtw_ret_t ret = OSP_RTW_RET_ERROR_NONE;
    can_sock_info_t* can_sock_info;
    lwlog_info("can_info.can_type [%d]\n", can_info->can_type);
    lwlog_info("can_info.interface [%s]\n", can_info->interface);
    lwlog_info("can_info.pid [%d]\n", can_info->pid);
    can_sock_info = (can_sock_info_t*)(can_info->priv_data);
    if(NULL == can_sock_info) {
        // ret = OSP_RTW_RET_ERROR_NONE;
        goto result;
    }
    if(can_sock_info->s > 0) {
        close(can_sock_info->s);
    }
    free(can_info->priv_data);
result:
    return ret;
}
static osp_rtw_ret_t can_sock_send(osp_rtw_can_info_t* can_info, osp_rtw_can_frame_t* can_frm)
{
    lwlog_info("can_sock_send is called\n");
    osp_rtw_ret_t ret = OSP_RTW_RET_ERROR_NONE;
    can_sock_info_t* can_sock_info;
    struct can_frame can_sock_frame;
    struct pollfd fds;
    int write_ret;
    lwlog_info("can_sock_send can_type: [%d]\n", can_info->can_type);
    lwlog_info("can_sock_send interface [%s]\n", can_info->interface);
    lwlog_info("can_sock_send pid [%d]\n", can_info->pid);
    lwlog_info("can_sock_send can_id: [%lld]\n", can_frm->can_id);
    lwlog_info("can_sock_send dlc: [%lld]\n", can_frm->dlc);
    lwlog_info("can_sock_send data: [%d]\n", can_frm->data[0]);
    //lwlog_info("can_sock_send is ended!\n");
    can_sock_info = (can_sock_info_t*)(can_info->priv_data);
    if(NULL == can_sock_info) {
        lwlog_err("invalid can_info\n");
        goto result;
    }
    can_sock_frame.can_id   = can_frm->can_id;
    can_sock_frame.can_dlc  = can_frm->dlc;
    memcpy(can_sock_frame.data, can_frm->data, CAN_MAX_DLEN);
    do {
        write_ret = write(can_sock_info->s, &can_sock_frame, sizeof(struct can_frame));
        if(write_ret < 0) {
            lwlog_err("send error [%d] [%d]\n", write_ret, sizeof( struct can_frame));
            if(can_sock_info->polltimeout > 0) {
                if(poll(&can_sock_info->fds, 1, can_sock_info->polltimeout) < 0) {
                    lwlog_err("write poll error\n");
                    ret = OSP_RTW_RET_ERROR_EFAULT;
                    goto result;
                } else {
                    lwlog_info("CONTINUE\n");
                    continue;
                }
            } else {
                ret = OSP_RTW_RET_ERROR_EFAULT;
                goto result;
            }
        } else if(write_ret > 0 && write_ret != sizeof(struct can_frame) ){
            lwlog_err("send error [%d] [%d]\n", write_ret, sizeof( struct can_frame));
            ret = OSP_RTW_RET_ERROR_EFAULT;
            goto result;
        } else {
            ret = OSP_RTW_RET_ERROR_NONE;
            goto result;
        }
    } while(1);
result:
    lwlog_info("can_sock_send ended\n");
    return ret;
}
#else
// static function declare
static osp_rtw_ret_t can_sock_open(osp_rtw_can_open_opt_t* open_opt, osp_rtw_can_info_t* can_info);
static osp_rtw_ret_t can_sock_close(osp_rtw_can_info_t* can_info);
static osp_rtw_ret_t can_sock_send(osp_rtw_can_info_t* can_info);
// static function implement
static osp_rtw_ret_t can_sock_open(osp_rtw_can_open_opt_t* open_opt, osp_rtw_can_info_t* can_info)
{
    osp_rtw_ret_t ret = OSP_RTW_RET_ERROR_NONE;
    lwlog_info("can_sock_open is called\n");
    can_info->can_type = OCT_STUB;
    can_info->interface = open_opt->interface;
    can_info->pid = 1234;
    can_info->priv_data = NULL;
    return ret;
}
static osp_rtw_ret_t can_sock_close(osp_rtw_can_info_t* can_info)
{
    lwlog_info("can_sock_close is called\n");
    osp_rtw_ret_t ret = OSP_RTW_RET_ERROR_NONE;
    lwlog_info("can_info.can_type [%d]\n", can_info->can_type);
    lwlog_info("can_info.interface [%s]\n", can_info->interface);
    lwlog_info("can_info.pid [%d]\n", can_info->pid);
    return ret;
}
static osp_rtw_ret_t can_sock_send(osp_rtw_can_info_t* can_info, osp_rtw_can_frame_t* can_frame)
{
    lwlog_info("can_sock_send is called\n");
    osp_rtw_ret_t ret = OSP_RTW_RET_ERROR_NONE;
    lwlog_info("can_sock_send can_type: [%d]\n", can_info->can_type);
    lwlog_info("can_sock_send interface [%s]\n", can_info->interface);
    lwlog_info("can_sock_send pid [%d]\n", can_info->pid);
    lwlog_info("can_sock_send can_id: [%d]\n", can_frame->can_id);
    lwlog_info("can_sock_send dlc: [%d]\n", can_frame->dlc);
    lwlog_info("can_sock_send data: [%s]\n", can_frame->data);
    lwlog_info("can_sock_send is ended!\n");
    return ret;
}
#endif
osp_rtw_drv_can_fops_t osp_rtw_can_drv_can_sock = {
    .type = OCT_SOCK,
    .open = can_sock_open,
    .close = can_sock_close,
    .send = can_sock_send,
};
/*EOF*/
