#pragma once

#define APP_DEBUG 0
#ifdef __cplusplus
extern "C"
{
#endif

#include "main.h"
#include "fdcan.h"

//--------------------------------------
// версия ПО
const uint8_t MajVersion = 0;
const uint8_t MinVersion = 1;
const uint8_t Build = 1;
//--------------------------------------

void app_run(void);
void app_on_timer(void);


#ifdef __cplusplus
} // extern "C"
#endif
