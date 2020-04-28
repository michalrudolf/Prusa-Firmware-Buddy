/*
 * screen_lan_settings.h
 *
 *  Created on: Dec 11, 2019
 *      Author: Migi
 */

#ifndef SRC_GUI_SCREEN_LAN_SETTINGS_H_
#define SRC_GUI_SCREEN_LAN_SETTINGS_H_

#include "gui.h"
#include "config.h"
#include "screen_menu.h"
#include "eeprom.h"

#define plsd              ((screen_lan_settings_data_t *)screen->pdata)
#define MAC_ADDR_STR_SIZE 18

typedef enum {
    NETVAR_LAN_FLAGS,
    NETVAR_HOSTNAME,
    NETVAR_CONNECT_TOKEN,
    NETVAR_LAN_IP4_ADDR,
    NETVAR_LAN_IP4_MSK,
    NETVAR_LAN_IP4_GW,
    NETVAR_DNS1,
    NETVAR_DNS2,
    NETVAR_CONNECT_IP4,
} NETVAR_t;
#define NETVAR_MSK(n_id) ((uint16_t)1 << (n_id))
#define NETVAR_STATIC_LAN_ADDRS \
    (NETVAR_MSK(NETVAR_LAN_IP4_ADDR) | NETVAR_MSK(NETVAR_LAN_IP4_MSK) | NETVAR_MSK(NETVAR_LAN_IP4_GW))

typedef struct {
    window_frame_t root;
    window_header_t header;
    window_menu_t menu;
    menu_item_t *items;

    window_text_t text;
    char mac_addr_str[MAC_ADDR_STR_SIZE];
} screen_lan_settings_data_t;

extern screen_t screen_lan_settings;
extern screen_t *const pscreen_lan_settings;

#endif /* SRC_GUI_SCREEN_LAN_SETTINGS_H_ */
