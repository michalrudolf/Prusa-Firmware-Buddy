#include "wui_helper_funcs.h"
#include "dbg.h"
#include "wui.h"
#include "string.h"

extern osMessageQId tcp_wui_mpool_id;
extern osSemaphoreId tcp_wui_semaphore_id;

void send_request_to_wui(const char *request) {

    osSemaphoreWait(tcp_wui_semaphore_id, osWaitForever);
    if (0 != tcp_wui_queue_id) // queue valid
    {
        uint32_t q_space = osMessageAvailableSpace(tcp_wui_queue_id);
        if (q_space) {
            wui_cmd_t *mptr;
            mptr = osPoolAlloc(tcp_wui_mpool_id);
            strlcpy(mptr->gcode_cmd, request, 100);
            osMessagePut(tcp_wui_queue_id, (uint32_t)mptr, osWaitForever); // Send Message
            osDelay(100);
        } else {
            _dbg("message queue to wui full");
        }
    }
    osSemaphoreRelease(tcp_wui_semaphore_id);
}
