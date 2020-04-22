/*
 * wui_api.h
 * \brief   interface functions for Web User Interface library
 *
 *  Created on: April 22, 2020
 *      Author: joshy <joshymjose[at]gmail.com>
 */

#ifndef _WUI_API_H_
#define _WUI_API_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {

} wui_eth_params_s;

/* \breif function to pass ethernet parameters for WUI
*/
void get_eth_params(wui_eth_params_s eth_params);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* _WUI_API_H_ */
