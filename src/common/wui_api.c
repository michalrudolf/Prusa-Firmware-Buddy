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
#include "ini_handler.h"
#include "eeprom.h"
#include "string.h"
#include "netifapi.h"
#include "dhcp.h"

#define PRINTER_TYPE_ADDR    0x0802002F // 1 B
#define PRINTER_VERSION_ADDR 0x08020030 // 1 B
#define IP4_ADDR_STR_SIZE   16

struct netif eth0;
ETH_config_t ethconfig;
ETH_config_t tmp_ethconfig;

static char s_addr_ip4_str[IP4_ADDR_STR_SIZE] = { 0 };
static char s_msk_ip4_str[IP4_ADDR_STR_SIZE] = { 0 };
static char s_gw_ip4_str[IP4_ADDR_STR_SIZE] = { 0 };
static char s_mac_addr_str[MAC_ADDR_STR_LEN] = { 0 };
static char s_connect_ip4[IP4_ADDR_STR_SIZE] = { 0 };

char * mac_addr_str(void) { return s_mac_addr_str; }
const char * addr_ip4_str(void) { return s_addr_ip4_str; }
const char * msk_ip4_str(void) { return s_msk_ip4_str; }
const char * gw_ip4_str(void) { return s_gw_ip4_str; }
const char * connect_ip4_str(void) { return s_connect_ip4; }
static int ini_handler_func(void *user, const char *section, const char *name, const char *value) {

    ETH_config_t * tmp_config = (ETH_config_t *)user;

#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0

    if (MATCH("lan_ip4", "type")) {
        if (strncmp(value, "DHCP", 4) == 0 || strncmp(value, "dhcp", 4) == 0) {
            CHANGE_LAN_TO_DHCP(tmp_config->lan.flag);
            tmp_config->var_mask |= ETHVAR_MSK(ETHVAR_LAN_FLAGS);
        } else if (strncmp(value, "STATIC", 6) == 0 || strncmp(value, "static", 6) == 0) {
            CHANGE_LAN_TO_STATIC(tmp_config->lan.flag);
            tmp_config->var_mask |= ETHVAR_MSK(ETHVAR_LAN_FLAGS);
        }
    } else if (MATCH("lan_ip4", "hostname")) {
        strlcpy(tmp_config->hostname, value, ETH_HOSTNAME_LEN + 1);
        tmp_config->hostname[ETH_HOSTNAME_LEN] = '\0';
        tmp_config->var_mask |= ETHVAR_MSK(ETHVAR_HOSTNAME);
    } else if (MATCH("lan_ip4", "address")) {
        if (ip4addr_aton(value, &tmp_config->lan.addr_ip4)) {
            tmp_config->var_mask |= ETHVAR_MSK(ETHVAR_LAN_ADDR_IP4);
        }
    } else if (MATCH("lan_ip4", "mask")) {
        if (ip4addr_aton(value, &tmp_config->lan.msk_ip4)) {
            tmp_config->var_mask |= ETHVAR_MSK(ETHVAR_LAN_MSK_IP4);
        }
    } else if (MATCH("lan_ip4", "gateway")) {
        if (ip4addr_aton(value, &tmp_config->lan.gw_ip4)) {
            tmp_config->var_mask |= ETHVAR_MSK(ETHVAR_LAN_GW_IP4);
        }
    } else if (MATCH("connect", "address")) {
        if (ip4addr_aton(value, &tmp_config->connect.ip4)) {
            tmp_config->var_mask |= ETHVAR_MSK(ETHVAR_CONNECT_IP4);
        }
    } else if (MATCH("connect", "token")) {
        strlcpy(tmp_config->connect.token, value, CONNECT_TOKEN_LEN + 1);
        tmp_config->var_mask |= ETHVAR_MSK(ETHVAR_CONNECT_TOKEN);
    } else {
        return 0; /* unknown section/name, error */
    }
    return 1;
}

static ini_handler wui_ini_handler = ini_handler_func;

uint32_t load_ini_params(void) {
    tmp_ethconfig.var_mask = 0;
    return ini_load_file(wui_ini_handler, &tmp_ethconfig);
}

