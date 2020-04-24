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
#include <stdint.h>

#define FW_VER_STR_LEN    32
#define MAC_ADDR_STR_LEN  18
#define SER_NUM_STR_LEN   16
#define UUID_STR_LEN      32
#define PRI_STATE_STR_LEN 10

// LAN FLAGS
#define LAN_ONOFF_FLAG_POS     (1 << 0)
#define LAN_FLAG_ADDR_TYPE_POS (1 << 1)

#define ETHVAR_MSK(n_id) ((uint16_t)1 << (n_id))
#define ETHVAR_STATIC_LAN_ADDRS \
    (ETHVAR_MSK(ETHVAR_LAN_IP4_ADDR) | ETHVAR_MSK(ETHVAR_LAN_IP4_MSK) | ETHVAR_MSK(ETHVAR_LAN_IP4_GW))

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ETHVAR_LAN_FLAGS,
    ETHVAR_HOSTNAME,
    ETHVAR_CONNECT_TOKEN,
    ETHVAR_LAN_IP4_ADDR,
    ETHVAR_LAN_IP4_MSK,
    ETHVAR_LAN_IP4_GW,
    ETHVAR_DNS1,
    ETHVAR_DNS2,
    ETHVAR_CONNECT_IP4,
} ETHVAR_t;

typedef struct {
    uint8_t printer_type;                  // Printer type (defined in CMakeLists.txt)
    uint8_t printer_version;               // Printer varsion (Stored in FLASH)
    char firmware_version[FW_VER_STR_LEN]; // Full project's version (4.0.3-BETA+1035.PR111.B4)
    char mac_address[MAC_ADDR_STR_LEN];    // MAC address string "MM:MM:MM:SS:SS:SS"
    char serial_number[SER_NUM_STR_LEN];   // serial number without first four characters "CZPX" (total 15 chars, zero terminated)
    char mcu_uuid[UUID_STR_LEN];           // Unique identifier (96bits) into string format "%08lx-%08lx-%08lx"
    char printer_state[PRI_STATE_STR_LEN]; // state of the printer, have to be set in wui
} printer_info_t;

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

/*!****************************************************************************
* \brief saves the Ethernet specific parameters to non-volatile memory
*
* \param [in] ETH_Config_t* pointer to struct with parameters
*
* \return   uint32_t    error value
*
* \retval   0 if successful
*****************************************************************************/
uint32_t save_eth_params(ETH_Config_t *config_t);

/*!****************************************************************************
* \brief load from ini file Ethernet specific parameters
*
* \param [out] ETH_Config_t* pointer to struct with parameters
*
* \return   uint32_t    error value
*
* \retval   0 if successful
*****************************************************************************/
void load_ini_params(ETH_Config_t *config_t);

void get_printer_info(printer_info_t *printer_info);
#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* _WUI_API_H_ */
