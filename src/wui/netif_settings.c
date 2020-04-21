#include "netif_settings.h"
#include "wui_eeprom_api.h"
#include "lwip/netifapi.h"
#include <string.h>
#include "dns.h"

#define MAX_UINT16          65535
#define MAC_ADDR_START      0x1FFF781A //MM:MM:MM:SS:SS:SS
#define MAC_ADDR_SIZE       6
#define IP4_ADDR_STR_SIZE   16
#define MAC_ADDR_STR_SIZE   18

struct netif eth0;

networkconfig_t netconfig = { 0 };
networkconfig_t tmp_netconfig = { 0 };
static char s_addr_ip4_str[IP4_ADDR_STR_SIZE] = { 0 };
static char s_msk_ip4_str[IP4_ADDR_STR_SIZE] = { 0 };
static char s_gw_ip4_str[IP4_ADDR_STR_SIZE] = { 0 };
static char s_mac_addr_str[MAC_ADDR_STR_SIZE] = { 0 };
static char s_connect_ip4[IP4_ADDR_STR_SIZE] = { 0 };

const char * get_mac_addr_str(void) { return s_mac_addr_str; }
const char * get_addr_ip4_str(void) { return s_addr_ip4_str; }
const char * get_msk_ip4_str(void) { return s_msk_ip4_str; }
const char * get_gw_ip4_str(void) { return s_gw_ip4_str; }
const char * get_connect_ip4_str(void) { return s_connect_ip4; }
void parse_MAC_addr(void) {
    volatile uint8_t mac_addr[] = { 0, 0, 0, 0, 0, 0 };
    for (uint8_t i = 0; i < MAC_ADDR_SIZE; i++)
        mac_addr[i] = *(volatile uint8_t *)(MAC_ADDR_START + i);

    sprintf(s_mac_addr_str, "%02x:%02x:%02x:%02x:%02x:%02x", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
}
void lan_set_static(void) {
    if (netif_is_up(&eth0)) {
        netifapi_netif_set_down(&eth0);
    }
    CHANGE_LAN_TO_STATIC(netconfig.lan.flg);
    wui_eeprom_set_var(NETVAR_LAN_FLAGS, variant8_ui8(netconfig.lan.flg));
    netifapi_netif_set_addr(&eth0,
        (const ip4_addr_t *)&(netconfig.lan.addr_ip4),
        (const ip4_addr_t *)&(netconfig.lan.msk_ip4),
        (const ip4_addr_t *)&(netconfig.lan.gw_ip4)
        );
#if BUDDY_ENABLE_DNS
    dns_setserver(0, &netconfig.dns1_ip4);
    dns_setserver(1, &netconfig.dns2_ip4);
#endif //BUDDY_ENABLE_DNS
    if (netif_is_link_up(&eth0) && IS_LAN_ON(netconfig.lan.flg)) {
        netifapi_netif_set_up(&eth0);
    }
}

void lan_set_dhcp(void) {
    if (netif_is_up(&eth0)) {        
        netifapi_netif_set_down(&eth0);
    }
    CHANGE_LAN_TO_DHCP(netconfig.lan.flg);
    wui_eeprom_set_var(NETVAR_LAN_FLAGS, variant8_ui8(netconfig.lan.flg));
    if (netif_is_link_up(&eth0) && IS_LAN_ON(netconfig.lan.flg)) {
        netifapi_netif_set_up(&eth0);
    }
}

/*  User can set any of these values independentely (none of them have to be in ini file), but:
*       If type=STATIC      ->      All 3 lan addrs (and at least one DNS addr) have to be there and be valid ip addrs
*       If type=DHCP        ->      All 3 lan addrs are skipped
*       If Connect IP is there, it has to be valid ip addr
*       If Connect token is there, it will set first 20chars (max len)
*       If lan hostname is there, it will set first 20chars (max len)
*/
uint8_t set_loaded_netconfig(void){

    // type=STATIC/DHCP is in INI file
    if (tmp_netconfig.set_flg & NETVAR_MSK(NETVAR_LAN_FLAGS)) {
        // if lan type is set to STATIC
        if (IS_LAN_STATIC(tmp_netconfig.lan.flg)){
            if ((tmp_netconfig.set_flg & NETVAR_STATIC_LAN_ADDRS) != NETVAR_STATIC_LAN_ADDRS
#if BUDDY_ENABLE_DNS
                && tmp_netconfig.set_flg & (NETVAR_MSK(NETVAR_DNS1_IP4) | NETVAR_MSK(NETVAR_DNS2_IP4))
                && (tmp_netconfig.dns1_ip4.addr != 0 || tmp_netconfig.dns2_ip4.addr != 0)
#endif //BUDDY_ENABLE_DNS
            ) {
                return 0;
            }
            wui_eeprom_set_var(NETVAR_LAN_IP4_ADDR, variant8_ui32(tmp_netconfig.lan.addr_ip4.addr));
            wui_eeprom_set_var(NETVAR_LAN_IP4_MSK, variant8_ui32(tmp_netconfig.lan.msk_ip4.addr));
            wui_eeprom_set_var(NETVAR_LAN_IP4_GW, variant8_ui32(tmp_netconfig.lan.gw_ip4.addr));
#if BUDDY_ENABLE_DNS
            wui_eeprom_set_var(NETVAR_DNS1_IP4, variant8_ui32(tmp_netconfig.dns1_ip4.addr));
            wui_eeprom_set_var(NETVAR_DNS2_IP4, variant8_ui32(tmp_netconfig.dns2_ip4.addr));
            netconfig.lan.addr_ip4.addr = tmp_netconfig.lan.addr_ip4.addr;
            netconfig.lan.msk_ip4.addr = tmp_netconfig.lan.msk_ip4.addr;
#endif //BUDDY_ENABLE_DNS
            netconfig.lan.gw_ip4.addr = tmp_netconfig.lan.gw_ip4.addr;
            netconfig.dns1_ip4.addr = tmp_netconfig.dns1_ip4.addr;
            netconfig.dns2_ip4.addr = tmp_netconfig.dns2_ip4.addr;
        }
    }
    if (tmp_netconfig.set_flg & NETVAR_MSK(NETVAR_HOSTNAME)) {
        strlcpy(netconfig.hostname, tmp_netconfig.hostname, LAN_HOSTNAME_LEN + 1);
        eth0.hostname = netconfig.hostname;
        variant8_t hostname = variant8_pchar(tmp_netconfig.hostname, 0, 0);
        wui_eeprom_set_var(NETVAR_HOSTNAME, hostname);
        //variant8_done() is not called, variant_pchar with init flag 0 doesnt hold its memory
    }
#ifdef BUDDY_ENABLE_CONNECT
    if (tmp_netconfig.set_flg & NETVAR_MSK(NETVAR_CONNECT_TOKEN)) {
        strlcpy(netconfig.connect.token, tmp_netconfig.connect.token, CONNECT_TOKEN_SIZE + 1);
        variant8_t token = variant8_pchar(tmp_netconfig.connect.token, 0, 0);
        wui_eeprom_set_var(NETVAR_CONNECT_TOKEN, token);
        //variant8_done() is not called, variant_pchar with init flag 0 doesnt hold its memory
    }
    if (tmp_netconfig.set_flg & NETVAR_MSK(NETVAR_CONNECT_IP4)) {
        netconfig.connect.ip4.addr = tmp_netconfig.connect.ip4.addr;
        wui_eeprom_set_var(NETVAR_CONNECT_IP4, variant8_ui32(tmp_netconfig.connect.ip4.addr));
    }
    if (tmp_netconfig.set_flg & NETVAR_MSK(NETVAR_CONNECT_PORT)) {
        netconfig.connect.port = tmp_netconfig.connect.port;
        wui_eeprom_set_var(NETVAR_CONNECT_PORT, variant8_ui16(tmp_netconfig.connect.port));
    }
#endif // BUDDY_ENABLE_CONNECT

    // if type=STATIC/DHCP is in INI file   
    if (tmp_netconfig.set_flg & NETVAR_MSK(NETVAR_LAN_FLAGS)) {
        // if there was a change from STATIC to DHCP
        if (IS_LAN_DHCP(tmp_netconfig.lan.flg) && IS_LAN_STATIC(wui_eeprom_get_var(NETVAR_LAN_FLAGS).ui8)) {
            lan_set_dhcp();
        // or STATIC to STATIC
        } else if (IS_LAN_STATIC(tmp_netconfig.lan.flg)){
            lan_set_static();
        }   
        // from DHCP to DHCP: do nothing
    }
    return 1;
}

static void update_addrs(uint8_t lan_flg){
    if (IS_LAN_ON(lan_flg)) {
        if (IS_LAN_DHCP(lan_flg) && dhcp_supplied_address(&eth0)) {
            netconfig.lan.addr_ip4.addr = netif_ip4_addr(&eth0)->addr;
            netconfig.lan.msk_ip4.addr = netif_ip4_netmask(&eth0)->addr;
            netconfig.lan.gw_ip4.addr = netif_ip4_gw(&eth0)->addr;
            return;
        }
    }
    if (IS_LAN_DHCP(lan_flg)) {
        netconfig.lan.addr_ip4.addr = 0;
        netconfig.lan.msk_ip4.addr = 0;
        netconfig.lan.gw_ip4.addr = 0;
        return;
    }
    netconfig.lan.addr_ip4.addr = wui_eeprom_get_var(NETVAR_LAN_IP4_ADDR).ui32;
    netconfig.lan.msk_ip4.addr = wui_eeprom_get_var(NETVAR_LAN_IP4_MSK).ui32;
    netconfig.lan.gw_ip4.addr = wui_eeprom_get_var(NETVAR_LAN_IP4_GW).ui32;
}

void update_netconfig(uint32_t msk){

    if(msk & NETVAR_MSK(NETVAR_LAN_FLAGS)){
        netconfig.lan.flg = wui_eeprom_get_var(NETVAR_LAN_FLAGS).ui8;
    }
    if((msk & NETVAR_STATIC_LAN_ADDRS) == NETVAR_STATIC_LAN_ADDRS){
        uint8_t lan_flg = wui_eeprom_get_var(NETVAR_LAN_FLAGS).ui8;
        update_addrs(lan_flg);
        strlcpy(s_addr_ip4_str, ip4addr_ntoa(&(netconfig.lan.addr_ip4)), IP4_ADDR_STR_SIZE);
        strlcpy(s_msk_ip4_str, ip4addr_ntoa(&(netconfig.lan.msk_ip4)), IP4_ADDR_STR_SIZE);
        strlcpy(s_gw_ip4_str, ip4addr_ntoa(&(netconfig.lan.gw_ip4)), IP4_ADDR_STR_SIZE);
    }
    if(msk & NETVAR_MSK(NETVAR_HOSTNAME)){
        variant8_t hostname = wui_eeprom_get_var(NETVAR_HOSTNAME);
        strlcpy(netconfig.hostname, hostname.pch, LAN_HOSTNAME_LEN + 1);
        variant8_done(&hostname);
    }
#if BUDDY_ENABLE_DNS
    if(msk & NETVAR_MSK(dns1_ip4)) {
        netconfig.dns1_ip4.addr = wui_eeprom_get_var(NETVAR_DNS1_IP4).ui32;
    }
    if(msk & NETVAR_MSK(dns2_ip4)) {
        netconfig.dns2_ip4.addr = wui_eeprom_get_var(NETVAR_DNS2_IP4).ui32;
    }
#endif //BUDDY_ENABLE_DNS
#ifdef BUDDY_ENABLE_CONNECT
    if(msk & NETVAR_MSK(NETVAR_CONNECT_IP4)){
        netconfig.connect.ip4.addr = wui_eeprom_get_var(NETVAR_CONNECT_IP4).ui32;
        strlcpy(s_connect_ip4, ip4addr_ntoa(&(netconfig.connect.ip4)), IP4_ADDR_STR_SIZE);
    }
    if(msk & NETVAR_MSK(NETVAR_CONNECT_TOKEN)){
        variant8_t connect_token = wui_eeprom_get_var(NETVAR_CONNECT_TOKEN);
        strlcpy(netconfig.connect.token, connect_token.pch, CONNECT_TOKEN_SIZE + 1);
        variant8_done(&connect_token);
    }
    if(msk & NETVAR_MSK(NETVAR_CONNECT_PORT)) {
        netconfig.connect.port = wui_eeprom_get_var(NETVAR_CONNECT_PORT).ui16;
    }
#endif // BUDDY_ENABLE_CONNECT
}

void lan_turn_off(void){
    if (netif_is_up(&eth0)) {
        netifapi_netif_set_down(&eth0);
    }
    TURN_LAN_OFF(netconfig.lan.flg);
    wui_eeprom_set_var(NETVAR_LAN_FLAGS, variant8_ui8(netconfig.lan.flg));
}

void lan_turn_on(void){
    TURN_LAN_ON(netconfig.lan.flg);
    wui_eeprom_set_var(NETVAR_LAN_FLAGS, variant8_ui8(netconfig.lan.flg));
    if (netif_is_link_up(&eth0)) {
        netifapi_netif_set_up(&eth0);
    }
}

int load_netconfig_ini_handler(void *user, const char *section, const char *name, const char *value) {
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
        strlcpy(tmp_config->hostname, value, LAN_HOSTNAME_LEN + 1);
        tmp_config->hostname[LAN_HOSTNAME_LEN] = '\0';
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
    }
#if BUDDY_ENABLE_DNS
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
    else if (MATCH("connect", "port")) {
        int32_t tmp_port = atoi(value);
        if ( tmp_port > 0 && tmp_port < MAX_UINT16){
            tmp_config->connect.port = tmp_port;
            tmp_config->set_flg |= NETVAR_MSK(NETVAR_CONNECT_PORT);
        }
    }
    else if (MATCH("connect", "address")) {
        if (ip4addr_aton(value, &tmp_config->connect.ip4)) {
            tmp_config->set_flg |= NETVAR_MSK(NETVAR_CONNECT_IP4);
        }
    } else if (MATCH("connect", "token")) {
        strlcpy(tmp_config->connect.token, value, CONNECT_TOKEN_LEN + 1);
        tmp_config->connect.token[CONNECT_TOKEN_LEN] = '\0';
        tmp_config->set_flg |= NETVAR_MSK(NETVAR_CONNECT_TOKEN);
    }
#endif // BUDDY_ENABLE_CONNECT
    else {
        return 0; /* unknown section/name, error */
    }
    return 1;
}

ETH_STATUS_t eth_status(void){
    ETH_STATUS_t ret;
    if (netif_is_link_up(&eth0)) {
        if (IS_LAN_STATIC(wui_eeprom_get_var(NETVAR_LAN_FLAGS).ui8)) {
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