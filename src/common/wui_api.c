/*
 * wui_api.h
 * \brief   interface functions for Web User Interface library
 *
 *  Created on: April 22, 2020
 *      Author: joshy <joshymjose[at]gmail.com>
 */
#include "wui_api.h"
#include "version.h"
#include "otp.h"
#include <string.h>
#include <stdio.h>

void get_eth_params(wui_eth_params_s eth_params) {
}

void get_printer_info(printer_info_t * printer_info){


    //| 0x0802002F - 0x0802002F | 1 B | Printer type

    //| 0x08020030 - 0x08020030 | 1 B | Printer version
    
    // FIRMWARE VERSION
    strlcpy(printer_info->firmware_version, project_version_full, FW_VER_STR_LEN);
    // PRINTER TYPE
    printer_info->printer_type = PRINTER_TYPE;
    // PRINTER_VERSION
    // TODO: couldn't find PRINTER_VERSION (MINI is 1 and MK4 is 4)
    
    char serial_numbers[OTP_SERIAL_NUMBER_SIZE];
    volatile uint8_t mac_addr[OTP_MAC_ADDRESS_SIZE];
    uint32_t uuid[3];

    // MAC ADDRESS
    for (uint8_t i = 0; i < OTP_MAC_ADDRESS_SIZE; i++){
        mac_addr[i] = *(volatile uint8_t *)(OTP_MAC_ADDRESS_ADDR + i);
    }
    snprintf(printer_info->mac_address, MAC_ADDR_STR_LEN, "%02x:%02x:%02x:%02x:%02x:%02x", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    // SERIAL NUMBER
    for (int i = 0; i < OTP_SERIAL_NUMBER_SIZE; i++) {
        serial_numbers[i] = *(volatile char *)(OTP_SERIAL_NUMBER_ADDR + i);
    }
    strlcpy(printer_info->serial_number, serial_numbers, SER_NUM_STR_LEN);
    // UUID - 96 bits
    for (int i = 0; i < 3; i++){
        uuid[i] = *(uint32_t *)(OTP_STM32_UUID_ADDR + i);
    }
    snprintf(printer_info->mcu_uuid, UUID_STR_LEN, "%08lx-%08lx-%08lx", uuid[0], uuid[1], uuid[2]);
}