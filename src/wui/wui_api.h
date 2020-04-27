/*
 * wui_api.h
 * \brief   interface functions for Web User Interface library
 *
 *  Created on: April 22, 2020
 *      Author: joshy <joshymjose[at]gmail.com>
 */

#ifndef _WUI_API_H_
#define _WUI_API_H_

#include <stdint.h>
#include "ip_addr.h"

#define FW_VER_STR_LEN          32          // length of full Firmware version string
#define MAC_ADDR_STR_LEN        18          // length of mac address string ("MM:MM:MM:SS:SS:SS" + 0)
#define SER_NUM_STR_LEN         16          // length of serial number string
#define UUID_STR_LEN            32          // length of unique identifier string
#define PRI_STATE_STR_LEN       10          // length of printer state string

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

#define ETHVAR_MSK(n_id) ((uint32_t)1 << (n_id))
#define ETHVAR_STATIC_LAN_ADDRS \
    (ETHVAR_MSK(ETHVAR_LAN_ADDR_IP4) | ETHVAR_MSK(ETHVAR_LAN_MSK_IP4) | ETHVAR_MSK(ETHVAR_LAN_GW_IP4))


#define ETHVAR_EEPROM_CONFIG \
    (ETHVAR_STATIC_LAN_ADDRS | ETHVAR_MSK(ETHVAR_LAN_FLAGS) | ETHVAR_MSK(ETHVAR_HOSTNAME) | ETHVAR_MSK(ETHVAR_CONNECT_IP4) | ETHVAR_MSK(ETHVAR_CONNECT_TOKEN))

#ifdef __cplusplus
extern "C" {
#endif

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
    uint32_t var_mask;                      // mask for setting ethvars
} ETH_config_t;

typedef struct {
    uint8_t printer_type;                  // Printer type (defined in CMakeLists.txt)
    uint8_t printer_version;               // Printer varsion (Stored in FLASH)
    char firmware_version[FW_VER_STR_LEN]; // Full project's version (4.0.3-BETA+1035.PR111.B4)
    char mac_address[MAC_ADDR_STR_LEN];    // MAC address string "MM:MM:MM:SS:SS:SS"
    char serial_number[SER_NUM_STR_LEN];   // serial number without first four characters "CZPX" (total 15 chars, zero terminated)
    char mcu_uuid[UUID_STR_LEN];           // Unique identifier (96bits) into string format "%08lx-%08lx-%08lx"
    char printer_state[PRI_STATE_STR_LEN]; // state of the printer, have to be set in wui
} printer_info_t;

extern ETH_config_t ethconfig;          // configurations of the network - declared in netif_settings.c
extern ETH_config_t tmp_ethconfig;      // temporary configuration for loading - declared in netif_settings.c
extern struct netif eth0;               // network interface for ETH - declared in netif_settings.c

/*!****************************************************************************
* \brief saves the Ethernet specific parameters to non-volatile memory
*
* \param [in] mask of parameters to set from static ethconfig to non-volatile memory
*
* \return   uint32_t    error value
*
* \retval   0 if successful
*****************************************************************************/
uint32_t save_eth_params(uint32_t mask);

/*!****************************************************************************
* \brief loads the Ethernet specific parameters from non-volatile memory
*
* \param [out] mask of parameters to get from memory to static ethconfig structure
*
* \return   uint32_t    error value
*
* \retval   0 if successful
*****************************************************************************/
uint32_t load_eth_params(uint32_t mask);

/*!****************************************************************************
* \brief load from ini file Ethernet specific parameters
*
* \return   uint32_t    error value
*
* \retval   1 if successful
*****************************************************************************/
uint32_t load_ini_params(void);

/*!****************************************************************************
* \brief access user defined addresses in memory and aquire vital printer info
*
* \param [out] printer_info* pointer to struct with storage for printer info
*
* \retval   0 if successful
*****************************************************************************/
void get_printer_info(printer_info_t *printer_info);

/*!****************************************************************************
* \brief parses MAC address from device's memory to static string
*
* \param [out] destination (static MAC address string)
*****************************************************************************/
void parse_MAC_address(char * dest);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* _WUI_API_H_ */
