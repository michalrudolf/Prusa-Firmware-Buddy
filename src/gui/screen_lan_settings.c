/*
 * screen_lan_settings.c
 *
 *  Created on: Nov 27, 2019
 *      Author: Migi
 */

#include "screen_lan_settings.h"
#include "ini_handler.h"
#include "lwip/dhcp.h"
#include "lwip/netifapi.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "netif_settings.h"

typedef enum {
    MI_RETURN,
    MI_SWITCH,
    MI_TYPE,
    MI_SAVE,
    MI_LOAD,
} MI_t;

static char *plan_str = NULL;
static bool conn_flg = false; // wait for dhcp to supply addresses
static networkconfig_t config;
static const char *LAN_switch_opt[] = { "On", "Off", NULL };
static const char *LAN_type_opt[] = { "DHCP", "static", NULL };
extern bool media_is_inserted();
const menu_item_t _menu_lan_items[] = {
    { { "LAN", 0, WI_SWITCH, .wi_switch_select = { 0, LAN_switch_opt } }, SCREEN_MENU_NO_SCREEN },
    { { "LAN IP", 0, WI_SWITCH, .wi_switch_select = { 0, LAN_type_opt } }, SCREEN_MENU_NO_SCREEN },
    { { "Save settings", 0, WI_LABEL }, SCREEN_MENU_NO_SCREEN },
    { { "Load settings", 0, WI_LABEL }, SCREEN_MENU_NO_SCREEN },
};

static void _screen_lan_settings_item(window_menu_t *pwindow_menu, uint16_t index,
    window_menu_item_t **ppitem, void *data) {
    screen_t *screen = (screen_t *)data;
    *ppitem = &(plsd->items[index].item);
}

static void stringify_net_info_for_screen(char * dest) {
    snprintf(dest, 150, "IPv4 Address:\n  %s      \nIPv4 Netmask:\n  %s      \nIPv4 Gateway:\n  %s      \nMAC Address:\n  %s",
        addr_ip4_str(), msk_ip4_str(), gw_ip4_str(), mac_addr_str());
}
// MOVE TO INI FILE HANDLER
static void stringify_net_info_for_ini(char * dest){
#ifdef BUDDY_ENABLE_CONNECT
    snprintf(dest, MAX_INI_SIZE, "[lan_ip4]\ntype=%s\nhostname=%s\naddress=%s\nmask=%s\ngateway=%s\n\n[connect]\naddress=%s\ntoken=%s\n",
        netconfig.lan.flg & LAN_MSK_TYPE ? LAN_type_opt[1] : LAN_type_opt[0], netconfig.hostname, addr_ip4_str(), msk_ip4_str(), gw_ip4_str(),
        connect_ip4_str(), netconfig.connect.token);
#else
    snprintf(dest, MAX_INI_SIZE, "[lan_ip4]\ntype=%s\nhostname=%s\naddress=%s\nmask=%s\ngateway=%s\n",
        netconfig.lan.flg & LAN_EEFLG_TYPE ? LAN_type_opt[1] : LAN_type_opt[0], netconfig.hostname, addr_ip4_str(), msk_ip4_str(), gw_ip4_str());
#endif // BUDDY_ENABLE_CONNECT
}

static void refresh_addrs(screen_t *screen) {
    update_netconfig(NETVAR_STATIC_LAN_ADDRS);
    stringify_net_info_for_screen(plan_str);
    plsd->text.text = plan_str;
    plsd->text.win.flg |= WINDOW_FLG_INVALID;
    gui_invalidate();
}

static void screen_lan_settings_init(screen_t *screen) {
    //============= SCREEN INIT ===============

    size_t count = sizeof(_menu_lan_items) / sizeof(menu_item_t);

    plsd->items = (menu_item_t *)malloc(sizeof(menu_item_t) * (count + 1));
    memset(plsd->items, '\0', sizeof(menu_item_t) * (count + 1));

    rect_ui16_t menu_rect = rect_ui16(10, 32, 220, 150);

    int16_t id;
    int16_t root = window_create_ptr(WINDOW_CLS_FRAME, -1, rect_ui16(0, 0, 0, 0), &(plsd->root));
    window_disable(root);

    id = window_create_ptr(WINDOW_CLS_HEADER, root, rect_ui16(0, 0, 240, 31), &(plsd->header));
    p_window_header_set_text(&(plsd->header), "LAN SETTINGS");

    id = window_create_ptr(WINDOW_CLS_MENU, root, menu_rect, &(plsd->menu));
    plsd->menu.padding = padding_ui8(20, 6, 2, 6);
    plsd->menu.icon_rect = rect_ui16(0, 0, 16, 30);
    plsd->menu.count = count + 1;
    plsd->menu.menu_items = _screen_lan_settings_item;
    plsd->menu.data = (void *)screen;

    window_set_capture(id); // set capture to list
    window_set_focus(id);

    id = window_create_ptr(WINDOW_CLS_TEXT, root, rect_ui16(10, 183, 230, 137), &(plsd->text));
    plsd->text.font = resource_font(IDR_FNT_SPECIAL);

    plsd->items[0] = menu_item_return;
    memcpy(plsd->items + 1, _menu_lan_items, count * sizeof(menu_item_t));

    update_netconfig(NETVAR_MSK(NETVAR_LAN_FLAGS));

    plsd->items[MI_SWITCH].item.wi_switch_select.index = config.lan.flg & LAN_MSK_ONOFF ? LAN_EEFLG_OFF : LAN_EEFLG_ON;
    plsd->items[MI_TYPE].item.wi_switch_select.index = config.lan.flg & LAN_MSK_TYPE ? LAN_EEFLG_STATIC : LAN_EEFLG_DHCP;
    if ((netconfig.lan.flg & LAN_MSK_ONOFF) == LAN_EEFLG_ON && 
        (netconfig.lan.flg & LAN_MSK_TYPE) == LAN_EEFLG_DHCP &&
        !dhcp_supplied_address(&eth0)) {
        conn_flg = true;
    }

    //============== DECLARE VARIABLES ================

    plan_str = (char *)gui_malloc(150 * sizeof(char));

    //============= FILL VARIABLES ============

    update_netconfig(NETVAR_MSK(NETVAR_MAC_ADDR));
    refresh_addrs(screen);
}
static uint8_t save_config(void) {
    stringify_net_info_for_ini(ini_file_str); //1 means parsing to ini file format
    return ini_save_file(ini_file_str);
}

