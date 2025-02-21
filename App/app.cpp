#include <app.h>
#include <air_signals.h>
#include <input_hal.h>
#include <disp_bright_hal.h>
#include <RotaryEncoder.h>
#include <rs422_port.h>
#include <can_port.h>
#include <math.h>
#include <gui.h>
#include <cstdio>
#include <array>
#include <build_time.h>


#if APP_DEBUG
	#define debug_print rs422_printf
#else
  #define debug_print(...)
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
static void can_serve();



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
static volatile uint32_t systime = 0;

//------------------------------------------------------------------------------
void app_run()
{
#if APP_DEBUG
	uint32_t refresh_cntr = 0;
	uint32_t prev_time = 0;
#endif

	can_init();
 	gui_init(&gui_state);
  disp_bright_init();
 	input_init();

 	while(1)
 	{
 	  can_serve();
  	gui_refresh(&gui_state);
  	lv_timer_handler();

#if APP_DEBUG
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
static void can_serve()
{
  static array<uint32_t, CAN_TOTAL_BUFNR> rx_time {}; // initialized by zeros
  static array<float, CAN_TOTAL_BUFNR> rx_val {};
  enum
  {
    ATTITUDE_TIMEOUT = 100,
    AIR_TIMEOUT = 500
  };

  for(int i=0; i<CAN_TOTAL_BUFNR; i++)
  {
    FDCAN_RxHeaderTypeDef rx_hdr;
    uint8_t buf[8];
    if(HAL_FDCAN_IsRxBufferMessageAvailable(&can, i) && (HAL_FDCAN_GetRxMessage(&can, i, &rx_hdr, buf) == HAL_OK))
    {
      can_val_t* can_val = (can_val_t*)buf;
      if(can_val->state.bins_ok && can_val->state.valid)
      {
        rx_time[i] = systime;
        rx_val[i] = can_val->value;
      }
    }
  }


  // roll
  if(((systime-rx_time[CAN_BUFNR_ROLL1]) < ATTITUDE_TIMEOUT) || ((systime-rx_time[CAN_BUFNR_ROLL2]) < ATTITUDE_TIMEOUT))
  {
    gui_state.valid_roll = true;
    gui_state.roll = rx_val[((systime-rx_time[CAN_BUFNR_ROLL1]) < ATTITUDE_TIMEOUT) ? CAN_BUFNR_ROLL1 : CAN_BUFNR_ROLL2]/180.0f*PI;
  }
  else
    gui_state.valid_roll = false;

  // pitch
  if(((systime-rx_time[CAN_BUFNR_PITCH1]) < ATTITUDE_TIMEOUT) || ((systime-rx_time[CAN_BUFNR_PITCH2]) < ATTITUDE_TIMEOUT))
  {
    gui_state.valid_pitch = true;
    gui_state.pitch = rx_val[((systime-rx_time[CAN_BUFNR_PITCH1]) < ATTITUDE_TIMEOUT) ? CAN_BUFNR_PITCH1 : CAN_BUFNR_PITCH2]/180.0f*PI;
  }
  else
    gui_state.valid_pitch = false;

  // heading
  if(((systime-rx_time[CAN_BUFNR_HEADING1]) < ATTITUDE_TIMEOUT) || ((systime-rx_time[CAN_BUFNR_HEADING2]) < ATTITUDE_TIMEOUT))
  {
    gui_state.valid_heading = true;
    gui_state.heading = rx_val[((systime-rx_time[CAN_BUFNR_HEADING1]) < ATTITUDE_TIMEOUT) ? CAN_BUFNR_HEADING1 : CAN_BUFNR_HEADING2]/180.0f*PI;
  }
  else
    gui_state.valid_heading = false;

  // altitude
  if(((systime-rx_time[CAN_BUFNR_PRES1]) < AIR_TIMEOUT) || ((systime-rx_time[CAN_BUFNR_PRES2]) < AIR_TIMEOUT))
  {
    gui_state.valid_altitude = true;
    float pres = rx_val[((systime-rx_time[CAN_BUFNR_PRES1]) < AIR_TIMEOUT) ? CAN_BUFNR_PRES1 : CAN_BUFNR_PRES2];
    float ref = GUI_PRES_QNE;
    if(gui_state.pres_type == GUI_QFE)
      ref = gui_state.pres_qfe;
    else if(gui_state.pres_type == GUI_QNH)
      ref = gui_state.pres_qnh;
    gui_state.altitude = altitude(pres, ref);
  }
  else
    gui_state.valid_altitude = false;

  // airspeed
  if(((systime-rx_time[CAN_BUFNR_AIRSPEED1]) < AIR_TIMEOUT) || ((systime-rx_time[CAN_BUFNR_AIRSPEED2]) < AIR_TIMEOUT))
  {
    gui_state.valid_speed = true;
    gui_state.speed = rx_val[((systime-rx_time[CAN_BUFNR_AIRSPEED1]) < AIR_TIMEOUT) ? CAN_BUFNR_AIRSPEED1 : CAN_BUFNR_AIRSPEED2]*3.6f;
  }
  else
    gui_state.valid_speed = false;

  // vertspeed
  if(((systime-rx_time[CAN_BUFNR_VERTSPEED1]) < AIR_TIMEOUT) || ((systime-rx_time[CAN_BUFNR_VERTSPEED2]) < AIR_TIMEOUT))
  {
    gui_state.valid_ver_speed = true;
    gui_state.vert_speed = rx_val[((systime-rx_time[CAN_BUFNR_VERTSPEED1]) < AIR_TIMEOUT) ? CAN_BUFNR_VERTSPEED1 : CAN_BUFNR_VERTSPEED2];
  }
  else
    gui_state.valid_ver_speed = false;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void app_on_timer()
{
  systime = systime + 1;

  if (gui_state.mode == GUI_INIT)
  {
    if (systime == 500)
      gui_state.mode = GUI_TEST;
  }
  else if (gui_state.mode == GUI_TEST)
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

  rs422_serve();
	lv_tick_inc(HAL_TICK_FREQ_DEFAULT);
	encoder.serve_input(input_get_enc(), input_get_btn());

#if 1
  static uint32_t cntr = 0;
  if(++cntr == 1000)
  {
  	cntr = 0;
  	static const can_msg_version_t version = {.timestamp=BuildTime, .maj_ver=MajVersion, .min_ver=MinVersion, .build=Build};
  	can_send_dat(CAN_VERSION_MFI, &version, sizeof(version));
  }
#endif
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