uint32_t save_eth_params(uint32_t mask) {

    if (mask & ETHVAR_MSK(ETHVAR_LAN_FLAGS)) {
        eeprom_set_var(EEVAR_LAN_FLAG, variant8_ui16(ethconfig.lan.flag));
    }
    if (mask & ETHVAR_MSK(ETHVAR_LAN_ADDR_IP4)) {
        eeprom_set_var(EEVAR_LAN_IP4_ADDR, variant8_ui32(ethconfig.lan.addr_ip4.addr));
    }
    if (mask & ETHVAR_MSK(ETHVAR_LAN_MSK_IP4)){
        eeprom_set_var(EEVAR_LAN_IP4_MSK, variant8_ui32(ethconfig.lan.msk_ip4.addr));
    }
    if (mask & ETHVAR_MSK(ETHVAR_LAN_GW_IP4)){
        eeprom_set_var(EEVAR_LAN_IP4_GW, variant8_ui32(ethconfig.lan.gw_ip4.addr));
    }
    if (mask & ETHVAR_MSK(ETHVAR_HOSTNAME)) {
        variant8_t hostname = variant8_pchar(ethconfig.hostname, 0, 0);
        eeprom_set_var(EEVAR_LAN_HOSTNAME, hostname);
        //variant8_done() is not called, variant_pchar with init flag 0 doesnt hold its memory
    }
    if (mask & ETHVAR_MSK(ETHVAR_CONNECT_TOKEN)) {
        variant8_t token = variant8_pchar(ethconfig.connect.token, 0, 0);
        eeprom_set_var(EEVAR_CONNECT_TOKEN, token);
        //variant8_done() is not called, variant_pchar with init flag 0 doesnt hold its memory
    }
    if (mask & ETHVAR_MSK(ETHVAR_CONNECT_IP4)) {
        eeprom_set_var(EEVAR_CONNECT_IP4, variant8_ui32(ethconfig.connect.ip4.addr));
    }

    return 0;
}

uint32_t load_eth_params(uint32_t mask) {

    if (mask & ETHVAR_MSK(ETHVAR_LAN_FLAGS)){
        ethconfig.lan.flag = eeprom_get_var(EEVAR_LAN_FLAG).ui8;
    }
    if (mask & ETHVAR_MSK(ETHVAR_LAN_ADDR_IP4)) {
        ethconfig.lan.addr_ip4.addr = eeprom_get_var(EEVAR_LAN_IP4_ADDR).ui32;
    }
    if (mask & ETHVAR_MSK(ETHVAR_LAN_MSK_IP4)) {
        ethconfig.lan.msk_ip4.addr = eeprom_get_var(EEVAR_LAN_IP4_MSK).ui32;
    }
    if (mask & ETHVAR_MSK(ETHVAR_LAN_GW_IP4)) {
        ethconfig.lan.gw_ip4.addr = eeprom_get_var(EEVAR_LAN_IP4_GW).ui32;
    }
    if (mask & ETHVAR_MSK(ETHVAR_HOSTNAME)) {
        variant8_t hostname = eeprom_get_var(EEVAR_LAN_HOSTNAME);
        strlcpy(ethconfig.hostname, hostname.pch, ETH_HOSTNAME_LEN + 1);
        variant8_done(&hostname);
    }
    if (mask & ETHVAR_MSK(ETHVAR_CONNECT_TOKEN)) {
        variant8_t token = eeprom_get_var(EEVAR_CONNECT_TOKEN);
        strlcpy(ethconfig.connect.token, token.pch, CONNECT_TOKEN_LEN + 1);
        variant8_done(&token);
    }
    if (mask & ETHVAR_MSK(ETHVAR_CONNECT_IP4)) {
        ethconfig.connect.ip4.addr = eeprom_get_var(EEVAR_CONNECT_IP4).ui32;
    }

    return 0;
}

void get_printer_info(printer_info_t *printer_info) {
    // FIRMWARE VERSION
    strlcpy(printer_info->firmware_version, project_version_full, FW_VER_STR_LEN);
    // PRINTER TYPE
    printer_info->printer_type = *(volatile uint8_t *)PRINTER_TYPE_ADDR;
    // PRINTER_VERSION
    printer_info->printer_version = *(volatile uint8_t *)PRINTER_VERSION_ADDR;
    // MAC ADDRESS
    parse_MAC_address(printer_info->mac_address);
    // SERIAL NUMBER
    for (int i = 0; i < OTP_SERIAL_NUMBER_SIZE; i++) {
        printer_info->serial_number[i] = *(volatile char *)(OTP_SERIAL_NUMBER_ADDR + i);
    }
    // UUID - 96 bits
    volatile uint32_t *uuid_ptr = (volatile uint32_t *)OTP_STM32_UUID_ADDR;
    snprintf(printer_info->mcu_uuid, UUID_STR_LEN, "%08lx-%08lx-%08lx", *uuid_ptr, *(uuid_ptr + 1), *(uuid_ptr + 2));
}

