#pragma once

#include "stdint.h"

#pragma pack(push, 1)


//----------------------------
typedef union
{
  uint8_t raw;
  struct
  {
    uint8_t bins_ok:1;
    uint8_t valid:1;
    uint8_t test:1;
    uint8_t calibr:1;
  };
} can_msg_state_t;
//----------------------------


//----------------------------
// формат передачи вещественных значений из БИНС.
// в этом формате передается ориентация БИНС, скорости, давления
typedef struct
{
  float value;
  uint8_t cntr;
  can_msg_state_t state;
} can_val_t;
//--------------------------------
// углы в радианах
#define CAN_GROUP_ATTITUDE          0x10
#define CAN_VAL_BINS1_ROLL          0x11
#define CAN_VAL_BINS1_PITCH         0x12
#define CAN_VAL_BINS1_HEADING       0x13
#define CAN_VAL_BINS2_ROLL          0x19
#define CAN_VAL_BINS2_PITCH         0x1A
#define CAN_VAL_BINS2_HEADING       0x1B
//------------------------------
// давление, Па
#define CAN_GROUP_PRES              0x20
#define CAN_VAL_BINS1_PRES_STAT     0x21
#define CAN_VAL_BINS1_PRES_FULL     0x22
#define CAN_VAL_BINS1_PRES_DYN      0x23
#define CAN_VAL_BINS2_PRES_STAT     0x29
#define CAN_VAL_BINS2_PRES_FULL     0x2A
#define CAN_VAL_BINS2_PRES_DYN      0x2B
//--------------------------------
// скорость, м/с
#define CAN_GROUP_SPEED             0x40
#define CAN_VAL_BINS1_IND_AIRSPEED  0x41
#define CAN_VAL_BINS1_VERT_SPEED    0x42
#define CAN_VAL_BINS2_IND_AIRSPEED  0x49
#define CAN_VAL_BINS2_VERT_SPEED    0x4A
//--------------------------------


//--------------------------------
typedef struct
{
  int32_t value;
  uint8_t quality;
  uint8_t cntr;
  can_msg_state_t state;
} can_msg_position_t;

enum
{
  CAN_POS_QUAL_STANDALONE,
  CAN_POS_QUAL_GNSS_FIX2D,
  CAN_POS_QUAL_GNSS_FIX3D,
  CAN_POS_QUAL_GNSS_SBAS,
  CAN_POS_QUAL_GNSS_RTK_FLOAT,
  CAN_POS_QUAL_GNSS_RTK_FIXED
};

// широта, долгота в градусах ЦМР 1e-7, высота - мм
#define CAN_GROUP_POSITION          0x30
#define CAN_MSG_BINS1_LONGITUDE     0x31
#define CAN_MSG_BINS1_LATITUDE      0x32
#define CAN_MSG_BINS1_BARO_HEIGHT   0x33
#define CAN_MSG_BINS1_GNSS_HEIGHT   0x34
#define CAN_MSG_BINS2_LONGITUDE     0x39
#define CAN_MSG_BINS2_LATITUDE      0x3A
#define CAN_MSG_BINS2_BARO_HEIGHT   0x3B
#define CAN_MSG_BINS2_GNSS_HEIGHT   0x3C
//--------------------------------


//--------------------------------
// время и дата UTC (из гнсс)
typedef struct
{
  uint16_t year;
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t min;
  uint8_t sec;
  can_msg_state_t state;
} can_msg_time_t;
#define CAN_GROUP_TIME              0x100
#define CAN_MSG_BINS1_TIME_DATE     0x101
#define CAN_MSG_BINS2_TIME_DATE     0x109
//--------------------------------


//--------------------------------
// периодически отправляемые пустые сообщения
#define CAN_GROUP_HEARTBEAT         0x400
#define CAN_HEARTBEAT_MFI           0x400
#define CAN_HEARTBEAT_BINS1         0x401
#define CAN_HEARTBEAT_BINS2         0x402

//--------------------------------


//--------------------------------
// состояние MTI (технологическое сообщение)
#define CAN_GROUP_MTI               0x410
#define CAN_MTI_BINS1_STATUS        0x411
#define CAN_MTI_BINS2_STATUS        0x419
//--------------------------------
// слово состояния MTI
typedef uint32_t can_msg_mti_stat_t;
//--------------------------------

#pragma pack(pop)


