// INI file handler (ini_handler.c)

#include "ini_handler.h"
#include "ini.h"
#include "ff.h"
#include "netif_settings.h"
#include <stdlib.h>

#define MAX_UINT16  65535
static const char network_ini_file_name[] = "/lan_settings.ini"; //change -> change msgboxes in screen_lan_settings

static int load_netconfig_handler(void *user, const char *section, const char *name, const char *value) {
    networkconfig_t *tmp_config = (networkconfig_t *)user;
#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    if (MATCH("lan_ip4", "type")) {
        if (strncmp(value, "DHCP", 4) == 0 || strncmp(value, "dhcp", 4) == 0) {
            CHANGE_LAN_TO_DHCP(tmp_config->lan.flg);
            tmp_config->set_flg |= NETVAR_MSK(NETVAR_LAN_FLAGS);
        } else if (strncmp(value, "STATIC", 6) == 0 || strncmp(value, "static", 6) == 0) {
            CHANGE_LAN_TO_STATIC(tmp_config->lan.flg);
            tmp_config->set_flg |= NETVAR_MSK(NETVAR_LAN_FLAGS);
        }
    } else if (MATCH("lan_ip4", "hostname")) {
        strlcpy(tmp_config->hostname, value, LAN_HOSTNAME_MAX_LEN + 1);
        tmp_config->hostname[LAN_HOSTNAME_MAX_LEN] = '\0';
        tmp_config->set_flg |= NETVAR_MSK(NETVAR_HOSTNAME);
    } else if (MATCH("lan_ip4", "address")) {
        if (ip4addr_aton(value, &tmp_config->lan.addr_ip4)) {
            tmp_config->set_flg |= NETVAR_MSK(NETVAR_LAN_IP4_ADDR);
        }
    } else if (MATCH("lan_ip4", "mask")) {
        if (ip4addr_aton(value, &tmp_config->lan.msk_ip4)) {
            tmp_config->set_flg |= NETVAR_MSK(NETVAR_LAN_IP4_MSK);
        }
    } else if (MATCH("lan_ip4", "gateway")) {
        if (ip4addr_aton(value, &tmp_config->lan.gw_ip4)) {
            tmp_config->set_flg |= NETVAR_MSK(NETVAR_LAN_IP4_GW);
        }
    } else if (MATCH("connect", "port")) {
        int32_t tmp_port = atoi(value);
        if ( tmp_port > 0 && tmp_port < MAX_UINT16){
            tmp_config->connect.port = tmp_port;
            tmp_config->set_flg |= NETVAR_MSK(NETVAR_CONNECT_PORT);
        }
    }
#ifdef BUDDY_ENABLE_DNS
    else if (MATCH("lan_ip4", "dns1")) {
        if (ip4addr_aton(value, &tmp_config->dns1_ip4)) {
            tmp_config->set_flg |= NETVAR_MSK(NETVAR_DNS1_IP4);
        }
    } else if (MATCH("lan_ip4", "dns2")) {
        if (ip4addr_aton(value, &tmp_config->dns2_ip4)) {
            tmp_config->set_flg |= NETVAR_MSK(NETVAR_DNS2_IP4);
        }
    }
#endif //BUDDY_ENABLE_DNS
#ifdef BUDDY_ENABLE_CONNECT
    else if (MATCH("connect", "address")) {
        if (ip4addr_aton(value, &tmp_config->connect.ip4)) {
            tmp_config->set_flg |= NETVAR_MSK(NETVAR_CONNECT_IP4);
        }
    } else if (MATCH("connect", "token")) {
        strlcpy(tmp_config->connect.token, value, CONNECT_TOKEN_SIZE + 1);
        tmp_config->connect.token[CONNECT_TOKEN_SIZE] = '\0';
        tmp_config->set_flg |= NETVAR_MSK(NETVAR_CONNECT_TOKEN);
    }
#endif // BUDDY_ENABLE_CONNECT
    else {
        return 0; /* unknown section/name, error */
    }
    return 1;
}

uint8_t ini_save_file(const char *ini_save_str) {

    UINT ini_config_len = strlen(ini_save_str);
    UINT written_bytes = 0;
    FIL ini_file;

    f_unlink(network_ini_file_name);

    uint8_t i = f_open(&ini_file, network_ini_file_name, FA_WRITE | FA_CREATE_NEW);
    uint8_t w = f_write(&ini_file, ini_save_str, ini_config_len, &written_bytes);
    uint8_t c = f_close(&ini_file);

    if (i || w || c || written_bytes != ini_config_len)
        return 0;

    return 1;
}

uint8_t ini_load_file(void *user_struct) {
    UINT written_bytes = 0;
    FIL ini_file;

    uint8_t file_init = f_open(&ini_file, network_ini_file_name, FA_READ);
    uint8_t file_read = f_read(&ini_file, ini_file_str, MAX_INI_SIZE, &written_bytes);
    uint8_t file_close = f_close(&ini_file);

    if (file_init || file_read || file_close) {
        return 0;
    }

    if (ini_parse_string(ini_file_str, load_netconfig_handler, user_struct) < 0) {
        return 0;
    }
    return 1;
}