void parse_MAC_address(char * dest){
    volatile uint8_t *mac_ptr = (volatile uint8_t *)OTP_MAC_ADDRESS_ADDR;
    snprintf(dest, MAC_ADDR_STR_LEN, "%02x:%02x:%02x:%02x:%02x:%02x",
        *mac_ptr, *(mac_ptr + 1), *(mac_ptr + 2), *(mac_ptr + 3), *(mac_ptr + 4), *(mac_ptr + 5));
}

void stringify_eth_for_ini(char * dest) {
    snprintf(dest, MAX_INI_SIZE,
        "[lan_ip4]\ntype=%s\nhostname=%s\naddress=%s\nmask=%s\ngateway=%s\n\n[connect]\naddress=%s\ntoken=%s\n",
        IS_LAN_STATIC(ethconfig.lan.flag) ? "STATIC" : "DHCP", ethconfig.hostname,
        addr_ip4_str(), msk_ip4_str(), gw_ip4_str(), connect_ip4_str(), ethconfig.connect.token);
}

void stringify_eth_for_screen(char * dest) {
    snprintf(dest, 150, "IPv4 Address:\n  %s      \nIPv4 Netmask:\n  %s      \nIPv4 Gateway:\n  %s      \nMAC Address:\n  %s",
        addr_ip4_str(), msk_ip4_str(), gw_ip4_str(), mac_addr_str());
}

void update_eth_addrs(void){
    if (IS_LAN_ON(ethconfig.lan.flag)) {
        if (IS_LAN_DHCP(ethconfig.lan.flag) && dhcp_supplied_address(&eth0)) {
            ethconfig.lan.addr_ip4.addr = netif_ip4_addr(&eth0)->addr;
            ethconfig.lan.msk_ip4.addr = netif_ip4_netmask(&eth0)->addr;
            ethconfig.lan.gw_ip4.addr = netif_ip4_gw(&eth0)->addr;
            return;
        }
    }
    if (IS_LAN_DHCP(ethconfig.lan.flag)) {
        ethconfig.lan.addr_ip4.addr = 0;
        ethconfig.lan.msk_ip4.addr = 0;
        ethconfig.lan.gw_ip4.addr = 0;
        return;
    }
    load_eth_params(ETHVAR_STATIC_LAN_ADDRS);
    strlcpy(s_addr_ip4_str, ip4addr_ntoa(&(ethconfig.lan.addr_ip4)), IP4_ADDR_STR_SIZE);
    strlcpy(s_msk_ip4_str, ip4addr_ntoa(&(ethconfig.lan.msk_ip4)), IP4_ADDR_STR_SIZE);
    strlcpy(s_gw_ip4_str, ip4addr_ntoa(&(ethconfig.lan.gw_ip4)), IP4_ADDR_STR_SIZE);
}

ETH_STATUS_t eth_status(void){
    ETH_STATUS_t ret;
    if (netif_is_link_up(&eth0)) {
        if (IS_LAN_STATIC(ethconfig.lan.flag)) {
            if (netif_is_up(&eth0)) {
                ret = ETH_NETIF_UP;
            } else {
                ret = ETH_NETIF_DOWN;
            }
        } else {
            if (dhcp_supplied_address(&eth0)) {
                ret = ETH_NETIF_UP;
            } else {
                ret = ETH_NETIF_DOWN;
            }
        }
    } else {
        ret = ETH_UNLINKED;
    }
    return ret;
}

void lan_turn_off(void){
    if (netif_is_up(&eth0)) {
        netifapi_netif_set_down(&eth0);
    }
    TURN_LAN_OFF(ethconfig.lan.flag);
    save_eth_params(ETHVAR_MSK(ETHVAR_LAN_FLAGS));
}

void lan_turn_on(void){
    TURN_LAN_ON(ethconfig.lan.flag);
    save_eth_params(ETHVAR_MSK(ETHVAR_LAN_FLAGS));
    if (netif_is_link_up(&eth0)) {
        netifapi_netif_set_up(&eth0);
    }
}

