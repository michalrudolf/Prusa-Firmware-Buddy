/*
 * netif_settings.h
 * \brief   functions that set up network interface
 *
 *  Created on: April 27, 2020
 *      Author: Migi <michal.rudolf[at]prusa3d.cz>
 */
 #ifndef NETIF_SETTINGS_H
 #define NETIF_SETTINGS_H

#include "wui_api.h"

/*!****************************************************************************
* \brief Sets network interface of LAN to DHCP server aquiring of ip4 addresses
*****************************************************************************/
void lan_set_dhcp(void);
/*!****************************************************************************
* \brief Sets network interface of LAN to STATIC aquiring of ip4 addresses
*****************************************************************************/
void lan_set_static(void);
/*!****************************************************************************
* \brief Get pointer to static mac address string
*
* \retval static mac address string
*****************************************************************************/
char * mac_addr_str(void);
/*!****************************************************************************
* \brief Get pointer to static ip4 address string
*
* \retval static ip4 address string
*****************************************************************************/
const char * addr_ip4_str(void);
/*!****************************************************************************
* \brief Get pointer to static netmask ip4 string
*
* \retval static ip4 string
*****************************************************************************/
const char * msk_ip4_str(void);
/*!****************************************************************************
* \brief Get pointer to static default gateway ip4 string
*
* \retval static ip4 string
*****************************************************************************/
const char * gw_ip4_str(void);
/*!****************************************************************************
* \brief Get pointer to static connect ip4 string
*
* \retval static ip4 string
*****************************************************************************/
const char * connect_ip4_str(void);
/*!****************************************************************************
* \brief Turns software switch of LAN netif to OFF
*****************************************************************************/
void lan_turn_off(void);
/*!****************************************************************************
* \brief Turns software switch of LAN netif to ON
*****************************************************************************/
void lan_turn_on(void);
/*!****************************************************************************
* \brief Parses MAC address from memory to static string
*****************************************************************************/
void parse_MAC_addr(void);
/*!****************************************************************************
* \brief Returns the status of ethernet link/netif
*
* \return ETH_STATUS_t enum for possible cases (unlinked, netif down, netif up)
*****************************************************************************/
ETH_STATUS_t eth_status(void);
/*!****************************************************************************
* \brief Sets up network interface according to loaded configuration values
*
* \return   uint32_t    error value
*
* \retval   0 if successful
*****************************************************************************/
uint32_t set_loaded_eth_params(void);
/*!******************************************************************************************
* \brief Updates ethernet addresses and their static strings according to ethconfig structure
********************************************************************************************/
void update_eth_addrs(void);
 #endif //NETIF_SETTINGS_H