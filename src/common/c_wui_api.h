/*
 * c_wui_api header
 * \brief   interface functions for WUI
 *
 *  Created on: April 21, 2020
 *      Author: Migi <michal.rudolf[at]prusa3d.cz>
 */

#ifndef COMMON_WUI_API_H
#define COMMON_WUI_API_H

#include <stdbool.h>
#include "netvars.h"

typedef int (*buddy_lan_ini_handler)(void* user, const char* section,
                           const char* name, const char* value);

// WUI STATUS INTERFACE
ETH_STATUS_t wui_eth_status(void);
bool wui_dhcp_supplied_addrs(void);

// NETIF SETTINGS INTERFACE
void wui_update_netvars(uint32_t msk);
const char * wui_mac_addr_str(void);
const char * wui_addr_ip4_str(void);
const char * wui_msk_ip4_str(void);
const char * wui_gw_ip4_str(void);
const char * wui_connect_ip4_str(void);
const char * wui_connect_token(void);
const char * wui_hostname(void);
uint8_t wui_lan_flg(void);
void * wui_get_tmp_netconfig(void);
buddy_lan_ini_handler wui_get_ini_load_handler(void);
uint8_t wui_set_loaded_netvars(void);

void wui_lan_off(void);
void wui_lan_on(void);
void wui_lan_set_static(void);
void wui_lan_set_dhcp(void);

#endif //COMMON_WUI_API_H