void lan_set_static(void) {
    if (netif_is_up(&eth0)) {
        netifapi_netif_set_down(&eth0);
    }
    CHANGE_LAN_TO_STATIC(ethconfig.lan.flag);
    save_eth_params(ETHVAR_MSK(ETHVAR_LAN_FLAGS));
    netifapi_netif_set_addr(&eth0,
        (const ip4_addr_t *)&(ethconfig.lan.addr_ip4),
        (const ip4_addr_t *)&(ethconfig.lan.msk_ip4),
        (const ip4_addr_t *)&(ethconfig.lan.gw_ip4)
        );
    if (netif_is_link_up(&eth0) && IS_LAN_ON(ethconfig.lan.flag)) {
        netifapi_netif_set_up(&eth0);
    }
}

void lan_set_dhcp(void) {
    if (netif_is_up(&eth0)) {        
        netifapi_netif_set_down(&eth0);
    }
    CHANGE_LAN_TO_DHCP(ethconfig.lan.flag);
    save_eth_params(ETHVAR_MSK(ETHVAR_LAN_FLAGS));
    if (netif_is_link_up(&eth0) && IS_LAN_ON(ethconfig.lan.flag)) {
        netifapi_netif_set_up(&eth0);
    }
}

uint32_t set_loaded_eth_params(void){
    ethconfig.var_mask = 0;
    uint8_t previous_lan_flag = ethconfig.lan.flag;
    if (tmp_ethconfig.var_mask & ETHVAR_MSK(ETHVAR_LAN_FLAGS)) {
        // if lan type is set to STATIC
        if (IS_LAN_STATIC(tmp_ethconfig.lan.flag)){
            if ((tmp_ethconfig.var_mask & ETHVAR_STATIC_LAN_ADDRS) == ETHVAR_STATIC_LAN_ADDRS) {
                CHANGE_LAN_TO_STATIC(ethconfig.lan.flag);
                ethconfig.lan.addr_ip4.addr = tmp_ethconfig.lan.addr_ip4.addr;
                ethconfig.lan.msk_ip4.addr = tmp_ethconfig.lan.msk_ip4.addr;
                ethconfig.lan.gw_ip4.addr = tmp_ethconfig.lan.gw_ip4.addr;
                ethconfig.var_mask |= (ETHVAR_STATIC_LAN_ADDRS | ETHVAR_MSK(ETHVAR_LAN_FLAGS));
            } else {
                return 1;
            }
        } else {
            CHANGE_LAN_TO_DHCP(ethconfig.lan.flag);
            ethconfig.var_mask |= ETHVAR_MSK(EEVAR_LAN_FLAG);
        }
    }
    if (tmp_ethconfig.var_mask & ETHVAR_MSK(ETHVAR_HOSTNAME)) {
        strlcpy(ethconfig.hostname, tmp_ethconfig.hostname, ETH_HOSTNAME_LEN + 1);
        eth0.hostname = ethconfig.hostname;
        ethconfig.var_mask |= ETHVAR_MSK(ETHVAR_HOSTNAME);
    }
    if (tmp_ethconfig.var_mask & ETHVAR_MSK(ETHVAR_CONNECT_TOKEN)) {
        strlcpy(ethconfig.connect.token, tmp_ethconfig.connect.token, CONNECT_TOKEN_LEN + 1);
        ethconfig.var_mask |= ETHVAR_MSK(ETHVAR_CONNECT_TOKEN);
    }
    if (tmp_ethconfig.var_mask & ETHVAR_MSK(ETHVAR_CONNECT_IP4)) {
        ethconfig.connect.ip4.addr = tmp_ethconfig.connect.ip4.addr;
        ethconfig.var_mask |= ETHVAR_MSK(ETHVAR_CONNECT_IP4);
    }

    save_eth_params(ethconfig.var_mask);

    // if type=STATIC/DHCP is in INI file
    if (tmp_ethconfig.var_mask & ETHVAR_MSK(ETHVAR_LAN_FLAGS)) {
        // if there was a change from STATIC to DHCP
        if (IS_LAN_STATIC(previous_lan_flag) && IS_LAN_DHCP(ethconfig.lan.flag)) {
            lan_set_dhcp();
        // or STATIC to STATIC
        } else if (IS_LAN_STATIC(ethconfig.lan.flag)){
            lan_set_static();
        }   
        // from DHCP to DHCP: do nothing
    }

    return 0;
}