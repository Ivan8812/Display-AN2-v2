#include <air_signals.h>
#include <app.h>
#include <input_hal.h>
#include <disp_bright_hal.h>
#include <RotaryEncoder.h>
#include <rs422_port.h>
#include <can_port.h>
#include <math.h>
#include <gui.h>
#include <cstdio>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wvolatile"
#include "fdcan.h"
#pragma GCC diagnostic pop

#if APP_DEBUG
	#define debug_print rs422_printf
#else
	static void debug_print(const char* fmt...) {}
#endif

using namespace std;

#define PI			3.141592653f

static void dummy_hold() {debug_print("dummy hold\r");};
static void dummy_timeout() {debug_print("dummy timeout\r");};

static void brg_rotate(int8_t delta);
static void brg_click();
static void brg_hold();

static void ref_rotate(int8_t delta);
static void ref_click();

static void pres_rotate(int8_t delta);
static void pres_click();

static void	menu_rotate(int8_t delta);
static void menu_click();
static void menu_hold();
static void menu_timeout();
static void	menu_select(uint8_t item);
static void	pres_select(uint8_t item);
static void	alt_select(uint8_t item);
static void	ver_select(uint8_t item);
#if USE_MTI_ICC
static void	icc_select(uint8_t item);
static void	bad_icc_select(uint8_t item);
static void	good_icc_select(uint8_t item);
#endif
static void can_serve(uint32_t dt);
static void can_serve_init();


static RotaryEncoder::listener_t enc_brg = {brg_rotate, brg_click, brg_hold, dummy_timeout};
static RotaryEncoder::listener_t enc_ref = {ref_rotate, ref_click, dummy_hold, dummy_timeout};
static RotaryEncoder::listener_t enc_pres = {pres_rotate, pres_click, dummy_hold, dummy_timeout};
static RotaryEncoder::listener_t enc_menu = {menu_rotate, menu_click, menu_hold, menu_timeout};
static RotaryEncoder::listener_t enc_menu_timeless = {menu_rotate, menu_click, menu_hold, dummy_timeout};

#if USE_MTI_ICC
static const char* main_menu_items[] = {"ЕД. ИЗМ. ДАВЛЕНИЯ","ЕД. ИЗМ. ВЫСОТЫ", "КАЛИБРОВКА КОМПАСА", "ВЕРСИЯ"};
static gui_menu_t menu_main = {NULL, main_menu_items, 4, 0, menu_select};
#else
static const char* main_menu_items[] = {"ЕД. ИЗМ. ДАВЛЕНИЯ","ЕД. ИЗМ. ВЫСОТЫ", "ВЕРСИЯ"};
static gui_menu_t menu_main = {NULL, main_menu_items, 3, 0, menu_select};
#endif

static const char* pres_items[] = {"мм. рт. ст","гПа"};
static gui_menu_t menu_pres = {"ЕДИНИЦЫ ДАВЛЕНИЯ", pres_items, 2, 0, pres_select};

static const char* alt_items[] = {"метры","футы"};
static gui_menu_t menu_alt = {"ЕДИНИЦЫ ВЫСОТЫ", alt_items, 2, 0, alt_select};

static const char* ver_items[] = {"ПО БИНС: -","ПО МФИ: 0.5", "ЗАГРУЗЧИК БИНС: -", "ЗАГРУЗЧИК МФИ: -"};
static gui_menu_t menu_ver = {"ВЕРСИЯ", ver_items, 4, 0, ver_select};

#if USE_MTI_ICC
static const char* icc_items[] = {"вкл","выкл"};
static gui_menu_t menu_icc = {"КАЛИБРОВКА КОМПАСА", icc_items, 2, 0, icc_select};

static const char* bad_icc_items[] = {"ОК"};
static gui_menu_t menu_bad_icc = {"НЕДОСТАТОЧНО ДАННЫХ\rДЛЯ КАЛИБРОВКИ", bad_icc_items, 1, 0, bad_icc_select};

static const char* good_icc_items[] = {"СОХРАНИТЬ","ОТМЕНА"};
static gui_menu_t menu_good_icc = {nullptr, good_icc_items, 2, 0, good_icc_select};
#endif

static gui_state_t gui_state = {0.0f, };
static RotaryEncoder encoder(enc_brg);

static float pres_dyn_offs = 0.0f;

