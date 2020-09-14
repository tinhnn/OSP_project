#pragma once
#include <any/platform.h>

#include <inc/osp_rtw/osp_rtw_can.h>
#include <inc/osp_rtw/osp_rtw_types.h>

#ifdef __cplusplus
    extern "C" {
#endif
        ANY_API osp_rtw_ret_t can_mw_cord_initialize(void);
        ANY_API osp_rtw_ret_t can_mw_cord_finalize(void);
        ANY_API osp_rtw_ret_t can_mw_cord_declare (osp_rtw_can_info_t* can_info, uint16_t* ref_cnt);
        ANY_API osp_rtw_ret_t can_mw_cord_undeclare (osp_rtw_can_info_t* can_info, uint16_t* ref_cnt);
        ANY_API osp_rtw_ret_t can_mw_cord_request(const osp_rtw_can_open_opt_t* open_opt, osp_rtw_can_info_t** can_info);
        ANY_API int can_mw_cord_get_ref_cnt(osp_rtw_can_info_t* can_info);
#ifdef __cplusplus
    }
#endif
/*EOF*/