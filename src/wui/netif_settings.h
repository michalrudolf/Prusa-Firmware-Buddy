#ifndef NETIF_SETTINGS_H
#define NETIF_SETTINGS_H

#include <stdint.h>
#include "lwip.h"
#define NETVAR_MSK(n_id) ((uint16_t)1 << (n_id))
#define NETVAR_STATIC_LAN_ADDRS \
    (NETVAR_MSK(NETVAR_LAN_IP4_ADDR) | NETVAR_MSK(NETVAR_LAN_IP4_MSK) | NETVAR_MSK(NETVAR_LAN_IP4_GW))

#define NETVAR_EEPROM_CONFIG \
    (NETVAR_STATIC_LAN_ADDRS | NETVAR_MSK(NETVAR_LAN_FLAGS) | NETVAR_MSK(NETVAR_HOSTNAME) | NETVAR_MSK(NETVAR_CONNECT_PORT) | NETVAR_MSK(NETVAR_CONNECT_IP4) | NETVAR_MSK(NETVAR_CONNECT_TOKEN))

#define CHANGE_LAN_TO_STATIC(flg)   (flg |= LAN_MSK_TYPE)   // flip lan type flg to STATIC
#define CHANGE_LAN_TO_DHCP(flg)     (flg &= ~LAN_MSK_TYPE)   // flip lan type flg to DHCP
#define TURN_LAN_OFF(flg)           (flg |= LAN_MSK_ONOFF)   // flip lan switch flg to OFF
#define TURN_LAN_ON(flg)            (flg &= ~LAN_MSK_ONOFF)   // flip lan switch flg to ON


typedef enum{
    NETVAR_LAN_FLAGS,
    NETVAR_HOSTNAME,
    NETVAR_LAN_IP4_ADDR,
    NETVAR_LAN_IP4_MSK,
    NETVAR_LAN_IP4_GW,
    NETVAR_DNS1_IP4,
    NETVAR_DNS2_IP4,
    NETVAR_CONNECT_PORT,
    NETVAR_CONNECT_IP4,
    NETVAR_CONNECT_TOKEN,
    NETVAR_MAC_ADDR,
} NETVAR_t;
typedef struct {
    char token[CONNECT_TOKEN_SIZE + 1];
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
    char hostname[LAN_HOSTNAME_MAX_LEN + 1];
    connect_t connect;
    lan_t lan;
    ip4_addr_t dns1_ip4;
    ip4_addr_t dns2_ip4;
    uint16_t set_flg;
} networkconfig_t;

extern networkconfig_t netconfig;

void lan_set_dhcp(void);
void lan_set_static(void);
uint8_t set_loaded_netconfig(networkconfig_t * tmp_config);
const char * mac_addr_str(void);
const char * addr_ip4_str(void);
const char * msk_ip4_str(void);
const char * gw_ip4_str(void);
const char * connect_ip4_str(void);
void update_netconfig(uint32_t msk);
void lan_turn_off(void);
void lan_turn_on(void);



#endif //NETIF_SETTINGS_H