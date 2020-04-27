/*
 * netif_settings.c
 * \brief   function definitions that set up network interface
 *
 *  Created on: April 27, 2020
 *      Author: Migi <michal.rudolf[at]prusa3d.cz>
 */

 #include "netif_settings.h"
 #include "netifapi.h"
 #include "string.h"

#define IP4_ADDR_STR_SIZE   16

struct netif eth0;

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

void lan_set_static(void) {
    if (netif_is_up(&eth0)) {
        netifapi_netif_set_down(&eth0);
    }
    CHANGE_LAN_TO_STATIC(ethconfig.lan.flag);
    save_eth_params(ETHVAR_MSK(NETVAR_LAN_FLAGS));
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
    CHANGE_LAN_TO_DHCP(ethconfig.lan.flg);
    save_eth_params(ETHVAR_MSK(NETVAR_LAN_FLAGS));
    if (netif_is_link_up(&eth0) && IS_LAN_ON(ethconfig.lan.flg)) {
        netifapi_netif_set_up(&eth0);
    }
}

uint32_t set_loaded_eth_params(void){
    ethconfig.var_mask = 0;
    uint8_t previous_lan_flag;
    if (tmp_ethconfig.var_mask & ETHVAR_MSK(ETHVAR_LAN_FLAGS)) {
        previous_lan_flag = ethconfig.lan.flag;
        // if lan type is set to STATIC
        if (IS_LAN_STATIC(tmp_ethconfig->lan.flag)){
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
        if (IS_LAN_STATIC(previous_lan_flag) && IS_LAN_DHCP(ethconfig.lan.flg)) {
            lan_set_dhcp();
        // or STATIC to STATIC
        } else if (IS_LAN_STATIC(ethconfig.lan.flg)){
            lan_set_static();
        }   
        // from DHCP to DHCP: do nothing
    }

    return 0;
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

void lan_turn_off(void){
    if (netif_is_up(&eth0)) {
        netifapi_netif_set_down(&eth0);
    }
    TURN_LAN_OFF(ethconfig.lan.flg);
    save_eth_params(ETHVAR_MSK(ETHVAR_LAN_FLAGS));
}

void lan_turn_on(void){
    TURN_LAN_ON(ethconfig.lan.flg);
    save_eth_params(ETHVAR_MSK(ETHVAR_LAN_FLAGS));
    if (netif_is_link_up(&eth0)) {
        netifapi_netif_set_up(&eth0);
    }
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