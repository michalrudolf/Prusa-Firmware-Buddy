#include "netif_settings.h"
#include "eeprom.h"
#include "lwip/dhcp.h"
#include "lwip/netifapi.h"

#define MAX_UINT16  65535

static void lan_set_static(void) {
    if (netif_is_up(&eth0)) {
        netifapi_netif_set_down(&eth0);
    }
    CHANGE_LAN_TO_STATIC(netconfig.lan.flg);
    eeprom_set_var(EEVAR_LAN_FLAG, variant8_ui8(netconfig.lan.flg));
    netifapi_netif_set_addr(&eth0,
        (const ip4_addr_t *)&(netconfig.lan.addr_ip4),
        (const ip4_addr_t *)&(netconfig.lan.msk_ip4),
        (const ip4_addr_t *)&(netconfig.lan.gw_ip4)
        );
    if (netif_is_link_up(&eth0) && ((netconfig.lan.flg & LAN_MSK_ONOFF) == LAN_EEFLG_ON)) {
        netifapi_netif_set_up(&eth0);
    }
}

static void lan_set_dhcp(void) {
    if (netif_is_up(&eth0)) {        
        netifapi_netif_set_down(&eth0);
    }
    CHANGE_LAN_TO_DHCP(netconfig.lan.flg);
    eeprom_set_var(EEVAR_LAN_FLAG, variant8_ui8(netconfig.lan.flg));
    if (netif_is_link_up(&eth0) && ((netconfig.lan.flg & LAN_MSK_ONOFF) == LAN_EEFLG_ON)) {
        netifapi_netif_set_up(&eth0);
    }
}

/*  User can set any of these values independentely (none of them have to be in ini file), but:
*       If type=STATIC      ->      All 3 lan addrs have to be there and be valid ip addrs
*       If type=DHCP        ->      All 3 lan addrs are skipped
*       If Connect IP is there, it has to be valid ip addr
*       If Connect token is there, it will set first 20chars (max len)
*       If lan hostname is there, it will set first 20chars (max len)
*/
uint8_t set_loaded_net_config(networkconfig_t tmp_config){

    // type=STATIC/DHCP is in INI file
    if (tmp_config.set_flg & NETVAR_MSK(NETVAR_LAN_FLAGS)) {
        // if lan type is set to STATIC
        if ((tmp_config.lan.flg & LAN_MSK_TYPE) == LAN_EEFLG_STATIC){
            if ((tmp_config.set_flg & NETVAR_STATIC_LAN_ADDRS) != NETVAR_STATIC_LAN_ADDRS) {
                return 0;
            }
            eeprom_set_var(EEVAR_LAN_IP4_ADDR, variant8_ui32(tmp_config.lan.addr_ip4.addr));
            eeprom_set_var(EEVAR_LAN_IP4_MSK, variant8_ui32(tmp_config.lan.msk_ip4.addr));
            eeprom_set_var(EEVAR_LAN_IP4_GW, variant8_ui32(tmp_config.lan.gw_ip4.addr));
        }
    }
    if (tmp_config.set_flg & NETVAR_MSK(NETVAR_HOSTNAME)) {
        strlcpy(interface_hostname, tmp_config.hostname, LAN_HOSTNAME_MAX_LEN + 1);
        eth0.hostname = interface_hostname;
        variant8_t hostname = variant8_pchar(tmp_config.hostname, 0, 0);
        eeprom_set_var(EEVAR_LAN_HOSTNAME, hostname);
        //variant8_done() is not called, variant_pchar with init flag 0 doesnt hold its memory
    }
#ifdef BUDDY_ENABLE_CONNECT
    if (tmp_config.set_flg & NETVAR_MSK(NETVAR_CONNECT_TOKEN)) {
        variant8_t token = variant8_pchar(tmp_config.connect.token, 0, 0);
        eeprom_set_var(EEVAR_CONNECT_TOKEN, token);
        //variant8_done() is not called, variant_pchar with init flag 0 doesnt hold its memory
    }
    if (tmp_config.set_flg & NETVAR_MSK(NETVAR_CONNECT_IP4)) {
        eeprom_set_var(EEVAR_CONNECT_IP4, variant8_ui32(tmp_config.connect.ip4.addr));
    }
#endif // BUDDY_ENABLE_CONNECT

    // if type=STATIC/DHCP is in INI file   
    if (tmp_config.set_flg & NETVAR_MSK(NETVAR_LAN_FLAGS)) {
        // if there was a change from STATIC to DHCP
        if ((tmp_config.lan.flg & LAN_MSK_TYPE) == LAN_EEFLG_DHCP && 
            (netconfig.lan.flg & LAN_MSK_TYPE) != (tmp_config.lan.flg & LAN_MSK_TYPE)) {
            lan_set_dhcp();
        // or STATIC to STATIC
        } else if ((tmp_config.lan.flg & LAN_MSK_TYPE) == LAN_EEFLG_STATIC){
            lan_set_static();
        }   
        // from DHCP to DHCP: do nothing
    }
    return 1;
}