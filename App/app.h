#pragma once

#define APP_DEBUG 0
#ifdef __cplusplus
extern "C"
{
#endif

#include "main.h"
#include "fdcan.h"

void app_run(void);
void app_on_timer(void);


#ifdef __cplusplus
} // extern "C"
#endif
