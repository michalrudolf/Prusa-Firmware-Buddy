/*
 * wui_api.h
 * \brief   interface functions for Web User Interface library
 *
 *  Created on: April 22, 2020
 *      Author: joshy <joshymjose[at]gmail.com>
 */

#ifndef _WUI_API_H_
#define _WUI_API_H_

#include "ethvars.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!****************************************************************************
* \brief saves the Ethernet specific parameters to non-volatile memory
*
* \param [in] ETH_config_t* pointer to struct with parameters
*
* \return   uint32_t    error value
*
* \retval   0 if successful
*****************************************************************************/
uint32_t save_eth_params(ETH_config_t * config);

/*!****************************************************************************
* \brief load from ini file Ethernet specific parameters
*
* \param [out] ETH_Config_t* pointer to struct with parameters
*
* \return   uint32_t    error value
*
* \retval   0 if successful
*****************************************************************************/
void load_ini_params(ETH_config_t * config);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* _WUI_API_H_ */
