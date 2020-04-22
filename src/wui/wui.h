/*
 * wui.h
 * \brief main interface functions for Web User Interface (WUI) thread
 *
 *  Created on: Dec 12, 2019
 *      Author: joshy
 */

#ifndef SRC_WUI_WUI_H_
#define SRC_WUI_WUI_H_

#define BUDDY_WEB_STACK_SIZE 512
#include <marlin_vars.h>
#include "cmsis_os.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!****************************************************************************
* \brief Webserver thread function
*
*****************************************************************************/
void StartWebServerTask(void const *argument);

extern osMessageQId tcp_wui_queue_id;
extern osSemaphoreId tcp_wui_semaphore_id;
extern osMutexId wui_thread_mutex_id;

typedef enum {
    CMD_UNKNOWN,
    CMD_SEND_INFO,
    CMD_START_PRINT,
    CMD_PAUSE_PRINT,
    CMD_STOP_PRINT,
    CMD_RESUME_PRINT,
    CMD_SET,
    CMD_GET,
} HTTPC_HIGH_LVL_CMD;

typedef enum {
    HIGH_LVL_CMD,
    LOW_LVL_CMD,
} CMD_LVL;

typedef struct {
    char gcode_cmd[100];
} wui_low_cmd_t;

typedef struct {
    HTTPC_HIGH_LVL_CMD cmd;
    char arg[100];
    //TODO: arg type depends on the command (vartiant8_t args[HIGH_CMD_MAX_ARGS_CNT] or union)
} wui_high_cmd_t;


#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* SRC_WUI_WUI_H_ */
