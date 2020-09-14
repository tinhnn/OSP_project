#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <lwlog/lwlog.h>

#include <any/list.h>

#include "can_mw_cord.h"
struct osp_rtw_can_cord_state_t;
struct osp_rtw_can_info_node_t;
typedef struct {
    osp_rtw_can_info_t* can_info;
    uint16_t ref_cnt;
} osp_rtw_can_cord_state_t;
typedef struct osp_rtw_can_info_node_t {
    osp_rtw_can_cord_state_t* val;
    struct osp_rtw_can_info_node_t* self;
    alist_node_t node;
} osp_rtw_can_info_node_t;
static alist_t m_can_info_list;
static bool m_is_init = false;
static int can_mw_cord_check(osp_rtw_can_info_t* can_info, alist_node_t** node);
static int can_mw_cord_check_by_open_opt(osp_rtw_can_open_opt_t* open_opt, alist_node_t** node);
static osp_rtw_can_info_node_t* new_node(osp_rtw_can_info_t* can_info);
static void delete_node_info(osp_rtw_can_cord_state_t* state);
static osp_rtw_ret_t free_node(alist_node_t* itr);
static osp_rtw_can_cord_state_t* new_node_info(osp_rtw_can_info_t* can_info)
{
    osp_rtw_can_cord_state_t* p_state = (osp_rtw_can_cord_state_t*)malloc(sizeof(osp_rtw_can_cord_state_t));
    if (NULL == p_state) {
        lwlog_err("Error. Cannot allocate osp_rtw_can_cord_state_t\n");
        return NULL;
    }
    osp_rtw_can_info_t* p = (osp_rtw_can_info_t*)malloc(sizeof(osp_rtw_can_info_t));
    if (NULL == p) {
        lwlog_err("Error. Cannot allocate osp_rtw_can_info_t\n");
        return NULL;
    }
    p->can_type = can_info->can_type;
    size_t size = strlen(can_info->channel);
    p->channel = (char*)malloc(size + 1);
    strcpy(p->channel, can_info->channel);
    size = strlen(can_info->interface);
    p->interface = (char*)malloc(size + 1);
    strcpy(p->interface, can_info->interface);
    p->pid = can_info->pid;
    p->priv_data = can_info->priv_data;
    p_state->can_info = p;
    p_state->ref_cnt = 0;
    return p_state;
}
static osp_rtw_can_info_node_t* new_node(osp_rtw_can_info_t* can_info)
{
    lwlog_info("new_node is called\n");
    osp_rtw_can_cord_state_t* p_state = new_node_info(can_info);
    osp_rtw_can_info_node_t* node = (osp_rtw_can_info_node_t*)malloc(sizeof(osp_rtw_can_info_node_t));
    if (NULL == p_state || NULL == node) {
        lwlog_err("Cannot allocate resources\n");
        return NULL;
    }
    node->val = p_state;
    node->self = node;
    return node;
}
static void delete_node_info(osp_rtw_can_cord_state_t* state)
{
    if (NULL == state) {
        return;
    }
    if (NULL != state && NULL != state->can_info) {
        if (NULL != state->can_info->channel) {
            free(state->can_info->channel);
            state->can_info->channel = NULL;
        }
        if (NULL != state->can_info->interface) {
            free(state->can_info->interface);
            state->can_info->interface = NULL;
        }
        // free can_info
        free(state->can_info);
    }
    return;
}
static osp_rtw_ret_t free_node(alist_node_t* itr)
{
    osp_rtw_ret_t ret = OSP_RTW_RET_ERROR_NONE;
    lwlog_info("free node is called\n");
    osp_rtw_can_cord_state_t* p_state = NULL;
    osp_rtw_can_info_node_t* p_node = NULL;
    p_state = ALIST_NODE_CAST(osp_rtw_can_info_node_t, itr)->val;
    p_node = ALIST_NODE_CAST(osp_rtw_can_info_node_t, itr)->self;
    // free p_node
    free(p_node);
    // delete node info
    delete_node_info(p_state);
    // free state
    free(p_state);
    return ret;
}
static int can_mw_cord_check(osp_rtw_can_info_t* can_info, alist_node_t** node) {
    lwlog_info("check is called\n");
    int ret = 0;
    if (false == m_is_init) {
        lwlog_err("can cord is not initialized yet!\n");
        return -1;
    }
    alist_node_t* itr = alist_head(&m_can_info_list);
    osp_rtw_can_cord_state_t* p = NULL;
    // TODO. hash mechanism to improve performance
    do {
        if (alist_is_end(&m_can_info_list, itr)) {
            break;
        }
        p = ALIST_NODE_CAST(osp_rtw_can_info_node_t, itr)->val;
        if (p->can_info->can_type == can_info->can_type &&
            0 == strcmp(p->can_info->channel, can_info->channel) &&
            0 == strcmp(p->can_info->interface, can_info->interface) &&
            p->can_info->priv_data == can_info->priv_data) {
            if (node != NULL) {
                *node = itr;
            }
            return ret;
        }
        ret++;
        itr = itr->next;
    } while (true);
    return -1;
}
static int can_mw_cord_check_by_open_opt(osp_rtw_can_open_opt_t* open_opt, alist_node_t** node) {
    lwlog_info("check is called\n");
    alist_node_t* itr = alist_head(&m_can_info_list);
    int ret = 0;
    osp_rtw_can_cord_state_t* p = NULL;
    // TODO. hash mechanism to improve performance
    do {
        if (alist_is_end(&m_can_info_list, itr)) {
            break;
        }
        p = ALIST_NODE_CAST(osp_rtw_can_info_node_t, itr)->val;
        if (p->can_info->can_type == open_opt->can_type &&
            0 == strcmp(p->can_info->channel, open_opt->channel) &&
            0 == strcmp(p->can_info->interface, open_opt->interface)) {
            if (node != NULL) {
                *node = itr;
            }
            return ret;
        }
        ret++;
        itr = itr->next;
    } while (true);
    return -1;
}
osp_rtw_ret_t can_mw_cord_initialize(void)
{
    osp_rtw_ret_t ret = OSP_RTW_RET_ERROR_NONE;
    lwlog_info("initialize is called\n");
    if (false == m_is_init) {
        alist_init(&m_can_info_list);
        m_is_init = true;
    }
    return ret;
}
osp_rtw_ret_t can_mw_cord_finalize(void)
{
    osp_rtw_ret_t ret = OSP_RTW_RET_ERROR_NONE;
    lwlog_info("finalize is called\n");
    if (true == m_is_init) {
        // TODO. hash mechanism to improve performance
        do {
            alist_node_t* itr = alist_head(&m_can_info_list);
            if (alist_is_end(&m_can_info_list, itr)) {
                break;
            }
            // erase node from list
            alist_node_erase(itr);
            // free node
            free_node(itr);
            ret++;
        } while (true);
        m_is_init = false;
    }
    return ret;
}
osp_rtw_ret_t can_mw_cord_declare (osp_rtw_can_info_t* can_info, uint16_t* ref_cnt)
{
    osp_rtw_ret_t ret = OSP_RTW_RET_ERROR_NONE;
    lwlog_info("declare is called\n");
    if (false == m_is_init) {
        lwlog_err("can cord has not initialized yet!\n");
        return OSP_RTW_RET_ERROR_EFAULT;
    }
    //can_mw_cord_initialize();
    alist_node_t* node = NULL;
    // check current list for this can_info
    int check_ret = can_mw_cord_check(can_info, &node);
    if(check_ret >= 0) {
        lwlog_info("Error. Duplicate\n");
        if (ref_cnt != NULL) {
            osp_rtw_can_cord_state_t* p = ALIST_NODE_CAST(osp_rtw_can_info_node_t, node)->val;
            *ref_cnt = p->ref_cnt;
        }
        ret = OSP_RTW_RET_ERROR_DUPLICATE;
        goto result;
    }
    else {
        osp_rtw_can_info_node_t* p_node = new_node(can_info);
        if (NULL == p_node) {
            lwlog_err("Error. Cannot create new node\n");
            ret = OSP_RTW_RET_ERROR_NOMEM;
            goto result;
        }
        p_node->val->ref_cnt = 1; // the first one
        // push to list
        alist_push_back(&m_can_info_list, &p_node->node);
        goto result;
    }
result:
    return ret;
}
osp_rtw_ret_t can_mw_cord_undeclare(osp_rtw_can_info_t* can_info, uint16_t* ref_cnt)
{
    osp_rtw_ret_t ret = OSP_RTW_RET_ERROR_NONE;
    lwlog_info("undeclare is called\n");
    if (false == m_is_init) {
        lwlog_err("can cord has not initialized yet!\n");
        return OSP_RTW_RET_ERROR_EFAULT;
    }
    alist_node_t* itr;
    int check_ret = can_mw_cord_check(can_info, &itr);
    if (check_ret < 0) {
        lwlog_info("Error. invalid argument\n");
        return OSP_RTW_RET_ERROR_INVALID;
    }
    else {
        // free the unmanaged resource in node
        osp_rtw_can_cord_state_t* p_state = NULL;
        //osp_rtw_can_info_node_t* p_node = NULL;
        p_state = ALIST_NODE_CAST(osp_rtw_can_info_node_t, itr)->val;
        //p_node = ALIST_NODE_CAST(osp_rtw_can_info_node_t, itr)->self;
        p_state->ref_cnt--;
        if (NULL != ref_cnt) {
            *ref_cnt = p_state->ref_cnt;
        }
        if (0 == p_state->ref_cnt) {
            // erase node from list
            alist_node_erase(itr);
            // free node information
            free_node(itr);
        }
    }

    return ret;
}
osp_rtw_ret_t can_mw_cord_request(const osp_rtw_can_open_opt_t* open_opt, osp_rtw_can_info_t** can_info)
{
    osp_rtw_ret_t ret = OSP_RTW_RET_ERROR_NONE;
    osp_rtw_can_cord_state_t* p = NULL;
    lwlog_info("can_mw_cord_get is called\n");
    if (false == m_is_init) {
        lwlog_err("can cord has not initialized yet!\n");
        return OSP_RTW_RET_ERROR_EFAULT;
    }
    if (NULL == open_opt) {
        lwlog_err("Error. Invalid argument\n");
    }
    alist_node_t* node;
    int check_ret = can_mw_cord_check_by_open_opt(open_opt, &node);
    if (check_ret <0) {
        lwlog_err("Error. Can not find can_info for the open_opt\n");
        return OSP_RTW_RET_ERROR_EFAULT;
    }
    else {
        if (can_info != NULL) {
            // get this value
            p = ALIST_NODE_CAST(osp_rtw_can_info_node_t, node)->val;
            p->ref_cnt++;
            *can_info = p->can_info;
        }
    }
    return ret;
}
ANY_API int can_mw_cord_get_ref_cnt(osp_rtw_can_info_t* can_info)
{
    osp_rtw_ret_t ret = OSP_RTW_RET_ERROR_NONE;
    lwlog_info("can_mw_cord_get_ref_cnt is called\n");
    if (false == m_is_init) {
        lwlog_err("can cord has not initialized yet!\n");
        return OSP_RTW_RET_ERROR_EFAULT;
    }
    alist_node_t* node;
    int check_ret = can_mw_cord_check(can_info, &node);
    if (check_ret < 0) {
        lwlog_info("Error. invalid argument\n");
        return OSP_RTW_RET_ERROR_INVALID;
    }
    osp_rtw_can_cord_state_t* p = NULL;
    p = ALIST_NODE_CAST(osp_rtw_can_info_node_t, node)->val;
    return p->ref_cnt;
}
/*EOF*/