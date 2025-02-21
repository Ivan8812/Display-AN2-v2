#include <can_port.h>
#include <can_messages.h>
#include <map>
#include <app.h>


using namespace std;

static map<uint8_t, uint8_t> cntrs;


//------------------------------------------------------------------------------
void can_init()
{
  FDCAN_FilterTypeDef filter_cfg;

  filter_cfg.IdType = FDCAN_EXTENDED_ID;
  filter_cfg.FilterConfig = FDCAN_FILTER_TO_RXBUFFER;
  filter_cfg.FilterType = FDCAN_FILTER_MASK;
  filter_cfg.FilterID2 = 0x1FFFFFFF;

  filter_cfg.FilterID1 = CAN_VAL_BINS1_ROLL;
  filter_cfg.RxBufferIndex = CAN_BUFNR_ROLL1;
  filter_cfg.FilterIndex = CAN_BUFNR_ROLL1;
  HAL_FDCAN_ConfigFilter(&can, &filter_cfg);

  filter_cfg.FilterID1 = CAN_VAL_BINS2_ROLL;
  filter_cfg.RxBufferIndex = CAN_BUFNR_ROLL2;
  filter_cfg.FilterIndex = CAN_BUFNR_ROLL2;
  HAL_FDCAN_ConfigFilter(&can, &filter_cfg);

  filter_cfg.FilterID1 = CAN_VAL_BINS1_PITCH;
  filter_cfg.RxBufferIndex = CAN_BUFNR_PITCH1;
  filter_cfg.FilterIndex = CAN_BUFNR_PITCH1;
  HAL_FDCAN_ConfigFilter(&can, &filter_cfg);

  filter_cfg.FilterID1 = CAN_VAL_BINS2_PITCH;
  filter_cfg.RxBufferIndex = CAN_BUFNR_PITCH2;
  filter_cfg.FilterIndex = CAN_BUFNR_PITCH2;
  HAL_FDCAN_ConfigFilter(&can, &filter_cfg);

  filter_cfg.FilterID1 = CAN_VAL_BINS1_HEADING;
  filter_cfg.RxBufferIndex = CAN_BUFNR_HEADING1;
  filter_cfg.FilterIndex = CAN_BUFNR_HEADING1;
  HAL_FDCAN_ConfigFilter(&can, &filter_cfg);

  filter_cfg.FilterID1 = CAN_VAL_BINS2_HEADING;
  filter_cfg.RxBufferIndex = CAN_BUFNR_HEADING2;
  filter_cfg.FilterIndex = CAN_BUFNR_HEADING2;
  HAL_FDCAN_ConfigFilter(&can, &filter_cfg);

  filter_cfg.FilterID1 = CAN_VAL_BINS1_PRES_STAT;
  filter_cfg.RxBufferIndex = CAN_BUFNR_PRES1;
  filter_cfg.FilterIndex = CAN_BUFNR_PRES1;
  HAL_FDCAN_ConfigFilter(&can, &filter_cfg);

  filter_cfg.FilterID1 = CAN_VAL_BINS2_PRES_STAT;
  filter_cfg.RxBufferIndex = CAN_BUFNR_PRES2;
  filter_cfg.FilterIndex = CAN_BUFNR_PRES2;
  HAL_FDCAN_ConfigFilter(&can, &filter_cfg);

  filter_cfg.FilterID1 = CAN_VAL_BINS1_IND_AIRSPEED;
  filter_cfg.RxBufferIndex = CAN_BUFNR_AIRSPEED1;
  filter_cfg.FilterIndex = CAN_BUFNR_AIRSPEED1;
  HAL_FDCAN_ConfigFilter(&can, &filter_cfg);

  filter_cfg.FilterID1 = CAN_VAL_BINS2_IND_AIRSPEED;
  filter_cfg.RxBufferIndex = CAN_BUFNR_AIRSPEED2;
  filter_cfg.FilterIndex = CAN_BUFNR_AIRSPEED2;
  HAL_FDCAN_ConfigFilter(&can, &filter_cfg);

  filter_cfg.FilterID1 = CAN_VAL_BINS1_VERT_SPEED;
  filter_cfg.RxBufferIndex = CAN_BUFNR_VERTSPEED1;
  filter_cfg.FilterIndex = CAN_BUFNR_VERTSPEED1;
  HAL_FDCAN_ConfigFilter(&can, &filter_cfg);

  filter_cfg.FilterID1 = CAN_VAL_BINS2_VERT_SPEED;
  filter_cfg.RxBufferIndex = CAN_BUFNR_VERTSPEED2;
  filter_cfg.FilterIndex = CAN_BUFNR_VERTSPEED2;
  HAL_FDCAN_ConfigFilter(&can, &filter_cfg);

#if USE_MTI_ICC
  filter_cfg.FilterID1 = CAN_CMD_ICC;
  filter_cfg.RxBufferIndex = CAN_BUFNR_ICC;
  filter_cfg.FilterIndex = CAN_BUFNR_ICC;
  HAL_FDCAN_ConfigFilter(fdcan, &filter_cfg);
#endif

  HAL_FDCAN_Start(&can);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
bool can_send_dat(uint16_t id, const void* data, const uint8_t len)
{
	if(len > 8)
		return false;

	if(HAL_FDCAN_GetTxFifoFreeLevel(&can) == 0)
		return false;

	FDCAN_TxHeaderTypeDef tx_hdr;
	tx_hdr.Identifier = id;
	tx_hdr.IdType = FDCAN_EXTENDED_ID;
	tx_hdr.TxFrameType = FDCAN_DATA_FRAME;
	tx_hdr.DataLength = len;
	tx_hdr.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
	tx_hdr.BitRateSwitch = FDCAN_BRS_OFF;
	tx_hdr.FDFormat = FDCAN_CLASSIC_CAN;
	tx_hdr.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
	tx_hdr.MessageMarker = 0;
	return (HAL_FDCAN_AddMessageToTxFifoQ(&can, &tx_hdr, (uint8_t*)data) == HAL_OK) ? true : false;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
bool can_send_val(uint16_t id, float val, bool valid)
{
	if(cntrs.contains(id))
		cntrs[id]++;
	else
		cntrs[id] = 0;

	can_val_t msg;
	msg.state.raw = 0x0;
	msg.state.bins_ok = true;
	msg.state.valid = valid;
	msg.value = val;
	msg.cntr = cntrs[id];

	return can_send_dat(id, &msg, sizeof(msg));
}
//------------------------------------------------------------------------------




