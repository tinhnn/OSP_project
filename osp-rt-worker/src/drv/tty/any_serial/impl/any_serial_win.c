#include "inc/any_serial_win.h"

#if defined _WIN32 || defined(__WIN32__) || defined(_WIN32) || defined(_MSC_VER)
//#define ANY_SERIAL_DEBUG
#define __AS_FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#ifdef ANY_SERIAL_DEBUG
#   define serial_debug_log(M, ...)    printf("[===INFO===]    " "%s (%s:%d) "M "\n", __func__, __AS_FILENAME__, __LINE__, ##__VA_ARGS__);
#else
#   define serial_debug_log(M, ...) do{} while(0)
#endif
# define serial_error_log(M, ...) printf("[ERROR]    " "%s (%s:%d) "M "\n", __func__, __AS_FILENAME__, __LINE__, ##__VA_ARGS__);
typedef struct {
    any_serial_obj_t* serial_obj;
    uint8_t* buf;
    uint16_t size;
} write_worker_thread_param_t;
#ifdef ANY_SERIAL_THREAD_WORKER
DWORD WINAPI write_thread_worker(LPVOID lpParam)
{
    serial_debug_log("write_thread_worker is called\n");
    write_worker_thread_param_t* wwt = (write_worker_thread_param_t*)lpParam;
    any_serial_obj_t* p_serial_obj;
    if (NULL == lpParam) {
        serial_error_log("Error. input argument is invalid\n");
        return -1;
    }
    p_serial_obj = wwt->serial_obj;
#ifndef ANY_SERIAL_OVERLAPPED
#else
    if (!p_serial_obj->m_hevtOverlapped) {
        serial_error_log("Error. Invalid overlapped object\n");
        return ANY_SERIAL_RET_ERROR_INVALID;
    }
    OVERLAPPED ovInternal;
    LPOVERLAPPED lpOverlapped;
    DWORD dwBytesWritten;
    DWORD dwTimeout = 1000;
    ovInternal.hEvent = p_serial_obj->m_hevtOverlapped;
    lpOverlapped = &ovInternal;
    if (!WriteFile(p_serial_obj->fd, wwt->buf, wwt->size, &dwBytesWritten, lpOverlapped)) {
        long lLastError = GetLastError();
        serial_error_log("Error. Write data error [%d]\n", lLastError);
        if (lpOverlapped == &ovInternal) {
            switch (WaitForSingleObject(lpOverlapped->hEvent, dwTimeout)) {
            case WAIT_OBJECT_0:
                if (!GetOverlappedResult(p_serial_obj->fd, lpOverlapped, &dwBytesWritten, FALSE)) {
                    serial_error_log("Error. GetOverlappedResult [%d]\n", GetLastError());
                }
                break;
            case WAIT_TIMEOUT:
                CancelIo(p_serial_obj->fd);
                serial_error_log("Error. Timeout! [%d]\n", GetLastError());
                break;
            default:
                serial_error_log("Error. EFAULT! [%d]\n", GetLastError());
                break;
            }
        }
    }
    else {
        if (lpOverlapped) {
            SetEvent(lpOverlapped->hEvent);
        }
    }
#endif
finished:
    free(lpParam);
    return 0;
}
#endif
static any_serial_ret_t any_serial_reconfigure_port(any_serial_obj_t* serial_obj)
{
    any_serial_ret_t ret = ANY_SERIAL_RET_ERROR_NONE;
    DCB dcbSerialParams = { 0 };
    any_serial_open_info_t* p_opened_info = NULL;
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (!GetCommState(serial_obj->fd, &dcbSerialParams)) {
        serial_error_log("Error getting the serial port state\n");
        return ANY_SERIAL_RET_ERROR_EFAULT;
    }
    p_opened_info = &serial_obj->opened_info;
    // setup baudrate. TODO
    dcbSerialParams.BaudRate = p_opened_info->baudrate;
    
    // setup char len. We treat the bytesize enum to actual value. TODO
    dcbSerialParams.ByteSize = p_opened_info->bytesize;

    //setup stopbits

    if (ANY_SERIAL_STOPBITS_ONE == p_opened_info->stopbits) {
        dcbSerialParams.StopBits = ONESTOPBIT;
    }
    else if (ANY_SERIAL_STOPBITS_TWO == p_opened_info->stopbits) {
        dcbSerialParams.StopBits == TWOSTOPBITS;
    }
    else if (ANY_SERIAL_STOPBITS_ONE_POINT_FIVE == p_opened_info->stopbits) {
        dcbSerialParams.StopBits = ONE5STOPBITS;
    }
    else {
        serial_error_log("Stopbit [%d] is invalid", p_opened_info->stopbits);
        return ANY_SERIAL_RET_ERROR_INVALID;
    }

    //setup parity
    if (ANY_SERIAL_PARITY_NONE == p_opened_info->parity) {
        dcbSerialParams.Parity = NOPARITY;
    }
    else if(ANY_SERIAL_PARITY_EVEN == p_opened_info->parity) {
        dcbSerialParams.Parity = EVENPARITY;
    }
    else if (ANY_SERIAL_PARITY_MARK == p_opened_info->parity) {
        dcbSerialParams.Parity = MARKPARITY;
    }
    else if (ANY_SERIAL_PARITY_ODD == p_opened_info->parity) {
        dcbSerialParams.Parity = ODDPARITY;
    }
    else if (ANY_SERIAL_PARITY_SPACE == p_opened_info->parity) {
        dcbSerialParams.Parity = SPACEPARITY;
    }
    else {
        serial_error_log("Parity [%d] is invalid\n", p_opened_info->parity);
        return ANY_SERIAL_RET_ERROR_INVALID;
    }

    // setup flowcontrol
    if (ANY_SERIAL_FLOWCONTROL_NONE == p_opened_info->flowcontrol) {
        dcbSerialParams.fOutxCtsFlow = false;
        dcbSerialParams.fRtsControl = 0x00;
        dcbSerialParams.fOutX = false;
        dcbSerialParams.fInX = false;
    }
    else if (ANY_SERIAL_FLOWCONTROL_SOFTWARE == p_opened_info) {
        dcbSerialParams.fOutxCtsFlow = false;
        dcbSerialParams.fRtsControl = 0x00;
        dcbSerialParams.fOutX = true;
        dcbSerialParams.fInX = true;
    }
    else if (ANY_SERIAL_FLOWCONTROL_HARDWARE == p_opened_info->flowcontrol) {
        dcbSerialParams.fOutxCtsFlow = true;
        dcbSerialParams.fRtsControl = 0x03;
        dcbSerialParams.fOutX = false;
        dcbSerialParams.fInX = false;
    }
    else {
        serial_error_log("Flowcontrol [%d] is invalid\n", p_opened_info->flowcontrol);
        return ANY_SERIAL_RET_ERROR_INVALID;
    }

    // activate settings
    if (!SetCommState(serial_obj->fd, &dcbSerialParams)) {
        serial_error_log("Error. Cannot setting comm state\n");
        CloseHandle(serial_obj->fd);
        return ANY_SERIAL_RET_ERROR_EFAULT;
    }

    // setup timeout. Fixed value. TODO implement timeout obj
    // instance an object of COMMTIMEOUTS.
    COMMTIMEOUTS comTimeOut;
    // Specify time-out between charactor for receiving.
    comTimeOut.ReadIntervalTimeout = 3;
    // Specify value that is multiplied 
    // by the requested number of bytes to be read. 
    comTimeOut.ReadTotalTimeoutMultiplier = 3;
    // Specify value is added to the product of the 
    // ReadTotalTimeoutMultiplier member
    comTimeOut.ReadTotalTimeoutConstant = 2;
    // Specify value that is multiplied 
    // by the requested number of bytes to be sent. 
    comTimeOut.WriteTotalTimeoutMultiplier = 3;
    // Specify value is added to the product of the 
    // WriteTotalTimeoutMultiplier member
    comTimeOut.WriteTotalTimeoutConstant = 2;
    // set the time-out parameter into device control.
    if (!SetCommTimeouts(serial_obj->fd, &comTimeOut)) {
        serial_error_log("Error. Cannot setting comm timeout\n");
        return ANY_SERIAL_RET_ERROR_EFAULT;
    }
    return ret;
}
any_serial_ret_t any_serial_open (const any_serial_open_info_t* open_info, any_serial_obj_t* serial_obj)
{
    any_serial_ret_t ret = ANY_SERIAL_RET_ERROR_NONE;
    serial_debug_log("any_serial_open is called!\n");
    if(NULL == open_info || NULL == serial_obj) {
        serial_error_log("input parameters is invalid\n");
        return ANY_SERIAL_RET_ERROR_INVALID;
    }
    if (serial_obj->is_open) {
        serial_error_log("Already open?\n");
        return ANY_SERIAL_RET_ERROR_INVALID;
    }
#ifndef ANY_SERIAL_OVERLAPPED
    serial_obj->fd = CreateFile(open_info->port,
        GENERIC_READ | GENERIC_WRITE,
        0,
        0,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        0);
#else
    serial_obj->fd = CreateFile(open_info->port,
        GENERIC_WRITE, //GENERIC_READ | GENERIC_WRITE,
        0,
        0,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
        0);
#endif
    if (INVALID_HANDLE_VALUE == serial_obj->fd) {
        serial_debug_log("Cannot open serial port [%s]\n", open_info->port);
        return ANY_SERIAL_RET_ERROR_EFAULT;
    }
#ifndef ANY_SERIAL_OVERLAPPED
#else
    memset(&serial_obj->ovl_write, 0, sizeof(serial_obj->ovl_write));
    serial_obj->ovl_write.hEvent = CreateEvent(0, true, false, 0);
    if (NULL == serial_obj->ovl_write.hEvent) {
        serial_error_log("Cannot create overlapped object\n");
        CloseHandle(serial_obj->fd);
        serial_obj->fd = INVALID_HANDLE_VALUE;
        return ANY_SERIAL_RET_ERROR_EFAULT;
    }
#endif
    memcpy(&serial_obj->opened_info, open_info, sizeof(any_serial_open_info_t));
    ret = any_serial_reconfigure_port(serial_obj);
    if (ANY_SERIAL_RET_ERROR_NONE == ret) {
        serial_obj->is_open = true;
    }
    serial_obj->is_writing = false;
    return ret;
}
any_serial_ret_t any_serial_close (any_serial_obj_t* serial_obj)
{
    any_serial_ret_t ret = ANY_SERIAL_RET_ERROR_NONE;
    serial_debug_log("any_serial_close is called!\n");
    if (NULL == serial_obj) {
        serial_error_log("Error. Invalid argument\n");
        return ANY_SERIAL_RET_ERROR_INVALID;
    }
    if (true == serial_obj->is_open) {
        if (INVALID_HANDLE_VALUE != serial_obj->fd) {
            bool r = CloseHandle(serial_obj->fd);
            if (false == r) {
                serial_error_log("Error. Cannot closehandle [%d]\n", serial_obj->fd);
            }
            else {
                serial_obj->fd = INVALID_HANDLE_VALUE;
            }
        }
        serial_obj->is_open = false;
    }
#ifdef ANY_SERIAL_OVERLAPPED
    CloseHandle(serial_obj->ovl_write.hEvent);
#endif
    return ret;
}
any_serial_ret_t any_serial_write_blocking(any_serial_obj_t* serial_obj, const uint8_t* buf, const uint16_t size, unsigned int timeout_ms)
{
    any_serial_ret_t ret = ANY_SERIAL_RET_ERROR_NONE;
    serial_debug_log("any_serial_write_blocking is called!\n");
#ifdef ANY_SERIAL_OVERLAPPED
    if (NULL == serial_obj || NULL == buf) {
        serial_error_log("Error. Invalid input argument\n");
        return ANY_SERIAL_RET_ERROR_INVALID;
    }
    DWORD bytes_written = 0;
    BOOL result;
    if (serial_obj->is_writing) {
        serial_debug_log("Waiting for previous write to completed.\n");
        result = GetOverlappedResult(serial_obj->fd, &serial_obj->ovl_write, &bytes_written, TRUE);
        serial_obj->is_writing = false;
        if (!result) {
            serial_error_log("Previous write failed to complete.\n");
            return ANY_SERIAL_RET_ERROR_EFAULT;
        }
    }
    if (WriteFile(serial_obj->fd, buf, size, bytes_written/*NULL*/, &serial_obj->ovl_write)) {
        serial_debug_log("Write completed immediately\n");
        SetEvent(serial_obj->ovl_write.hEvent);
        return size;
    } 
    else if (ERROR_IO_PENDING == GetLastError()) {
        serial_debug_log("Waiting for write to complete");
#if 0
        if (false == GetOverlappedResult(serial_obj->fd, &serial_obj->ovl_write, &bytes_written, TRUE)) {
            if (ERROR_SEM_TIMEOUT == GetLastError()) {
                serial_debug_log("Write timed out\n");
                return ANY_SERIAL_RET_ERROR_TIMEOUT;
            }
            else {
                serial_error_log("GetOverlappedResult failed\n");
                return ANY_SERIAL_RET_ERROR_EFAULT;
            }
        }
#endif
        switch (WaitForSingleObject(serial_obj->ovl_write.hEvent, timeout_ms)) {
        case WAIT_OBJECT_0:
            if (!GetOverlappedResult(serial_obj->fd, &serial_obj->ovl_write, &bytes_written, FALSE)) {
                serial_error_log("WriteFile error [%d]\n", GetLastError());
                return ANY_SERIAL_RET_ERROR_EFAULT;
            }
            if (0 == bytes_written) {
                //CancelIo(serial_obj->fd);
                if (!PurgeComm(serial_obj->fd, PURGE_TXCLEAR | PURGE_RXCLEAR)) {
                    serial_error_log("Error. Write purge failed [%d]\n", GetLastError());
                    return ANY_SERIAL_RET_ERROR_EFAULT;
                }
            }
            break;
        case WAIT_TIMEOUT:
            serial_debug_log("Write timeout!\n");
            CancelIo(serial_obj->fd);
            return ANY_SERIAL_RET_ERROR_TIMEOUT;
            break;
        default:
            serial_error_log("WriteFile error [%d]\n", GetLastError());
            return ANY_SERIAL_RET_ERROR_EFAULT;
            break;
        }
        serial_debug_log("Write completed, [%d/%d] bytes written", bytes_written, size);
    }
    else {
        serial_error_log("WriteFile failed [%d]\n", GetLastError());
        return ANY_SERIAL_RET_ERROR_EFAULT;
    }
#endif
    return ret;
}
int any_serial_write_nonblocking (any_serial_obj_t* serial_obj, const uint8_t* buf, const uint16_t size)
{
    any_serial_ret_t ret = ANY_SERIAL_RET_ERROR_NONE;
    serial_debug_log("any_serial_write_nonblocking is called!\n");
    if (NULL == serial_obj || NULL == buf) {
        serial_error_log("Error. Invalid input argument\n");
        return ANY_SERIAL_RET_ERROR_INVALID;
    }
    DWORD bytes_written = 0;
#ifndef ANY_SERIAL_OVERLAPPED
    if (!WriteFile(serial_obj->fd, buf, (DWORD)(size), &bytes_written, NULL)) {
        serial_error_log("Error while writing to the serial port [%s]\n", serial_obj->opened_info.port);
        return ANY_SERIAL_RET_ERROR_EFAULT;
    }
    if (bytes_written != size) {
        serial_error_log("Error. Number of written bytes is not match with desired [ [%d] vs [%d] ]\n", bytes_written, size);
        if (!PurgeComm(serial_obj->fd, PURGE_TXCLEAR | PURGE_RXCLEAR)) {
            serial_error_log("Error. Write purge failed [%d]\n", GetLastError());
            //return ANY_SERIAL_RET_ERROR_EFAULT;
        }
        return ANY_SERIAL_RET_ERROR_EFAULT;
    }
#else
#   if 0
    // Create new write worker thread
    write_worker_thread_param_t* wwt = malloc(sizeof(write_worker_thread_param_t));

    wwt->serial_obj = serial_obj;
    wwt->buf = buf;
    wwt->size = size;
    HANDLE threadHandle = CreateThread(NULL, 0, write_thread_worker, wwt, 0, NULL);
#endif
    if (true == serial_obj->is_writing) {
        if (HasOverlappedIoCompleted(&serial_obj->ovl_write)) {
            serial_debug_log("Previous write completed\n");
            serial_obj->is_writing = false;
        }
        else {
            serial_error_log("Previous write not complete\n");
            return ANY_SERIAL_RET_ERROR_EFAULT;
        }
    }
    DWORD this_written = 0;
    DWORD lastError;
    while (bytes_written < size) {
        //data = data + bytes_written;
        DWORD expected_write = size - bytes_written;
        //printf("==> [%d]\n", expected_write);
        if (false == WriteFile(serial_obj->fd, buf + bytes_written, expected_write, &this_written, &serial_obj->ovl_write)) {
            lastError = GetLastError();
            if (ERROR_IO_PENDING == lastError) {
                if (HasOverlappedIoCompleted(&serial_obj->ovl_write)) {
                    serial_debug_log("Asynchronous write completed immediately\n");
                    serial_obj->is_writing = false;
                    bytes_written += expected_write;
                    continue;
                }
                else {
                    //serial_error_log("Asynchronous write running");
                    serial_debug_log("Asynchronous write running");
                    serial_obj->is_writing = true;
                    return bytes_written;
                }
            }
            else {
                serial_error_log("Error. Write failed\n");
                return ANY_SERIAL_RET_ERROR_EFAULT;
            }
        }
        else {
            serial_debug_log("Write done [%d => %d] \n", bytes_written, this_written);
            //bytes_written += this_written;
        }
    }
#endif
    return bytes_written;
}
any_serial_ret_t any_serial_read (any_serial_obj_t* serial_obj)
{
    any_serial_ret_t ret = ANY_SERIAL_RET_ERROR_NONE;
    serial_debug_log("any_serial_read is called\n");
    return ret;
}
#endif // #if defined _WIN32)