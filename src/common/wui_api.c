/*
 * wui_api.h
 * \brief   interface functions for Web User Interface library
 *
 *  Created on: April 22, 2020
 *      Author: joshy <joshymjose[at]gmail.com>
 */
#include "wui_api.h"

#include "ini_handler.h"
#include "eeprom.h"
#include "string.h"

#define LAN_MSK_TYPE      2 //EEPROM mask for user-defined settings (Switch between dhcp and static)
#define IP4_TYPE_FLAG_POS 1

#define IS_LAN_STATIC(flg) (flg & LAN_MSK_TYPE)
#define IS_LAN_DHCP(flg)   (!IS_LAN_STATIC(flg))

static int ini_handler_func(void *user, const char *section, const char *name, const char *value) {

    ETH_Config_t *tmp_config = (ETH_Config_t *)user;

#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0

    if (MATCH("lan_ip4", "type")) {
        if (strncmp(value, "DHCP", 4) == 0 || strncmp(value, "dhcp", 4) == 0) {
            tmp_config->lan_flag &= ~LAN_EEFLG_TYPE;
            tmp_config->set_flag |= NETVAR_MSK(NETVAR_LAN_FLAGS);
        } else if (strncmp(value, "STATIC", 6) == 0 || strncmp(value, "static", 6) == 0) {
            tmp_config->lan_flag |= LAN_EEFLG_TYPE;
            tmp_config->set_flag |= NETVAR_MSK(NETVAR_LAN_FLAGS);
        }
    } else if (MATCH("lan_ip4", "hostname")) {
        strlcpy(tmp_config->hostname, value, LAN_HOSTNAME_MAX_LEN + 1);
        tmp_config->hostname[LAN_HOSTNAME_MAX_LEN] = '\0';
        tmp_config->set_flag |= NETVAR_MSK(NETVAR_HOSTNAME);
    } else if (MATCH("lan_ip4", "address")) {
        if (ip4addr_aton(value, &tmp_config->lan_ip4_addr)) {
            tmp_config->set_flag |= NETVAR_MSK(NETVAR_LAN_IP4_ADDR);
        }
    } else if (MATCH("lan_ip4", "mask")) {
        if (ip4addr_aton(value, &tmp_config->lan_ip4_msk)) {
            tmp_config->set_flag |= NETVAR_MSK(NETVAR_LAN_IP4_MSK);
        }
    } else if (MATCH("lan_ip4", "gateway")) {
        if (ip4addr_aton(value, &tmp_config->lan_ip4_gw)) {
            tmp_config->set_flag |= NETVAR_MSK(NETVAR_LAN_IP4_GW);
        }
    } else if (MATCH("connect", "address")) {
        if (ip4addr_aton(value, &tmp_config->connect_ip4)) {
            tmp_config->set_flag |= NETVAR_MSK(NETVAR_CONNECT_IP4);
        }
    } else if (MATCH("connect", "token")) {
        strlcpy(tmp_config->connect_token, value, CONNECT_TOKEN_SIZE + 1);
        tmp_config->connect_token[CONNECT_TOKEN_SIZE] = '\0';
        tmp_config->set_flag |= NETVAR_MSK(NETVAR_CONNECT_TOKEN);
    } else {
        return 0; /* unknown section/name, error */
    }
    return 1;
}

static ini_handler wui_init_handler = ini_handler_func;

void load_ini_params(ETH_Config_t *config_t) {
    ini_load_file(wui_init_handler, (void *)config_t);
}

uint32_t eeprom_set_eth_params(ETH_Config_t *config_t) {

    // type=STATIC/DHCP is in INI file
    if (config_t->set_flag & NETVAR_MSK(NETVAR_LAN_FLAGS)) {
        // if lan type is set to STATIC
        if (IS_LAN_STATIC(config_t->lan_flag) && (config_t->set_flag & NETVAR_STATIC_LAN_ADDRS)) {
            uint8_t lan_flags = eeprom_get_var(EEVAR_LAN_FLAG).ui8;
            lan_flags |= (1 << IP4_TYPE_FLAG_POS);
            eeprom_set_var(EEVAR_LAN_FLAG, variant8_ui8(lan_flags));
            eeprom_set_var(EEVAR_LAN_IP4_ADDR,
                variant8_ui32(config_t->lan_ip4_addr.addr));
            eeprom_set_var(EEVAR_LAN_IP4_MSK,
                variant8_ui32(config_t->lan_ip4_msk.addr));
            eeprom_set_var(EEVAR_LAN_IP4_GW,
                variant8_ui32(config_t->lan_ip4_gw.addr));
        } else {
            uint8_t lan_flags = eeprom_get_var(EEVAR_LAN_FLAG).ui8;
            lan_flags = 0xff;
            lan_flags &= ~(1 << IP4_TYPE_FLAG_POS);
            eeprom_set_var(EEVAR_LAN_FLAG, variant8_ui8(lan_flags));
        }
    }

    if (config_t->set_flag & NETVAR_MSK(NETVAR_HOSTNAME)) {
        variant8_t hostname = variant8_pchar(config_t->hostname, 0, 0);
        eeprom_set_var(EEVAR_LAN_HOSTNAME, hostname);
        //variant8_done() is not called, variant_pchar with init flag 0 doesnt hold its memory
    }

    if (config_t->set_flag & NETVAR_MSK(NETVAR_CONNECT_TOKEN)) {
        variant8_t token = variant8_pchar(config_t->connect_token, 0, 0);
        eeprom_set_var(EEVAR_CONNECT_TOKEN, token);
        //variant8_done() is not called, variant_pchar with init flag 0 doesnt hold its memory
    }
    if (config_t->set_flag & NETVAR_MSK(NETVAR_CONNECT_IP4)) {

        eeprom_set_var(EEVAR_CONNECT_IP4, variant8_ui32(config_t->connect_ip4.addr));
    }

    return 0;
}
