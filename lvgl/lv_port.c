/*
 * lv_port.c
 *
 *  Created on: Jul 20, 2023
 *      Author: Иван
 */

#include "lv_port.h"
#include "ltdc.h"

static uint8_t display_buf[2][DISP_HOR_RES*DISP_VER_RES*2] __attribute__ ((section(".video_ram")));

static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);
static volatile uint32_t buf_addr = (uint32_t)display_buf[0];
static lv_disp_drv_t disp_drv;

//------------------------------------------------------------------------------
void lv_port_disp_init(void)
{
	HAL_LTDC_SetAddress(&hltdc, (uint32_t)display_buf[0], 0);
	HAL_LTDC_EnableDither(&hltdc);

  static lv_disp_draw_buf_t draw_buf;
  lv_disp_draw_buf_init(&draw_buf, display_buf[0], display_buf[1], DISP_HOR_RES*DISP_VER_RES);

  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = DISP_HOR_RES;
  disp_drv.ver_res = DISP_VER_RES;
  disp_drv.flush_cb = disp_flush;
  disp_drv.draw_buf = &draw_buf;
  disp_drv.full_refresh = true;
  disp_drv.antialiasing = 1;
  lv_disp_drv_register(&disp_drv);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
/*Flush the content of the internal buffer the specific area on the display.
 *`px_map` contains the rendered image as raw pixel map and it should be copied to `area` on the display.
 *You can use DMA or any hardware acceleration to do this operation in the background but
 *'lv_disp_flush_ready()' has to be called when it's finished.*/
static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
	buf_addr = (uint32_t)color_p;
	HAL_LTDC_ProgramLineEvent(&hltdc, 0);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void HAL_LTDC_LineEventCallback(LTDC_HandleTypeDef *hltdc)
{
	HAL_LTDC_SetAddress(hltdc, buf_addr, 0);
	lv_disp_flush_ready(&disp_drv);
}
//------------------------------------------------------------------------------


