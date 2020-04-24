/*
 * ethvars.h
 * \brief   structures, enums and defines needed for ETH manupulation
 *
 *  Created on: April 24, 2020
 *      Author: Migi <michal.rudolf[at]prusa3d.cz>
 */

#ifndef ETHVARS_H
#define ETHVARS_H

#include <stdint.h>
#include "ip_addr.h"

#define ETH_HOSTNAME_LEN        20          // ethernet hostname MAX length
#define CONNECT_TOKEN_LEN       20          // CONNECT security token length

#define LAN_FLAG_ONOFF_POS      (1 << 0)    // position of ONOFF switch in lan.flag
#define LAN_FLAG_TYPE_POS       (1 << 1)    // position of DHCP/STATIC switch in lan.flag

#define IS_LAN_OFF(flg)         (flg & LAN_FLAG_ONOFF_POS)           // returns true if flag is set to OFF
#define IS_LAN_ON(flg)          (!IS_LAN_OFF(flg))              // returns true if flag is set to ON
#define IS_LAN_STATIC(flg)      (flg & LAN_FLAG_TYPE_POS)            // returns true if flag is set to STATIC
#define IS_LAN_DHCP(flg)        (!IS_LAN_STATIC(flg))           // returns true if flag is set to DHCP

#define CHANGE_LAN_TO_STATIC(flg)   (flg |= LAN_FLAG_TYPE_POS)      // flip lan type flg to STATIC
#define CHANGE_LAN_TO_DHCP(flg)     (flg &= ~LAN_FLAG_TYPE_POS)     // flip lan type flg to DHCP
#define TURN_LAN_ON(flg)            (flg &= ~LAN_FLAG_ONOFF_POS)    // flip lan switch flg to ON
#define TURN_LAN_OFF(flg)           (flg |= LAN_FLAG_ONOFF_POS)     // flip lan switch flg to OFF

#define ETHVAR_MSK(n_id) ((uint16_t)1 << (n_id))
#define ETHVAR_STATIC_LAN_ADDRS \
    (ETHVAR_MSK(ETHVAR_LAN_ADDR_IP4) | ETHVAR_MSK(ETHVAR_LAN_MSK_IP4) | ETHVAR_MSK(ETHVAR_LAN_GW_IP4))


#define ETHVAR_EEPROM_CONFIG \
    (ETHVAR_STATIC_LAN_ADDRS | ETHVAR_MSK(ETHVAR_LAN_FLAGS) | ETHVAR_MSK(ETHVAR_HOSTNAME) | ETHVAR_MSK(ETHVAR_CONNECT_IP4) | ETHVAR_MSK(ETHVAR_CONNECT_TOKEN))

typedef enum {
    ETHVAR_LAN_FLAGS,       // uint8_t, lan.flag
    ETHVAR_HOSTNAME,        // char[20 + 1], hostname
    ETHVAR_CONNECT_TOKEN,   // char[20 + 1], connect.token
    ETHVAR_LAN_ADDR_IP4,    // ip4_addr_t, lan.addr_ip4
    ETHVAR_LAN_MSK_IP4,     // ip4_addr_t, lan.msk_ip4
    ETHVAR_LAN_GW_IP4,      // ip4_addr_t, lan.gw_ip4
    ETHVAR_CONNECT_IP4,     // ip4_addr_t, connect.ip4
} ETHVAR_t;

typedef enum {
    ETH_UNLINKED,       // ETH cabel is unlinked
    ETH_NETIF_DOWN,     // ETH interface is DOWN
    ETH_NETIF_UP,       // ETH interface is UP
} ETH_STATUS_t;

typedef struct {
    char token[CONNECT_TOKEN_LEN + 1];  // security token: 20 chars
    ip4_addr_t ip4;                     // user defined CONNECT ip4
} connect_t; 

typedef struct {
    uint8_t flag;                       // lan flags: pos0 = switch(ON=0, OFF=1), pos1 = type(DHCP=0, STATIC=1)
    ip4_addr_t addr_ip4;                // user defined static ip4 address
    ip4_addr_t msk_ip4;                 // user defined ip4 netmask
    ip4_addr_t gw_ip4;                  // user define ip4 default gateway
} lan_t;

typedef struct {
    char hostname[ETH_HOSTNAME_LEN + 1];    // ETH hostname: MAX 20 chars
    connect_t connect;                      // user defined CONNECT configurations
    lan_t lan;                              // user defined CONNECT configurations
    uint16_t set_flag;                      // mask for setting ethvars
} ETH_config_t;

#endif //ETHVARS_H