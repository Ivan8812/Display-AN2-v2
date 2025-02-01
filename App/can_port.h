#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#define CAN_ICC_BUFNR	0

#include "stdbool.h"
#include "can_messages.h"

void can_init(void);
bool can_send_dat(uint16_t id, void* data, const uint8_t len);
bool can_send_val(uint16_t id, float val, bool valid);

#ifdef __cplusplus
} // extern "C"
#endif
