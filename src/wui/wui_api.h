/*
 * wui_api.h
 * \brief   interface functions for Web User Interface library
 *
 *  Created on: April 22, 2020
 *      Author: joshy <joshymjose[at]gmail.com>
 */

#ifndef _WUI_API_H_
#define _WUI_API_H_

#include "lwip/netif.h"
#include "eeprom.h"

#ifdef __cplusplus
extern "C" {
#endif

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
    uint8_t lan_flag;
    char hostname[LAN_HOSTNAME_MAX_LEN + 1];
    char connect_token[CONNECT_TOKEN_SIZE + 1];
    ip4_addr_t connect_ip4;
    ip4_addr_t lan_ip4_addr;
    ip4_addr_t lan_ip4_msk;
    ip4_addr_t lan_ip4_gw;
    uint16_t set_flag;
} ETH_Config_t;

/* \breif function to save Ethernt configuration to non-volatile memory
*/
uint32_t eeprom_set_eth_params(ETH_Config_t *config_t);

void load_ini_params(ETH_Config_t *config_t);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* _WUI_API_H_ */