static int ini_load_handler(void *user, const char *section, const char *name, const char *value) {
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
    }
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

static uint8_t load_config(void) {

    networkconfig_t tmp_config;
    tmp_config.lan.flg = config.lan.flg;
    tmp_config.set_flg = 0;

    if (ini_load_file(ini_load_handler, &tmp_config) == 0) {
        return 0;
    }

    set_loaded_netconfig(&tmp_config);
    
    return 1;
}
static int screen_lan_settings_event(screen_t *screen, window_t *window,
    uint8_t event, void *param) {

    window_header_events(&(plsd->header));

    if (conn_flg) {
        if ((config.lan.flg & LAN_MSK_TYPE) == LAN_EEFLG_DHCP || dhcp_supplied_address(&eth0)) {
            conn_flg = false;
            refresh_addrs(screen);
        }
    }

    if (event != WINDOW_EVENT_CLICK) {
        return 0;
    }

    switch ((int)param) {
    case MI_RETURN:
        screen_close();
        return 1;
    case MI_SWITCH: {
        if ((netconfig.lan.flg & LAN_MSK_ONOFF) == LAN_EEFLG_ON) {
            lan_turn_off();
            refresh_addrs(screen);
        } else {
            lan_turn_on();
            refresh_addrs(screen);
            conn_flg = true;
        }
        break;
    }
    case MI_TYPE: {
        if ((netconfig.lan.flg & LAN_MSK_TYPE) == LAN_EEFLG_STATIC) {
            if (eeprom_get_var(EEVAR_LAN_IP4_ADDR).ui32 == 0) {
                if (gui_msgbox("Static IPv4 addresses were not set.",
                    MSGBOX_BTN_OK | MSGBOX_ICO_ERROR) == MSGBOX_RES_OK) {
                    plsd->items[MI_TYPE].item.wi_switch_select.index = 0;
                }
                return 0;
            }
            lan_set_static();
            refresh_addrs(screen);
        } else {
            lan_set_dhcp();
            refresh_addrs(screen);
            if ((netconfig.lan.flg & LAN_MSK_ONOFF) == LAN_EEFLG_ON) {
                conn_flg = true;
            }
        }
        break;
    }
    case MI_SAVE:
        if (media_is_inserted() == false) {
            if (gui_msgbox("Please insert a USB drive and try again.",
                    MSGBOX_BTN_OK | MSGBOX_ICO_ERROR)
                == MSGBOX_RES_OK) {
            }
        } else {
            if (save_config()) { // !its possible to save empty configurations!
                if (gui_msgbox("The settings have been saved successfully in the \"lan_settings.ini\" file.",
                        MSGBOX_BTN_OK | MSGBOX_ICO_INFO)
                    == MSGBOX_RES_OK) {
                }
            } else {
                if (gui_msgbox("There was an error saving the settings in the \"lan_settings.ini\" file.",
                        MSGBOX_BTN_OK | MSGBOX_ICO_ERROR)
                    == MSGBOX_RES_OK) {
                }
            }
        }
        break;
    case MI_LOAD:
        if (media_is_inserted() == false) {
            if (gui_msgbox("Please insert USB flash disk and try again.",
                    MSGBOX_BTN_OK | MSGBOX_ICO_ERROR)
                == MSGBOX_RES_OK) {
            }
        } else {
            if (load_config()) {
                if (gui_msgbox("Settings successfully loaded", MSGBOX_BTN_OK | MSGBOX_ICO_INFO) == MSGBOX_RES_OK) {
                    plsd->items[MI_TYPE].item.wi_switch_select.index = config.lan.flg & LAN_MSK_TYPE ? LAN_EEFLG_STATIC : LAN_EEFLG_DHCP;
                    window_invalidate(plsd->menu.win.id);
                    refresh_addrs(screen);
                    if ((config.lan.flg & LAN_MSK_TYPE) == LAN_EEFLG_DHCP) {
                        conn_flg = true;
                    }
                }
            } else {
                if (gui_msgbox("IP addresses are not valid or the file \"lan_settings.ini\" is not in the root directory of the USB drive.",
                        MSGBOX_BTN_OK | MSGBOX_ICO_ERROR)
                    == MSGBOX_RES_OK) {
                }
            }
        }
        break;
    }
    return 0;
}

static void screen_lan_settings_draw(screen_t *screen) {
}

static void screen_lan_settings_done(screen_t *screen) {

    if (plan_str)
        gui_free(plan_str);
    window_destroy(plsd->root.win.id);
}

screen_t screen_lan_settings = {
    0,
    0,
    screen_lan_settings_init,
    screen_lan_settings_done,
    screen_lan_settings_draw,
    screen_lan_settings_event,
    sizeof(screen_lan_settings_data_t), //data_size
    0,                                  //pdata
};

screen_t *const pscreen_lan_settings = &screen_lan_settings;
