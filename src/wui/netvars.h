/*
 * netvars.c
 * \brief   NETVARS for wui api
 *
 *  Created on: April 21, 2020
 *      Author: Migi <michal.rudolf[at]prusa3d.cz>
 */
#ifndef NETVARS_H
#define NETVARS_H

#include <stdint.h>
#include <variant8.h>

#define LAN_MSK_ONOFF       1 //EEPROM mask for user-defined settings (SW turn OFF/ON of LAN)
#define LAN_MSK_TYPE        2 //EEPROM mask for user-defined settings (Switch between dhcp and static)

#define CONNECT_TOKEN_LEN       20
#define LAN_HOSTNAME_LEN        20

#define IS_LAN_OFF(flg)         (flg & LAN_MSK_ONOFF)
#define IS_LAN_ON(flg)          (!IS_LAN_OFF(flg))
#define IS_LAN_STATIC(flg)      (flg & LAN_MSK_TYPE)
#define IS_LAN_DHCP(flg)        (!IS_LAN_STATIC(flg))

#define NETVAR_MSK(n_id) ((uint16_t)1 << (n_id))
#define NETVAR_STATIC_LAN_ADDRS \
    (NETVAR_MSK(NETVAR_LAN_IP4_ADDR) | NETVAR_MSK(NETVAR_LAN_IP4_MSK) | NETVAR_MSK(NETVAR_LAN_IP4_GW))

#define NETVAR_EEPROM_CONFIG \
    (NETVAR_STATIC_LAN_ADDRS | NETVAR_MSK(NETVAR_LAN_FLAGS) | NETVAR_MSK(NETVAR_HOSTNAME) | NETVAR_MSK(NETVAR_DNS1_IP4) | NETVAR_MSK(NETVAR_DNS2_IP4) | NETVAR_MSK(NETVAR_CONNECT_PORT) | NETVAR_MSK(NETVAR_CONNECT_IP4) | NETVAR_MSK(NETVAR_CONNECT_TOKEN))

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
} NETVAR_t;

typedef enum {
    ETH_UNLINKED,
    ETH_NETIF_DOWN,
    ETH_NETIF_UP,
} ETH_STATUS_t;

#endif //NETVARS_H