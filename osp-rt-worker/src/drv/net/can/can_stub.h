#pragma once

#if 0
#ifdef __cplusplus
    extern "C" {
#endif
        int can_sock_open_if(const char* if);
        void can_sock_send(const int sock, const CAN_FRAME_T* frame);
        void can_sock_close_if(const int sock);
#ifdef __cplusplus
    }    // extern "C"
#endif
#endif
/*EOF*/
