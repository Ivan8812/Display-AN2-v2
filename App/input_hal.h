#pragma once

#ifdef __cplusplus
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wvolatile"
extern "C"
{
#endif

#include "stdint.h"
#include "stdbool.h"

#include "tim.h"
#include "main.h"

void input_init()
{
 	HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_1);
}

int8_t input_get_enc()
{
	return (__HAL_TIM_GET_COUNTER(&htim2) >> 1) & 0xFF;
}

bool input_get_btn()
{
	return (HAL_GPIO_ReadPin(ENC_BTN_GPIO_Port, ENC_BTN_Pin) == GPIO_PIN_RESET);
}

#ifdef __cplusplus
#pragma GCC diagnostic pop
} // extern "C"
#endif
