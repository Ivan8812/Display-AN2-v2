#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#define CAN_ICC_BUFNR	0

#include "stdbool.h"
#include "can_messages.h"


enum
{
  CAN_BUFNR_ROLL1,
  CAN_BUFNR_ROLL2,
  CAN_BUFNR_PITCH1,
  CAN_BUFNR_PITCH2,
  CAN_BUFNR_HEADING1,
  CAN_BUFNR_HEADING2,
  CAN_BUFNR_PRES1,
  CAN_BUFNR_PRES2,
  CAN_BUFNR_AIRSPEED1,
  CAN_BUFNR_AIRSPEED2,
  CAN_BUFNR_VERTSPEED1,
  CAN_BUFNR_VERTSPEED2,

  // must be the last in enum
  CAN_TOTAL_BUFNR,
};


void can_init(void);
bool can_send_dat(uint16_t id, void* data, const uint8_t len);
bool can_send_val(uint16_t id, float val, bool valid);

#ifdef __cplusplus
} // extern "C"
#endif
