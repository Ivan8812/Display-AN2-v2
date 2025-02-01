#ifndef GUI_H_
#define GUI_H_

#ifdef __cplusplus
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wvolatile"
#pragma GCC diagnostic ignored "-Wdeprecated-enum-enum-conversion"
#include "lvgl.h"
#pragma GCC diagnostic pop
extern "C" {
#else
#include "lvgl.h"
#endif

typedef enum{GUI_INIT, GUI_TEST, GUI_MAIN_MODE, GUI_SET_PRES_REF,
	           GUI_SET_PRES_VAL, GUI_MENU} __attribute__ ((__packed__)) gui_mode_t;
typedef enum{GUI_QFE, GUI_QNH, GUI_QNE} __attribute__ ((__packed__))  gui_type_pres_t;
typedef enum{GUI_METER, GUI_FOOT} __attribute__ ((__packed__)) gui_unit_alt_t;
typedef enum{GUI_PASC, GUI_MERC} __attribute__ ((__packed__)) gui_unit_pres_t;

enum{GUI_INVALID, GUI_VALID};

typedef struct
{
	const char* header;
	const char** items;
	uint8_t items_num;
	uint8_t curr_item;
	void (*on_select)(uint8_t);
} gui_menu_t;

#define GUI_PRES_QNE	101325.0f

typedef struct
{
	float speed;
	float vert_speed;
	float altitude;
	float pitch;
	float roll;
	float heading;
	float pres_qfe;
	float pres_qnh;

	uint32_t valid_speed : 1;
	uint32_t valid_ver_speed : 1;
	uint32_t valid_altitude : 1;
	uint32_t valid_pitch : 1;
	uint32_t valid_roll : 1;
	uint32_t valid_heading : 1;
	uint32_t valid_pressure : 1;
	uint32_t icc_active:1;

	gui_menu_t* menu;
	gui_mode_t mode;
	gui_type_pres_t pres_type;
	gui_unit_pres_t pres_unit;
	gui_unit_alt_t alt_unit;
} gui_state_t;


void gui_init(gui_state_t* state);

void gui_refresh(const gui_state_t* state);


#ifdef __cplusplus
} // extern "C"
#endif

#endif /* GUI_H_ */
