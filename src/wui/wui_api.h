/*
 * wui_api.h
 * \brief   interface functions for Web User Interface library
 *
 *  Created on: April 22, 2020
 *      Author: joshy <joshymjose[at]gmail.com>
 */

#ifndef _WUI_API_H_
#define _WUI_API_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define FW_VER_STR_LEN      32
#define MAC_ADDR_STR_LEN    18
#define SER_NUM_STR_LEN     16
#define UUID_STR_LEN        32
#define PRI_STATE_STR_LEN   10
typedef struct {

    uint8_t printer_type;
    uint8_t printer_version;
    char firmware_version[FW_VER_STR_LEN];
    char mac_address[MAC_ADDR_STR_LEN];
    char serial_number[SER_NUM_STR_LEN];
    char mcu_uuid[UUID_STR_LEN];
    char printer_state[PRI_STATE_STR_LEN];
} printer_info_t;

typedef struct {

} wui_eth_params_s;

/* \breif function to pass ethernet parameters for WUI
*/
void get_eth_params(wui_eth_params_s eth_params);

void get_printer_info(printer_info_t * printer_info);
#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* _WUI_API_H_ */
