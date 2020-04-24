/*
 * wui_api.h
 * \brief   interface functions for Web User Interface library
 *
 *  Created on: April 22, 2020
 *      Author: joshy <joshymjose[at]gmail.com>
 */

#ifndef _WUI_API_H_
#define _WUI_API_H_

#include "ethvars.h"
#include <stdint.h>

#define FW_VER_STR_LEN    32
#define MAC_ADDR_STR_LEN  18
#define SER_NUM_STR_LEN   16
#define UUID_STR_LEN      32
#define PRI_STATE_STR_LEN 10

#ifdef __cplusplus
extern "C" {
#endif

    uint8_t printer_type;                  // Printer type (defined in CMakeLists.txt)
    uint8_t printer_version;               // Printer varsion (Stored in FLASH)
    char firmware_version[FW_VER_STR_LEN]; // Full project's version (4.0.3-BETA+1035.PR111.B4)
    char mac_address[MAC_ADDR_STR_LEN];    // MAC address string "MM:MM:MM:SS:SS:SS"
    char serial_number[SER_NUM_STR_LEN];   // serial number without first four characters "CZPX" (total 15 chars, zero terminated)
    char mcu_uuid[UUID_STR_LEN];           // Unique identifier (96bits) into string format "%08lx-%08lx-%08lx"
    char printer_state[PRI_STATE_STR_LEN]; // state of the printer, have to be set in wui
} printer_info_t;

typedef struct {
/*!****************************************************************************
* \brief saves the Ethernet specific parameters to non-volatile memory
*
* \param [in] ETH_config_t* pointer to struct with parameters
*
* \return   uint32_t    error value
*
* \retval   0 if successful
*****************************************************************************/
uint32_t save_eth_params(ETH_config_t * config);

/*!****************************************************************************
* \brief load from ini file Ethernet specific parameters
*
* \param [out] ETH_Config_t* pointer to struct with parameters
*
* \return   uint32_t    error value
*
* \retval   0 if successful
*****************************************************************************/
void load_ini_params(ETH_config_t * config);

void get_printer_info(printer_info_t *printer_info);
#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* _WUI_API_H_ */
