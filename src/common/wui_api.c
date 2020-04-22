#include "wui_custom_api.h"
#include "eeprom.h"

static uint8_t get_eeprom_id(uint8_t net_id){
    uint8_t ret = 0;
    if(net_id == NETVAR_LAN_FLAGS){
        ret = EEVAR_LAN_FLAG;
    } else if (net_id == NETVAR_LAN_IP4_ADDR){
        ret = EEVAR_LAN_IP4_ADDR;
    } else if (net_id == NETVAR_LAN_IP4_MSK){
        ret = EEVAR_LAN_IP4_MSK;
    } else if (net_id == NETVAR_LAN_IP4_GW){
        ret = EEVAR_LAN_IP4_GW;
    } else if (net_id == NETVAR_CONNECT_IP4){
        ret = EEVAR_CONNECT_IP4;
    } else if (net_id == NETVAR_CONNECT_TOKEN){
        ret = EEVAR_CONNECT_TOKEN;
    } else if (net_id == NETVAR_CONNECT_PORT){
        ret = EEVAR_CONNECT_PORT;
    } else if (net_id == NETVAR_HOSTNAME){
        ret = EEVAR_LAN_HOSTNAME;
    } else if (net_id == NETVAR_DNS1_IP4){
        ret = EEVAR_DNS1_IP4;
    } else if (net_id == NETVAR_DNS2_IP4){
        ret = EEVAR_DNS2_IP4;
    }
    return ret;
}

void wui_set_netvar(uint8_t net_id, variant8_t val){
    return eeprom_set_var(get_eeprom_id(net_id), val);
}

variant8_t wui_get_netvar(uint8_t net_id){
    return eeprom_get_var(get_eeprom_id(net_id));
}