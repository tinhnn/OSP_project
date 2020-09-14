#include <any/std_io.h>

#include <any/actor.h>
#include <any/loader.h>
#include <any/std_string.h>
#include <any/rt_types.h>
#include <any/std_tuple.h>
#include <any/std_buffer.h>
#include <stdio.h>
#include <lwlog/lwlog.h>
#include "gen_can_mw.h"
#include "can_mw_cord.h"

#include "drv/net/can/can.h"
#include "drv/net/can/can_utils.h"

// functions declare
#define CAN_INFO_FILL_INVALID(can_info) do {        \
        can_info->can_type = OCT_INVALID;           \
        can_info->interface = "";                   \
        can_info->channel = "";                     \
        can_info->pid = -1;                         \
        can_info->priv_data = NULL;                 \
    } while(0);

static void linitialize(aactor_t* a);
static void lfinalize(aactor_t* a);

static void lopen(aactor_t* a);
static void lclose(aactor_t* a);
static void lsend(aactor_t* a);
static void lsubcribe(aactor_t* a);
static void lunsubcribe(aactor_t* a);
static void lget_sent_history(aactor_t* a);
static void lsetopt(aactor_t* a);

static osp_rtw_ret_t get_can_mw_open_info_from_avm_stack(aactor_t* a, aint_t stack_idx, osp_rtw_can_open_opt_t* open_info);
static osp_rtw_ret_t get_can_mw_info_from_avm_stack(aactor_t* a, aint_t stack_idx, osp_rtw_can_info_t* can_mw_info);
static osp_rtw_ret_t get_can_frame_from_avm_stack(aactor_t* a, aint_t stack_idx, osp_rtw_can_frame_t* can_frame);

// static variables declare
static alib_func_t funcs[] = {
    {"initialize/0", linitialize},
    {"finalize/0", lfinalize},
    {"open/4", &lopen },
    {"close/1", &lclose },
    {"send/2", &lsend},
    {"setopt/2", &lsetopt},
    {"subcribe/2", &lsubcribe},
    {"unsubcribe/2", &lunsubcribe},
    // {"get_list_interfaces/1", &lget_list_interfaces},
    {"get_sent_history/1", &lget_sent_history},
    { NULL, NULL }
};
static alib_t mod = {"gen_can_mw", funcs};

