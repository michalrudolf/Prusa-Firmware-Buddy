#ifndef NETIF_SETTINGS_H
#define NETIF_SETTINGS_H

#include <stdint.h>
#include "lwip.h"

#define NETVAR_MSK(n_id) ((uint16_t)1 << (n_id))
#define NETVAR_STATIC_LAN_ADDRS \
    (NETVAR_MSK(NETVAR_LAN_IP4_ADDR) | NETVAR_MSK(NETVAR_LAN_IP4_MSK) | NETVAR_MSK(NETVAR_LAN_IP4_GW))

#define CHANGE_LAN_TO_STATIC(flg)   (flg |= LAN_MSK_TYPE)   // flip lan type flg to STATIC
#define CHANGE_LAN_TO_DHCP(flg)     (flg &= ~LAN_MSK_TYPE)   // flip lan type flg to DHCP
#define CHANGE_LAN_TO_OFF(flg)      (flg |= LAN_MSK_ONOFF)   // flip lan switch flg to OFF
#define CHANGE_LAN_TO_ON(flg)       (flg &= ~LAN_MSK_ONOFF)   // flip lan switch flg to ON


typedef enum{
    NETVAR_LAN_FLAGS,
    NETVAR_HOSTNAME,
    NETVAR_CONNECT_TOKEN,
    NETVAR_LAN_IP4_ADDR,
    NETVAR_LAN_IP4_MSK,
    NETVAR_LAN_IP4_GW,
    NETVAR_CONNECT_IP4,
    NETVAR_CONNECT_PORT,
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
#ifdef BUDDY_ENABLE_CONNECT
    connect_t connect;
#endif // BUDDY_ENABLE_CONNECT
    lan_t lan;
    uint16_t set_flg;
} networkconfig_t;

extern networkconfig_t netconfig;


#endif //NETIF_SETTINGS_H