//------------------------------------------------------------------------------
void app_run()
{
#if 0
	uint32_t refresh_cntr = 0;
	uint64_t prev_time = 0;
#endif

	can_init();
 	gui_init(&gui_state);
  disp_bright_init();
 	input_init();

 	while(1)
 	{
  	gui_refresh(&gui_state);
  	lv_timer_handler();
#if 0
  	refresh_cntr++;
  	uint32_t dt = systime - prev_time;
  	if(dt >= 1000)
  	{
  		prev_time = systime;
  		uint32_t rate = refresh_cntr*10000/dt;
  		refresh_cntr = 0;
  		__disable_irq();
  		debug_print("FPS %d.%01d\r", rate/10, rate%10);
  		__enable_irq();
  	}
#endif
 	}
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
static void can_serve(uint32_t dt)
{
	static uint32_t tim_roll = 0;
	static uint32_t tim_pitch = 0;
	static uint32_t tim_heading = 0;
	static uint32_t tim_pstat = 0;
	static uint32_t tim_pdyn = 0;

	tim_roll += dt;
	tim_pitch += dt;
	tim_heading += dt;
	tim_pstat += dt;
	tim_pdyn += dt;
 	while(HAL_FDCAN_GetRxFifoFillLevel(&hfdcan2, FDCAN_RX_FIFO0))
 	{
 		FDCAN_RxHeaderTypeDef rx_hdr;
 		uint8_t buf[8];
 		if(HAL_FDCAN_GetRxMessage(&hfdcan2, FDCAN_RX_FIFO0, &rx_hdr, buf) != HAL_OK)
 			break;

 		switch(rx_hdr.Identifier)
 		{
 		case CAN_VAL_ROLL:
 		{
 			can_val_t* msg = (can_val_t*)buf;
 			gui_state.valid_roll = msg->state.valid;
 			gui_state.roll = msg->value/180.0f*PI;
 			static uint8_t prev_cntr = 0;
 			if((uint8_t)(msg->cntr - prev_cntr) != 1u)
 				debug_print("roll cntrs: %d %d\r", msg->cntr, prev_cntr);
 			prev_cntr = msg->cntr;
 			tim_roll = 0;
 			break;
 		}
 		case CAN_VAL_PITCH:
 		{
 			can_val_t* msg = (can_val_t*)buf;
 			gui_state.valid_pitch = msg->state.valid;;
 			gui_state.pitch = msg->value/180.0f*PI;
 			static uint8_t prev_cntr = 0;
 			if((uint8_t)(msg->cntr - prev_cntr) != 1u)
 				debug_print("pitch cntrs: %d %d\r", msg->cntr, prev_cntr);
 			prev_cntr = msg->cntr;
 			tim_pitch = 0;
 			break;
 		}
 		case CAN_VAL_HEADING:
 		{
 			can_val_t* msg = (can_val_t*)buf;
 			gui_state.valid_heading = msg->state.valid;;
 			gui_state.heading = msg->value/180.0f*PI;
 			static uint8_t prev_cntr = 0;
 			if((uint8_t)(msg->cntr - prev_cntr) != 1u)
 				debug_print("heading cntrs: %d %d\r", msg->cntr, prev_cntr);
 			prev_cntr = msg->cntr;
 			tim_heading = 0;
 			break;
 		}
 		case CAN_VAL_PRES_STAT:
 		{
 			can_val_t* msg = (can_val_t*)buf;
 			gui_state.valid_altitude = msg->state.valid;
 			gui_state.valid_ver_speed = msg->state.valid;
 			static uint8_t prev_cntr = 0;
 			if((uint8_t)(msg->cntr - prev_cntr) != 1u)
 				debug_print("stat pres cntrs: %d %d\r", msg->cntr, prev_cntr);
 			tim_pstat = 0;
 			if(!msg->state.valid)
 				break;
 			float pres = msg->value;
 			float ref = GUI_PRES_QNE;
 			if(gui_state.pres_type == GUI_QFE)
 				ref = gui_state.pres_qfe;
 			else if(gui_state.pres_type == GUI_QNH)
 			  ref = gui_state.pres_qnh;
 			gui_state.altitude = altitude(pres, ref);
 			static float prev_pres = ref;
 			if((uint8_t)(msg->cntr - prev_cntr) == 1u)
 				gui_state.vert_speed = vert_speed(pres, ref, (pres-prev_pres)*10.0f); // 10Hz msg rate
 		  prev_cntr = msg->cntr;
 		  prev_pres = pres;
 		  break;
 		}
 		case CAN_VAL_PRES_DYN:
 		{
 			can_val_t* msg = (can_val_t*)buf;
 			gui_state.valid_speed = msg->state.valid;
 			gui_state.speed = air_speed(msg->value - pres_dyn_offs);
 			static uint8_t prev_cntr = 0;
 			if((uint8_t)(msg->cntr - prev_cntr) != 1u)
 				debug_print("dyn pres cntrs: %d %d\r", msg->cntr, prev_cntr);
 			prev_cntr = msg->cntr;
 			tim_pdyn = 0;
 			break;
 		}
 		case CAN_MTI_STATUS:
 		{
 		  can_msg_mti_stat_t* status = (can_msg_mti_stat_t*)buf;
 			gui_state.icc_active = (*status & (1<<5)) ? true : false;
 			break;
 		}
#if USE_MTI_ICC
 		case CAN_MSG_RESULT:
 		{
 			gui_state.icc_active = false;
 			can_icc_result_t* result = (can_icc_result_t*)buf;
 			if(result->err)
 				gui_state.menu = &menu_bad_icc;
 			else
 			{
 				static char header [128];
 				sprintf(header,"РЕЗУЛЬТАТ КАЛИБРОВКИ\r\rНОРМА: %d.%d\rРЕЖИМ: %dD",
 						    (int)result->ddt, ((int)(result->ddt*100.0f))%100, result->dim);
 				menu_good_icc.header = header;
 				gui_state.menu = &menu_good_icc;
 			}
 			gui_state.mode = GUI_MENU;
 			encoder.change_listener(enc_menu_timeless);
 			break;
 		}
#endif
 		default:
 			break;
 		};
 	}

 	if(tim_roll > 100)
 		gui_state.valid_roll = false;
 	if(tim_pitch > 100)
 	  gui_state.valid_pitch = false;
 	if(tim_heading > 100)
 	  gui_state.valid_heading = false;
 	if(tim_pdyn > 250)
 	  gui_state.valid_speed = false;
 	if(tim_pstat > 250)
 	{
 	  gui_state.valid_altitude = false;
 	  gui_state.valid_ver_speed = false;
 	}
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
static void can_serve_init()
{
	static uint32_t cntr = 0;

 	while(HAL_FDCAN_GetRxFifoFillLevel(&hfdcan2, FDCAN_RX_FIFO0))
 	{
 		FDCAN_RxHeaderTypeDef rx_hdr;
 		uint8_t buf[8];
 		if(HAL_FDCAN_GetRxMessage(&hfdcan2, FDCAN_RX_FIFO0, &rx_hdr, buf) != HAL_OK)
 			break;

 		if(rx_hdr.Identifier == CAN_VAL_PRES_DYN)
 		{
 			can_val_t* msg = (can_val_t*)buf;
 			if(msg->state.valid)
 			pres_dyn_offs += msg->value;
 			if(++cntr == 20)
 			{
 				pres_dyn_offs /= cntr;
 				gui_state.mode = GUI_TEST;
 			}
 		}

 	}

}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void app_on_timer()
{
	static uint32_t systime = 0;

	if (gui_state.mode == GUI_TEST)
	{
		static uint32_t test_timeot = 1000;
		if(test_timeot == 0)
		{
			gui_state.mode = GUI_MAIN_MODE;
			test_timeot = 1000;
		}
		else
			test_timeot--;
	}

	if (gui_state.mode == GUI_INIT)
		can_serve_init();
	else
		can_serve(HAL_TICK_FREQ_DEFAULT);

	lv_tick_inc(HAL_TICK_FREQ_DEFAULT);
	encoder.serve_input(input_get_enc(), input_get_btn());
  rs422_serve();
  rs422_test_rx2tx();

  static uint8_t sync_cntr = 0;
  if(++sync_cntr == 20)
  {
  	sync_cntr = 0;
  	can_send_dat(CAN_HEARTBEAT(1), nullptr, 0);
  }

  systime++;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void brg_rotate(int8_t delta)
{
	int brg = disp_bright_get();
	brg += (1+brg/10)*delta;
	if(brg < 0)
		brg = 0;
	else if(brg > 255)
		brg = 255;
	disp_bright_set((uint8_t)brg);
	debug_print("brightness %d\r", brg);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void brg_click()
{
	gui_state.mode = GUI_SET_PRES_REF;
	encoder.change_listener(enc_ref);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void brg_hold()
{
	gui_state.menu = &menu_main;
	gui_state.mode = GUI_MENU;
	encoder.change_listener(enc_menu);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void ref_rotate(int8_t delta)
{
	int type_nr = (int)gui_state.pres_type;
	if(delta>0)
		gui_state.pres_type = (gui_type_pres_t)((type_nr < 2) ? ++type_nr : 0);
	else if(delta<0)
		gui_state.pres_type = (gui_type_pres_t)((type_nr > 0) ? --type_nr : 2);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void ref_click()
{
	if(gui_state.pres_type == GUI_QNE)
	{
		gui_state.mode = GUI_MAIN_MODE;
		encoder.change_listener(enc_brg);
	}
	else
	{
		gui_state.mode = GUI_SET_PRES_VAL;
		encoder.change_listener(enc_pres);
	}
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void pres_rotate(int8_t delta)
{
	if(gui_state.pres_type == GUI_QFE)
		gui_state.pres_qfe += delta*10;
	else if(gui_state.pres_type == GUI_QNH)
		gui_state.pres_qnh += delta*10;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void pres_click()
{
	gui_state.mode = GUI_MAIN_MODE;
	encoder.change_listener(enc_brg);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void menu_rotate(int8_t delta)
{
	int item_nr = (int)gui_state.menu->curr_item;
	if(delta>0)
		gui_state.menu->curr_item = ((item_nr < (gui_state.menu->items_num-1)) ? ++item_nr : 0);
	else if(delta<0)
		gui_state.menu->curr_item = ((item_nr > 0) ? --item_nr : (gui_state.menu->items_num-1));
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void menu_click()
{
	gui_state.menu->on_select(gui_state.menu->curr_item);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void menu_hold()
{
	gui_state.mode = GUI_MAIN_MODE;
	encoder.change_listener(enc_brg);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void menu_timeout()
{
	gui_state.mode = GUI_MAIN_MODE;
	encoder.change_listener(enc_brg);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void menu_select(uint8_t item)
{
	switch(item)
	{
	case 0:
		gui_state.menu = &menu_pres;
		break;
	case 1:
		gui_state.menu = &menu_alt;
		break;
#if USE_MTI_ICC
	case 2:
		gui_state.menu = &menu_icc;
		break;
	case 3:
		gui_state.menu = &menu_ver;
		break;
#else
	case 2:
	  gui_state.menu = &menu_ver;
	  break;
#endif
	}
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void pres_select(uint8_t item)
{
	switch(item)
	{
	case 0:
		gui_state.pres_unit = GUI_MERC;
		break;
	case 1:
		gui_state.pres_unit = GUI_PASC;
		break;
	}
	gui_state.mode = GUI_MAIN_MODE;
	encoder.change_listener(enc_brg);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void alt_select(uint8_t item)
{
	switch(item)
	{
	case 0:
		gui_state.alt_unit = GUI_METER;
		break;
	case 1:
		gui_state.alt_unit = GUI_FOOT;
		break;
	}
	gui_state.mode = GUI_MAIN_MODE;
	encoder.change_listener(enc_brg);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void ver_select(uint8_t item)
{
	gui_state.mode = GUI_MAIN_MODE;
	encoder.change_listener(enc_brg);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
#if USE_MTI_ICC
void icc_select(uint8_t item)
{
	can_icc_cmd_t cmd;
	switch(item)
	{
	case 0:
		cmd = 0x0;
		break;
	case 1:
		cmd = 0x1;
		break;
	}
	can_send_dat(CAN_CMD_ICC, &cmd, sizeof(cmd));
	gui_state.mode = GUI_MAIN_MODE;
	encoder.change_listener(enc_brg);
}
#endif
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
#if USE_MTI_ICC
void bad_icc_select(uint8_t item)
{
	gui_state.mode = GUI_MAIN_MODE;
	encoder.change_listener(enc_brg);
}
#endif
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
#if USE_MTI_ICC
void good_icc_select(uint8_t item)
{
	if(item == 0)
	{
		can_icc_cmd_t cmd = CAN_CMD_ICC_SRORE;
		can_send_dat(CAN_CMD_ICC, &cmd, sizeof(cmd));
	}
	gui_state.mode = GUI_MAIN_MODE;
	encoder.change_listener(enc_brg);
}
#endif
//------------------------------------------------------------------------------


