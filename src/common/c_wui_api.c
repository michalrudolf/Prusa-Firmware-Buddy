/*
 * c_wui_api.c
 * \brief
 *
 *  Created on: April 21, 2020
 *      Author: Migi <michal.rudolf[at]prusa3d.cz>
 */

#include "c_wui_api.h"
#include "netif_settings.h"
#include "wui_eeprom_api.h"
#include "dhcp.h"

ETH_STATUS_t wui_eth_status(void) { return eth_status(); }

bool wui_dhcp_supplied_addrs(void){
    return dhcp_supplied_address(&eth0);
}

void * wui_get_tmp_netconfig(void) { 
    tmp_netconfig.lan.flg = netconfig.lan.flg;
#if BUDDY_ENABLE_DNS
    tmp_netconfig.dns1_ip4.addr = tmp_netconfig.dns2_ip4.addr = 0;
#endif //BUDDY_ENABLE_DNS
    tmp_netconfig.set_flg = 0;
    return &tmp_netconfig; 
}

buddy_lan_ini_handler wui_get_ini_load_handler(void) { return load_netconfig_ini_handler; }
uint8_t wui_set_loaded_netvars(void)    { return set_loaded_netconfig(); }
void wui_update_netvars(uint32_t msk)   { update_netconfig(msk); }
const char * wui_mac_addr_str(void)     { return get_mac_addr_str(); }
const char * wui_addr_ip4_str(void)     { return get_addr_ip4_str(); }
const char * wui_msk_ip4_str(void)      { return get_msk_ip4_str(); }
const char * wui_gw_ip4_str(void)       { return get_gw_ip4_str(); }
const char * wui_connect_ip4_str(void)  { return get_connect_ip4_str(); }
const char * wui_connect_token(void)    { return netconfig.connect.token; }
const char * wui_hostname(void)         { return netconfig.hostname; }
uint8_t wui_lan_flg(void)           { return netconfig.lan.flg; }
void wui_lan_off(void)              { lan_turn_off(); }
void wui_lan_on(void)               { lan_turn_on(); }
void wui_lan_set_static(void)       { lan_set_static(); }
void wui_lan_set_dhcp(void)         { lan_set_dhcp(); }