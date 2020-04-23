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

#define WUI_VAR_MSK(id)         ((uint32_t)1 << id)
#define HIGH_CMD_MAX_ARGS_CNT   5

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

typedef enum {
    VAR_CONNECT_IP4,
    VAR_CONNECT_TOKEN,
    VAR_CONNECT_PORT,
    VAR_LAN_IP4_ADDR,
    VAR_LAN_IP4_MSK,
    VAR_LAN_IP4_GW,
    VAR_LAN_NETIF_UP,
    VAR_LAN_NETIF_DOWN,
    VAR_LAN_DHCP,
    VAR_LAN_STATIC,
    VAR_DNS1_IP4,
    VAR_DNS2_IP4,
    VAR_HOSTNAME,
} CONNECT_CUSTOM_VARS;

typedef struct {
    CMD_LVL lvl;
    HTTPC_HIGH_LVL_CMD high_lvl_cmd;
    uint32_t var_msk;
    char arg[100];
    // TODO: other possible arg's data types
} wui_cmd_t;


#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* SRC_WUI_WUI_H_ */
