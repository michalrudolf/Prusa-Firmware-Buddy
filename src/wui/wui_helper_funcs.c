#include "wui_helper_funcs.h"
#include "http_client.h"
#include <stdio.h>
#include <string.h>
#include "eeprom.h"
#include "ip4_addr.h"
#include "dbg.h"

extern osMessageQId tcp_wui_mpool_id;
extern osSemaphoreId tcp_wui_semaphore_id;

uint8_t send_request_to_wui(CMD_LVL cmd_lvl, void * req_ptr) {

    osSemaphoreWait(tcp_wui_semaphore_id, osWaitForever);
    if (0 != tcp_wui_queue_id) // queue valid
    {   
        uint32_t q_space = osMessageAvailableSpace(tcp_wui_queue_id);
        if (cmd_lvl == HIGH_LVL_CMD){
            if (q_space < sizeof(wui_high_cmd_t)){
                _dbg("message queue to wui full");
                return 1;
            }
            wui_high_cmd_t * high_ptr = osPoolAlloc(tcp_wui_mpool_id);
            memcpy(high_ptr, req_ptr, sizeof(wui_high_cmd_t));
            osMessagePut(tcp_wui_queue_id, (uint32_t)high_ptr, osWaitForever); // Send Message
            osDelay(100);
        } else if (cmd_lvl == LOW_LVL_CMD){
            if (q_space < sizeof(wui_low_cmd_t)){
                _dbg("message queue to wui full");
                return 1;
            }
            wui_low_cmd_t * low_ptr = osPoolAlloc(tcp_wui_mpool_id);
            strlcpy(low_ptr->gcode_cmd, (const char*)req_ptr, 100);
            osMessagePut(tcp_wui_queue_id, (uint32_t)low_ptr, osWaitForever); // Send Message
            osDelay(100);
        } else {
            _dbg("unknown cmd level BUG");
        }
    }
    osSemaphoreRelease(tcp_wui_semaphore_id);
    return 0;
}