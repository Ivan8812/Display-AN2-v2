#pragma once

#include "stm32h7xx_hal.h"

#define SDRAM_BASE	0x60000000

void sdram_init(SDRAM_HandleTypeDef *hsdram);
