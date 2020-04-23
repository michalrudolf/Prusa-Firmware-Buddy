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

typedef enum {
    SET_CONNECT_IP4,
    SET_CONNECT_TOKEN,
    SET_CONNECT_PORT,
    SET_LAN_IP4_ADDR,
    SET_LAN_IP4_MSK,
    SET_LAN_IP4_GW,
    SET_LAN_NETIF_UP,
    SET_LAN_NETIF_DOWN,
    SET_LAN_DHCP,
    SET_LAN_STATIC,
    SET_DNS1_IP4,
    SET_DNS2_IP4,
    SET_HOSTNAME,
} CONNECT_SET_OPT;

typedef struct {
    CMD_LVL lvl;
    HTTPC_HIGH_LVL_CMD high_lvl_cmd;
    uint32_t set_flg;
    char arg[100];
    // TODO: other possible arg's data types
} wui_cmd_t;


#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* SRC_WUI_WUI_H_ */
