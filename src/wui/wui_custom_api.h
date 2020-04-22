#ifndef WUI_EEPROM_API_H
#define WUI_EEPROM_API_H

#include "netvars.h"

void wui_set_netvar(uint8_t net_id, variant8_t val);
variant8_t wui_get_netvar(uint8_t id);

#endif //WUI_EEPROM_API_H