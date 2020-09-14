#ifndef __OSP_RTW_TYPES_H__
#define __OSP_RTW_TYPES_H__
typedef enum {
    OSP_RTW_RET_ERROR_NONE      = 0,
    OSP_RTW_RET_ERROR_INVALID   = -1,
    OSP_RTW_RET_ERROR_NOMEM     = -2,
    OSP_RTW_RET_ERROR_EFAULT    = -3,
    OSP_RTW_RET_ERROR_DUPLICATE = -4,
    OSP_RTW_RET_ERROR_UNKNOWN   = -1000,
} osp_rtw_ret_t;
#endif