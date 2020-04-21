/*
 * netif_settings.c
 * \brief   Functions for setting net interface
 *
 *  Created on: April 21, 2020
 *      Author: Migi <michal.rudolf[at]prusa3d.cz>
 */
#ifndef NETIF_SETTINGS_H
#define NETIF_SETTINGS_H

#include "ip_addr.h"
#include "netvars.h"

#define CHANGE_LAN_TO_STATIC(flg)   (flg |= LAN_MSK_TYPE)   // flip lan type flg to STATIC
#define CHANGE_LAN_TO_DHCP(flg)     (flg &= ~LAN_MSK_TYPE)   // flip lan type flg to DHCP
#define TURN_LAN_OFF(flg)           (flg |= LAN_MSK_ONOFF)   // flip lan switch flg to OFF
#define TURN_LAN_ON(flg)            (flg &= ~LAN_MSK_ONOFF)   // flip lan switch flg to ON


#define BUDDY_ENABLE_DNS  LWIP_DNS

typedef struct {
    char token[CONNECT_TOKEN_LEN + 1];
    uint32_t port;
    ip4_addr_t ip4;
} connect_t; 

typedef struct {
    uint8_t flg;
    ip4_addr_t addr_ip4;
    ip4_addr_t msk_ip4;
    ip4_addr_t gw_ip4;
} lan_t;

typedef struct {
    char hostname[LAN_HOSTNAME_LEN + 1];
    connect_t connect;
    lan_t lan;
    ip4_addr_t dns1_ip4;
    ip4_addr_t dns2_ip4;
    uint16_t set_flg;
} networkconfig_t;

extern struct netif eth0;
extern networkconfig_t netconfig;
extern networkconfig_t tmp_netconfig;

void lan_set_dhcp(void);
void lan_set_static(void);
uint8_t set_loaded_netconfig(void);
const char * get_mac_addr_str(void);
const char * get_addr_ip4_str(void);
const char * get_msk_ip4_str(void);
const char * get_gw_ip4_str(void);
const char * get_connect_ip4_str(void);
void update_netconfig(uint32_t msk);
void lan_turn_off(void);
void lan_turn_on(void);
void parse_MAC_addr();
int load_netconfig_ini_handler(void *user, const char *section, const char *name, const char *value);
ETH_STATUS_t eth_status(void);


#endif //NETIF_SETTINGS_H