/*
 * lv_port.h
 *
 *  Created on: Jul 20, 2023
 *      Author: Иван
 */

#ifndef LV_PORT_H_
#define LV_PORT_H_

#ifdef __cplusplus
extern "C" {
#endif

#define DISP_HOR_RES    320
#define DISP_VER_RES    240

#include "lvgl.h"


/* Initialize low level display driver */
void lv_port_disp_init(void);


#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LV_PORT_H_ */