static void* m_ud = NULL;
static osp_rtw_ret_t get_can_mw_open_info_from_avm_stack(aactor_t* a, aint_t stack_idx, osp_rtw_can_open_opt_t* open_info)
{
    lwlog_info("get_can_mw_open_info_from_avm_stack is called\n");
    osp_rtw_ret_t ret = OSP_RTW_RET_ERROR_NONE;
    if (NULL == open_info) {
        return OSP_RTW_RET_ERROR_INVALID;
    }
    aint_t a_channel_idx = any_check_index(a, stack_idx - 3);
    aint_t a_open_type_idx = any_check_index(a, stack_idx - 2);
    aint_t a_can_mw_type_idx = any_check_index(a, stack_idx-1);
    aint_t a_interface_idx = any_check_index(a, stack_idx);
    open_info->can_type = (can_type_t) any_check_integer(a, a_can_mw_type_idx);
    open_info->interface = any_check_string(a, a_interface_idx);
    open_info->open_type = (osp_rtw_can_open_type_t)any_check_integer(a, a_open_type_idx);
    open_info->channel = any_check_string(a, a_channel_idx);
    return ret;
}
static osp_rtw_ret_t get_can_frame_from_avm_stack(aactor_t* a, aint_t stack_idx, osp_rtw_can_frame_t* can_frame)
{
    lwlog_info("get_can_frame_from_avm_stack is called\n");
    osp_rtw_ret_t ret = OSP_RTW_RET_ERROR_NONE;
    aint_t can_frame_idx = any_check_index(a, stack_idx);
    aint_t cur_idx;
    if(NULL == can_frame) {
        return OSP_RTW_RET_ERROR_INVALID;
    }
    any_import(a, "std-tuple", "get/2");
    any_push_integer(a, 0);
    any_push_index(a, can_frame_idx);
    any_call(a, 2);
    cur_idx = any_top(a);
    can_frame->can_id = any_check_integer(a, cur_idx);
    any_pop(a, 1);

    any_import(a, "std-tuple", "get/2");
    any_push_integer(a, 1);
    any_push_index(a, can_frame_idx);
    any_call(a, 2);
    cur_idx = any_top(a);
    can_frame->dlc = any_check_integer(a, cur_idx);
    any_pop(a, 1);

    any_import(a, "std-tuple", "get/2");
    any_push_integer(a, 2);
    any_push_index(a, can_frame_idx);
    any_call(a, 2);
    cur_idx = any_top(a);
    can_frame->data = any_to_buffer(a, cur_idx);
    any_pop(a, 1);

    lwlog_info("==> can_frame->can_id: [%lld]\n", can_frame->can_id);
    lwlog_info("==> can_frame->dlc: [%lld]\n", can_frame->dlc);
    lwlog_info("==> can_frame->data: [%d]\n", can_frame->data[0]);
    return ret;
}
static osp_rtw_ret_t get_can_mw_info_from_avm_stack(aactor_t* a, aint_t stack_idx, osp_rtw_can_info_t* can_info)
{
    osp_rtw_ret_t ret = OSP_RTW_RET_ERROR_NONE;
    lwlog_info("get_can_mw_info_from_avm_stack is called\n");
    if(NULL == can_info) {
        return OSP_RTW_RET_ERROR_INVALID;
    }
    //aint_t t_idx = any_top(a);
    aint_t can_mw_info_idx = any_check_index(a, stack_idx);
    aint_t cur_idx;
    //aint_t num_can_mw_info_elements = 3;
    any_import(a, "std-tuple", "get/2");
    any_push_integer(a, 0);
    any_push_index(a, can_mw_info_idx);
    any_call(a, 2);
    cur_idx = any_top(a);
    can_info->can_type = any_check_integer(a, cur_idx);
    any_pop(a, 1);

    any_import(a, "std-tuple", "get/2");
    any_push_integer(a, 1);
    any_push_index(a, can_mw_info_idx);
    any_call(a, 2);
    cur_idx = any_top(a);
    can_info->interface = any_check_string(a, cur_idx);
    any_pop(a, 1);

    any_import(a, "std-tuple", "get/2");
    any_push_integer(a, 2);
    any_push_index(a, can_mw_info_idx);
    any_call(a, 2);
    cur_idx = any_top(a);
    can_info->pid = any_to_pid(a, cur_idx);
    any_pop(a, 1);

    any_import(a, "std-tuple", "get/2");
    any_push_integer(a, 3);
    any_push_index(a, can_mw_info_idx);
    any_call(a, 2);
    cur_idx = any_top(a);
    can_info->priv_data = (void*) any_check_integer(a, cur_idx);
    any_pop(a, 1);

    any_import(a, "std-tuple", "get/2");
    any_push_integer(a, 4);
    any_push_index(a, can_mw_info_idx);
    any_call(a, 2);
    cur_idx = any_top(a);
    can_info->channel = any_check_string(a, cur_idx);
    any_pop(a, 1);

    lwlog_info("==> can_info->can_type: [%d]\n", can_info->can_type);
    lwlog_info("==> can_info->interface: [%s]\n", can_info->interface);
    lwlog_info("==> can_info->channel: [%s]\n", can_info->channel);
    lwlog_info("==> can_info->pid: [%d]\n", can_info->pid);
    lwlog_info("==> can_info->priv: [%d]\n", can_info->priv_data);
    return ret;
}
static void linitialize(aactor_t* a)
{
    lwlog_info("linitialize is called\n");
    // call cord initialize
    can_mw_cord_initialize();
    any_push_nil(a);
    return;
}
static void lfinalize(aactor_t* a)
{
    lwlog_info("lfinalize is called\n");
    can_mw_cord_finalize();
    any_push_nil(a);
    return;
}
static osp_rtw_ret_t open_type_b(osp_rtw_can_open_opt_t* open_opt, osp_rtw_can_info_t* can_info)
{
    lwlog_info("open_type_b is called\n");
    osp_rtw_ret_t ret = OSP_RTW_RET_ERROR_NONE;
    osp_rtw_drv_can_fops_t* drv_fops = NULL;
    ret = can_utils_get_drv_by_type(open_opt->can_type, &drv_fops);
    if (ret != OSP_RTW_RET_ERROR_NONE) {
        // invalid -> return immediatly
        CAN_INFO_FILL_INVALID(can_info);
        return ret;
    }
    ret = drv_fops->open(open_opt, can_info);
    if (ret != OSP_RTW_RET_ERROR_NONE) {
        CAN_INFO_FILL_INVALID(can_info);
        return ret;
    }
    else {
        ret = can_mw_cord_declare(can_info, NULL);
    }
    return ret;
}
static osp_rtw_ret_t open_type_a(osp_rtw_can_open_opt_t* open_opt, osp_rtw_can_info_t* can_info)
{
    lwlog_info("open_type_a is called\n");
    osp_rtw_ret_t ret = OSP_RTW_RET_ERROR_NONE;
    //  try to get can_info from cordinator
    osp_rtw_can_info_t* p;
    ret = can_mw_cord_request(open_opt, &p);
    if (OSP_RTW_RET_ERROR_NONE != ret) {
        //ret = open_type_b(open_opt, can_info);
        return ret;
    }
    else {
        can_info->can_type = p->can_type;
        can_info->interface = p->interface;
        can_info->channel = p->channel;
        can_info->pid = p->pid;
        can_info->priv_data = p->priv_data;
    }
    return ret;
}
static osp_rtw_ret_t open_type_ab(osp_rtw_can_open_opt_t* open_opt, osp_rtw_can_info_t* can_info)
{
    lwlog_info("open_type_a is called\n");
    osp_rtw_ret_t ret = OSP_RTW_RET_ERROR_NONE;
    // open_type_a first
    ret = open_type_a(open_opt, can_info);
    if (OSP_RTW_RET_ERROR_NONE != ret) {
        // if error -> open_type_b
        ret = open_type_b(open_opt, can_info);
    }
    return ret;
}
static osp_rtw_ret_t open_type_ba(osp_rtw_can_open_opt_t* open_opt, osp_rtw_can_info_t* can_info)
{
    lwlog_info("open_type_a is called\n");
    osp_rtw_ret_t ret = OSP_RTW_RET_ERROR_NONE;
    // open_type_a first
    ret = open_type_b(open_opt, can_info);
    if (OSP_RTW_RET_ERROR_NONE != ret && OSP_RTW_RET_ERROR_DUPLICATE != ret) {
        // if error -> open_type_a
        ret = open_type_a(open_opt, can_info);
    }
    return ret;
}
static void lopen(aactor_t* a) {
    lwlog_info("lopen is called\n");
    osp_rtw_can_open_opt_t open_opt;
    osp_rtw_can_info_t can_info;
    osp_rtw_drv_can_fops_t* drv_fops=NULL;
    osp_rtw_ret_t ret = OSP_RTW_RET_ERROR_NONE;
    ret = get_can_mw_open_info_from_avm_stack(a, -1, &open_opt);
    lwlog_info("lopen opening the [%d] can type for [%s] interface\n", open_opt.can_type, open_opt.interface);
    switch (open_opt.open_type)
    {
    case OSP_RTW_CAN_OPEN_TYPE_A:
        ret = open_type_a(&open_opt, &can_info);
        //goto result;
        break;
    case OSP_RTW_CAN_OPEN_TYPE_B:
        ret = open_type_b(&open_opt, &can_info);
        break;
    case OSP_RTW_CAN_OPEN_TYPE_AB:
        ret = open_type_ab(&open_opt, &can_info);
        break;
    case OSP_RTW_CAN_OPEN_TYPE_BA:
        ret = open_type_ba(&open_opt, &can_info);
        break;
    default:
        can_info.can_type = OCT_INVALID;
        can_info.interface = "";
        can_info.channel = "";
        can_info.pid = -1;
        can_info.priv_data = NULL;
        break;
    }
    lwlog_info("==> can_info->can_type: [%d]\n", can_info.can_type);
    lwlog_info("==> can_info->interface: [%s]\n", can_info.interface);
    lwlog_info("==> can_info->channel: [%s]\n", can_info.channel);
    lwlog_info("==> can_info->pid: [%d]\n", can_info.pid);
    lwlog_info("==> can_info->priv: [%d]\n", can_info.priv_data);
result:
    any_push_tuple(a, 5);
    aint_t t_idx = any_top(a);
    //lwlog_info("lopen any_tuple_size [%d]\n", any_tuple_size(a, t_idx));
    any_import(a, "std-tuple", "set/3");
    any_push_integer(a, can_info.can_type);
    any_push_integer(a, 0);     // num of elements
    any_push_index(a, t_idx);   // push index
    any_call(a, 3);
    any_pop(a, 1);

    any_import(a, "std-tuple", "set/3");
    any_push_string(a, can_info.interface);
    any_push_integer(a, 1);     // num of elements
    any_push_index(a, t_idx);   // push index
    any_call(a, 3);
    any_pop(a, 1);

    any_import(a, "std-tuple", "set/3");
    any_push_pid(a, can_info.pid);
    any_push_integer(a, 2);     // num of elements
    any_push_index(a, t_idx);   // push index
    any_call(a, 3);
    any_pop(a, 1);

    any_import(a, "std-tuple", "set/3");
    any_push_integer(a, (aint_t)can_info.priv_data);
    any_push_integer(a, 3);
    any_push_index(a, t_idx);
    any_call(a, 3);
    any_pop(a, 1);

    any_import(a, "std-tuple", "set/3");
    any_push_string(a, can_info.channel);
    any_push_integer(a, 4);         // num of elements
    any_push_index(a, t_idx);       // push index
    any_call(a, 3);
    any_pop(a, 1);

    lwlog_info("lopen is ended\n");
    return;
}
static void lclose(aactor_t* a) {
    lwlog_info("lclose is called\n");
    osp_rtw_ret_t ret = OSP_RTW_RET_ERROR_NONE;
    osp_rtw_can_info_t can_info;
    osp_rtw_drv_can_fops_t* drv_fops=NULL;
    ret = get_can_mw_info_from_avm_stack(a, -1, &can_info);
    // TOOD check return error code.
    // call DRV
    ret = can_utils_get_drv_by_type(can_info.can_type, &drv_fops);
    // TODO check return error code
    if(ret != OSP_RTW_RET_ERROR_NONE) {
        // invalid -> return immediatly
        lwlog_err("invalid can_type\n");
        goto result;
    }
    uint16_t ref_cnt = 0;
    ret = can_mw_cord_undeclare(&can_info, &ref_cnt);
    //int ref_cnt = can_mw_cord_get_ref_cnt(&can_info);
    if(OSP_RTW_RET_ERROR_NONE == ret && 0 == ref_cnt) {
        drv_fops->close(&can_info);
    }
result:
    any_push_nil(a);
    lwlog_info("lclose is ended\n");
    return;
}
static void lsend(aactor_t* a) {
    lwlog_info("lsend is called\n");
    osp_rtw_ret_t ret = OSP_RTW_RET_ERROR_NONE;
    osp_rtw_drv_can_fops_t* drv_fops = NULL;
    osp_rtw_can_info_t can_info;
    osp_rtw_can_frame_t can_frame;
    aint_t can_frame_idx = any_check_index(a, -2);
    ret = get_can_mw_info_from_avm_stack(a, -1, &can_info);   // first parameter is can_mw_info
    // TODO check return error code
    ret = get_can_frame_from_avm_stack(a, -2, &can_frame);      // second paramter is can_frame

    lwlog_info("==> can_frame->can_id: [%lld]\n", can_frame.can_id);
    lwlog_info("==> can_frame->dlc: [%lld]\n", can_frame.dlc);
    lwlog_info("==> can_frame->data: [%d]\n",can_frame.data[0]);

    ret = can_utils_get_drv_by_type(can_info.can_type, &drv_fops);
    // TODO check return error code
    if(ret != OSP_RTW_RET_ERROR_NONE) {
        lwlog_err("invalid can_type\n");
        goto result;
    }

    ret = drv_fops->send(&can_info, &can_frame);
result:
    // return nil
    any_push_nil(a);
    lwlog_info("lsend is ended\n");
    return;
}
static void lsetopt(aactor_t* a)
{
    lwlog_info("lsubcribe is called\n");
    osp_rtw_ret_t ret = OSP_RTW_RET_ERROR_NONE;
    osp_rtw_can_info_t can_info;
    osp_rtw_drv_can_fops_t* drv_fops=NULL;
    ret = get_can_mw_info_from_avm_stack(a, -1, &can_info);
    // TOOD check return error code.
    // call DRV
    ret = can_utils_get_drv_by_type(can_info.can_type, &drv_fops);
    // TODO check return error code
    if(ret != OSP_RTW_RET_ERROR_NONE) {
        // invalid -> return immediatly
        lwlog_err("invalid can_type\n");
        goto result;
    }
    drv_fops->setopt(&can_info, NULL);
result:
    any_push_nil(a);
    return;
}
static void lsubcribe(aactor_t* a) {
    lwlog_info("lsubcribe is called\n");
    aint_t a_interface_idx = any_check_index(a, -1);
    aint_t interface = any_check_integer(a, a_interface_idx);
    aint_t a_pid_idx = any_check_index(a, -2);
    aint_t pid = any_check_integer(a, a_pid_idx);
    lwlog_info("lsubcribe subcribing pid [%lld] to listen interface [%lld]", pid, interface);
    any_push_integer(a, 1000);
    return;
}
static void lunsubcribe(aactor_t* a) {
    lwlog_info("lunsubcribe is called\n");
    aint_t a_interface_idx = any_check_index(a, -1);
    aint_t interface = any_check_integer(a, a_interface_idx);
    aint_t a_pid_idx = any_check_index(a, -2);
    aint_t pid = any_check_integer(a, a_pid_idx);
    lwlog_info("lunsubcribe unsubcribing pid [%lld] to listen interface [%lld]", pid, interface);
    any_push_integer(a, 1000);
    return;
}
static void lget_sent_history(aactor_t* a) {
    lwlog_info("lget_sent_history is called\n");
    lwlog_info("lget_sent_history T.B.D\n");
    any_push_nil(a);
    return;
}
void anet_lib_add_gen_can_mw(aloader_t* l, void* ud) {
    aloader_add_lib(l, &mod);
    m_ud = ud;
}
