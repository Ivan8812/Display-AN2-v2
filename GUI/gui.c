#include "gui.h"
#include "main.h"
#include "stdio.h"
#include "stdlib.h"
#include "lv_port.h"

#define PI      3.141592653f

#define MAX_X   DISP_HOR_RES
#define MAX_Y   DISP_VER_RES

#define VERT_OFFS     -20

#define ROLL_SCALE_R  85

#define SIDE_PANEL_W  50

#define PANEL_UNIT_H  20

#define PANEL_PRES_W  70
#define PANEL_PRES_H  25
#define PANEL_PRES_X  30
#define PANEL_PRES_Y  2

#define PANEL_REF_W   44
#define PANEL_REF_H   25
#define PANEL_REF_X   (MAX_X - 95)
#define PANEL_REF_Y   4

#define PANEL_VSPD_W  50
#define PANEL_VSPD_H  24
#define PANEL_VSPD_X  126
#define PANEL_VSPD_Y  78

#define PANEL_SPD_W   50
#define PANEL_SPD_H   52
#define PANEL_SPD_X   -128
#define PANEL_SPD_Y   VERT_OFFS

#define PANEL_ALT_W   56
#define PANEL_ALT_H   40
#define PANEL_ALT_X   128
#define PANEL_ALT_Y   VERT_OFFS

#define PANEL_HDNG_W  40
#define PANEL_HDNG_H  20

#define PLANE_W       ((2*ROLL_SCALE_R)-10)
#define PLANE_H       20
#define PLANE_Y       VERT_OFFS

#define SCALE_HDNG_W  220
#define SCALE_HDNG_H  24

#define PANEL_MSG_W   200
#define PANEL_MSG_H   30
#define PANEL_MSG_X   0
#define PANEL_MSG_Y   -50

#define FOREGROUND_W  200
#define FOREGROUND_H  200
#define FOREGROUND_Y  VERT_OFFS

#define COLOR_BLACK   lv_color_hex(0x000000)
#define COLOR_RED     lv_color_hex(0xFF0000)
#define COLOR_GREEN   lv_color_hex(0x00FF00)
#define COLOR_BLUE    lv_color_hex(0x0000FF)
#define COLOR_GREY    lv_color_hex(0x808080)
#define COLOR_WHITE   lv_color_hex(0xFFFFFF)
#define COLOR_BROWN   lv_color_hex(0x964B00)
#define COLOR_YELLOW  lv_color_hex(0xFFFF00)

#define MODF(a,m)     ((a) - (m)*floorf((a)/(m)))

static lv_obj_t* cvs_background;
static lv_obj_t* cvs_foreground;
static lv_obj_t* cvs_hdng_scale;
static lv_obj_t* cvs_hdng_panel;
static lv_obj_t* cvs_msg_box;
static lv_obj_t* cvs_speed_v;
static lv_obj_t* cvs_speed;
static lv_obj_t* cvs_plane;
static lv_obj_t* cvs_pres;
static lv_obj_t* cvs_alt;
static lv_obj_t* cvs_ref;

static lv_draw_rect_dsc_t rect;
static lv_draw_label_dsc_t label;
static lv_draw_line_dsc_t line;

static uint8_t cvs_buf_background[LV_CANVAS_BUF_SIZE_TRUE_COLOR(MAX_X,MAX_Y)] __attribute__((section(".lvgl_ram")));
static uint8_t cvs_buf_foreground[LV_CANVAS_BUF_SIZE_TRUE_COLOR_ALPHA(FOREGROUND_W,FOREGROUND_H)] __attribute__((section(".lvgl_ram")));
static uint8_t cvs_buf_hdng_panel[LV_CANVAS_BUF_SIZE_TRUE_COLOR(PANEL_HDNG_W,PANEL_HDNG_H)] __attribute__((section(".lvgl_ram")));
static uint8_t cvs_buf_hdng_scale[LV_CANVAS_BUF_SIZE_TRUE_COLOR_ALPHA(SCALE_HDNG_W,SCALE_HDNG_H)] __attribute__((section(".lvgl_ram")));
static uint8_t cvs_buf_speed_v[LV_CANVAS_BUF_SIZE_TRUE_COLOR(PANEL_VSPD_W,PANEL_VSPD_H)] __attribute__((section(".lvgl_ram")));
static uint8_t cvs_buf_speed[LV_CANVAS_BUF_SIZE_TRUE_COLOR_ALPHA(PANEL_SPD_W,PANEL_SPD_H)] __attribute__((section(".lvgl_ram")));
static uint8_t cvs_buf_plane[LV_CANVAS_BUF_SIZE_TRUE_COLOR_ALPHA(PLANE_W,PLANE_H)] __attribute__((section(".lvgl_ram")));
static uint8_t cvs_buf_pres[LV_CANVAS_BUF_SIZE_TRUE_COLOR(PANEL_PRES_W,PANEL_PRES_H)] __attribute__((section(".lvgl_ram")));
static uint8_t cvs_buf_alt[LV_CANVAS_BUF_SIZE_TRUE_COLOR_ALPHA(PANEL_ALT_W,PANEL_ALT_H)] __attribute__((section(".lvgl_ram")));
static uint8_t cvs_buf_ref[LV_CANVAS_BUF_SIZE_TRUE_COLOR_ALPHA(PANEL_REF_W,PANEL_REF_H)] __attribute__((section(".lvgl_ram")));
static uint8_t cvs_buf_msg_box[LV_CANVAS_BUF_SIZE_TRUE_COLOR(PANEL_MSG_W,PANEL_MSG_H)] __attribute__((section(".lvgl_ram")));

//------------------------------------------------------------------------------
static void init_cvs_speed_v(void);
static void init_cvs_speed(void);
static void init_cvs_plane(void);
static void init_cvs_alt(void);
static void draw_menu(const gui_menu_t* menu);
static void draw_main_window(const gui_state_t* state);
static void draw_background(float pitch, float roll, uint8_t is_pitch_valid, uint8_t is_roll_valid);
static void draw_foreground(float pitch, float roll, uint8_t is_pitch_valid, uint8_t is_roll_valid);
static void draw_vert_speed(float vert_speed, uint8_t is_vspeed_valid);
static void draw_speed_scale(float speed, uint8_t is_speed_valid);
static void draw_speed_label(float speed, uint8_t is_speed_valid);
static void draw_alt_scale(float altitude, uint8_t is_alt_valid, gui_unit_alt_t unit);
static void draw_alt_label(float altitude, uint8_t is_alt_valid, gui_unit_alt_t unit);
static void draw_heading(float heading, uint8_t is_heading_valid, uint8_t is_icc);
static void draw_pressure(float pressure, gui_mode_t mode, gui_unit_pres_t unit);
static void draw_pres_ref(gui_type_pres_t type, gui_mode_t mode);
static void view_message(const char* message);
static void clear_message();
static uint32_t get_num_lines(const char* message)
{
  uint32_t n = 1;
  const char* p = message;
  while(*p)
    if(*p++ == '\r')
      n++;
  return n;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void gui_init(gui_state_t* state)
{
  lv_init();
  lv_port_disp_init();

  lv_draw_rect_dsc_init(&rect);
  lv_draw_label_dsc_init(&label);
  lv_draw_line_dsc_init(&line);

  cvs_background = lv_canvas_create(lv_scr_act());
  lv_canvas_set_buffer(cvs_background, cvs_buf_background, MAX_X, MAX_Y, LV_IMG_CF_TRUE_COLOR );
  lv_obj_center(cvs_background);

  cvs_foreground = lv_canvas_create(cvs_background);
  lv_canvas_set_buffer(cvs_foreground, cvs_buf_foreground, FOREGROUND_W, FOREGROUND_H, LV_IMG_CF_TRUE_COLOR_ALPHA);
  lv_obj_align(cvs_foreground, LV_ALIGN_CENTER, 0, FOREGROUND_Y);

  cvs_hdng_panel = lv_canvas_create(lv_scr_act());
  lv_canvas_set_buffer(cvs_hdng_panel, cvs_buf_hdng_panel, PANEL_HDNG_W, PANEL_HDNG_H, LV_IMG_CF_TRUE_COLOR);
  lv_obj_align(cvs_hdng_panel, LV_ALIGN_BOTTOM_MID, 0, -SCALE_HDNG_H-2);

  cvs_hdng_scale = lv_canvas_create(lv_scr_act());
  lv_canvas_set_buffer(cvs_hdng_scale, cvs_buf_hdng_scale, SCALE_HDNG_W, SCALE_HDNG_H, LV_IMG_CF_TRUE_COLOR_ALPHA);
  lv_obj_align(cvs_hdng_scale, LV_ALIGN_BOTTOM_MID, 0, 0);

  cvs_speed_v = lv_canvas_create(lv_scr_act());
  lv_canvas_set_buffer(cvs_speed_v, cvs_buf_speed_v, PANEL_VSPD_W, PANEL_VSPD_H, LV_IMG_CF_TRUE_COLOR);
  lv_obj_align(cvs_speed_v, LV_ALIGN_CENTER, PANEL_VSPD_X, PANEL_VSPD_Y);
  init_cvs_speed_v();

  cvs_speed = lv_canvas_create(lv_scr_act());
  lv_canvas_set_buffer(cvs_speed, cvs_buf_speed, PANEL_SPD_W, PANEL_SPD_H, LV_IMG_CF_TRUE_COLOR_ALPHA);
  lv_obj_align(cvs_speed, LV_ALIGN_CENTER, PANEL_SPD_X, PANEL_SPD_Y);
  init_cvs_speed();

  cvs_plane = lv_canvas_create(lv_scr_act());
  lv_canvas_set_buffer(cvs_plane, cvs_buf_plane, PLANE_W, PLANE_H, LV_IMG_CF_TRUE_COLOR_ALPHA);
  lv_obj_align(cvs_plane, LV_ALIGN_CENTER, 0, PLANE_Y);
  init_cvs_plane();

  cvs_pres = lv_canvas_create(lv_scr_act());
  lv_canvas_set_buffer(cvs_pres, cvs_buf_pres, PANEL_PRES_W, PANEL_PRES_H, LV_IMG_CF_TRUE_COLOR);
  lv_obj_align(cvs_pres, LV_ALIGN_TOP_LEFT, PANEL_PRES_X, PANEL_PRES_Y);

  cvs_alt = lv_canvas_create(lv_scr_act());
  lv_canvas_set_buffer(cvs_alt, cvs_buf_alt, PANEL_ALT_W, PANEL_ALT_H, LV_IMG_CF_TRUE_COLOR_ALPHA);
  lv_obj_align(cvs_alt, LV_ALIGN_CENTER, PANEL_ALT_X, PANEL_ALT_Y);
  init_cvs_alt();

  cvs_ref = lv_canvas_create(lv_scr_act());
  lv_canvas_set_buffer(cvs_ref, cvs_buf_ref, PANEL_REF_W, PANEL_REF_H, LV_IMG_CF_TRUE_COLOR_ALPHA);
  lv_obj_align(cvs_ref, LV_ALIGN_TOP_LEFT, PANEL_REF_X, PANEL_REF_Y);

  cvs_msg_box = lv_canvas_create(lv_scr_act());
  lv_canvas_set_buffer(cvs_msg_box, cvs_buf_msg_box, PANEL_MSG_W, PANEL_MSG_H, LV_IMG_CF_TRUE_COLOR );
  lv_obj_align(cvs_msg_box, LV_ALIGN_CENTER, PANEL_MSG_X, PANEL_MSG_Y);

  memset(state, 0x0, sizeof(gui_state_t));
  state->pres_qfe = GUI_PRES_QNE;
  state->pres_qnh = GUI_PRES_QNE;
  gui_refresh(state);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void gui_refresh(const gui_state_t* state)
{
  gui_state_t tmp_state = *state;
  clear_message();

  if(state->mode != GUI_MENU)
  {
    if(state->mode == GUI_INIT)
    {
      tmp_state.valid_speed = 0;
      tmp_state.valid_ver_speed = 0;
      tmp_state.valid_altitude = 0;
      tmp_state.valid_pitch = 0;
      tmp_state.valid_roll = 0;
      tmp_state.valid_heading = 0;
      tmp_state.valid_pressure = 0;
      view_message("ИНИЦИАЛИЗАЦИЯ");
    }
    if(state->mode == GUI_TEST)
    {
      tmp_state.valid_speed = 1;
      tmp_state.valid_ver_speed = 1;
      tmp_state.valid_altitude = 1;
      tmp_state.valid_pitch = 1;
      tmp_state.valid_roll = 1;
      tmp_state.valid_heading = 1;
      tmp_state.valid_pressure = 1;
      tmp_state.speed = 120.0;
      tmp_state.vert_speed = 0;
      tmp_state.altitude = 5200;
      tmp_state.pitch = 0.0;
      tmp_state.roll = 0.0;
      tmp_state.heading = 90.0;
      tmp_state.pres_qfe = 101320.0;
      tmp_state.pres_qnh = 101320.0;
      view_message("ИНИЦИАЛИЗАЦИЯ");
    }
    draw_main_window(&tmp_state);
  }
  else
    draw_menu(tmp_state.menu);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void draw_menu(const gui_menu_t* menu)
{
  lv_obj_add_flag(cvs_foreground, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_flag(cvs_hdng_panel, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_flag(cvs_hdng_scale, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_flag(cvs_plane, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_flag(cvs_speed_v, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_flag(cvs_speed, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_flag(cvs_alt, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_flag(cvs_pres, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_flag(cvs_ref, LV_OBJ_FLAG_HIDDEN);

  lv_canvas_fill_bg(cvs_background, COLOR_BLACK, LV_OPA_COVER);

  const uint16_t TextH = 18;
  uint16_t pos = 10;
  label.font = &lv_font_sans18;
  if(menu->header)
  {
    label.line_space = 2;
    label.color = COLOR_GREEN;
    label.align = LV_TEXT_ALIGN_CENTER;
    lv_canvas_draw_text(cvs_background, 0, pos, MAX_X , &label, menu->header);
    pos += ((TextH+2)*get_num_lines(menu->header) + 20);
  }

  label.line_space = 0;
  label.color = COLOR_WHITE;
  label.align = LV_TEXT_ALIGN_LEFT;
  for(uint8_t i=0; i<menu->items_num; i++)
  {
    if(i == menu->curr_item)
    {
      rect.bg_color = COLOR_GREY;
      rect.bg_opa = LV_OPA_COVER;
      lv_canvas_draw_rect(cvs_background, 0, pos, MAX_X, TextH+12, &rect);
    }
    lv_canvas_draw_text(cvs_background, 20, pos+7, MAX_X-20 , &label, menu->items[i]);
    pos += (TextH + 15);
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void draw_main_window(const gui_state_t* state)
{
  lv_obj_clear_flag(cvs_foreground, LV_OBJ_FLAG_HIDDEN);
  lv_obj_clear_flag(cvs_hdng_panel, LV_OBJ_FLAG_HIDDEN);
  lv_obj_clear_flag(cvs_hdng_scale, LV_OBJ_FLAG_HIDDEN);
  lv_obj_clear_flag(cvs_plane, LV_OBJ_FLAG_HIDDEN);
  lv_obj_clear_flag(cvs_speed_v, LV_OBJ_FLAG_HIDDEN);
  lv_obj_clear_flag(cvs_speed, LV_OBJ_FLAG_HIDDEN);
  lv_obj_clear_flag(cvs_alt, LV_OBJ_FLAG_HIDDEN);
  lv_obj_clear_flag(cvs_pres, LV_OBJ_FLAG_HIDDEN);
  lv_obj_clear_flag(cvs_ref, LV_OBJ_FLAG_HIDDEN);

  draw_foreground(state->pitch, state->roll, state->valid_pitch, state->valid_roll);
  draw_background(state->pitch, state->roll, state->valid_pitch, state->valid_roll);
  draw_alt_scale(state->altitude, state->valid_altitude, state->alt_unit);
  draw_alt_label(state->altitude, state->valid_altitude, state->alt_unit);
  draw_vert_speed(state->vert_speed, state->valid_ver_speed);
  draw_speed_scale(state->speed, state->valid_speed);
  draw_speed_label(state->speed, state->valid_speed);
  float pres = GUI_PRES_QNE;
  if(state->pres_type == GUI_QFE)
    pres = state->pres_qfe;
  else if(state->pres_type == GUI_QNH)
    pres = state->pres_qnh;
  draw_pressure(pres, state->mode, state->pres_unit);
  draw_pres_ref(state->pres_type, state->mode);
  draw_heading(state->heading, state->valid_heading, state->icc_active);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void init_cvs_speed_v(void)
{
  lv_canvas_fill_bg(cvs_speed_v, COLOR_BLACK, LV_OPA_COVER);
  const lv_point_t border[] = {{0,0}, {PANEL_VSPD_W-1,0}, {PANEL_VSPD_W-1,PANEL_VSPD_H-1}, {0,PANEL_VSPD_H-1}, {0,0}};
  line.color = COLOR_WHITE;
  line.width = 1;
  lv_canvas_draw_line(cvs_speed_v, border, 5, &line);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void init_cvs_speed(void)
{
  lv_canvas_fill_bg(cvs_speed, COLOR_BLACK, LV_OPA_TRANSP);

  const uint16_t Y0 = PANEL_SPD_H/2;
  const lv_point_t triang[] = {{0,Y0}, {7,Y0-5}, {7,Y0+5}, {0,Y0}};
  rect.bg_color = COLOR_BLACK;
  rect.bg_opa = LV_OPA_COVER;
  lv_canvas_draw_polygon(cvs_speed, triang, 4, &rect);
  lv_canvas_draw_rect(cvs_speed, 7, Y0-12, 26, 24, &rect);
  lv_canvas_draw_rect(cvs_speed, 33, 0, 17, 52, &rect);

  const lv_point_t border[] = {{0,Y0}, {7,Y0-5}, {7,Y0-12}, {32,Y0-12}, {32,Y0-26}, {49,Y0-26},
                               {49,Y0+25}, {32,Y0+25}, {32,Y0+11}, {7,Y0+11}, {7,Y0+4}, {0,Y0}};

  line.color = COLOR_WHITE;
  line.width = 1;
  lv_canvas_draw_line(cvs_speed, border, sizeof(border)/sizeof(*border), &line);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void init_cvs_plane(void)
{
  lv_canvas_fill_bg(cvs_plane, COLOR_BLACK, LV_OPA_TRANSP);

  const int X0 = PLANE_W/2;
  const int Y0 = PLANE_H/2;
  const lv_point_t triang_left[]  = {{X0-ROLL_SCALE_R+7,Y0}, {X0-ROLL_SCALE_R+13,Y0-3}, {X0-ROLL_SCALE_R+13,Y0+3}, {X0-ROLL_SCALE_R+7,Y0}};
  const lv_point_t triang_right[] = {{X0+ROLL_SCALE_R-7,Y0}, {X0+ROLL_SCALE_R-13,Y0-3}, {X0+ROLL_SCALE_R-13,Y0+3}, {X0+ROLL_SCALE_R-7,Y0}};
  rect.bg_color = COLOR_YELLOW;
  rect.bg_opa = LV_OPA_COVER;
  lv_canvas_draw_polygon(cvs_plane, triang_left, 4, &rect);
  lv_canvas_draw_polygon(cvs_plane, triang_right, 4, &rect);

  const lv_point_t line_left[]   = {{X0-60,Y0}, {X0-20,Y0}, {X0-20,Y0+10}};
  const lv_point_t line_right[]  = {{X0+60,Y0}, {X0+20,Y0}, {X0+20,Y0+10}};
  const lv_point_t line_middle[] = {{X0-10,Y0}, {X0+10,Y0}};
  line.color = COLOR_YELLOW;
  line.width = 3;
  lv_canvas_draw_line(cvs_plane, line_left, 3, &line);
  lv_canvas_draw_line(cvs_plane, line_right, 3, &line);
  lv_canvas_draw_line(cvs_plane, line_middle, 2, &line);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void init_cvs_alt(void)
{
  lv_canvas_fill_bg(cvs_alt, COLOR_BLACK, LV_OPA_TRANSP);

  const uint16_t Y0 = PANEL_ALT_H/2;
  const uint16_t X1 = PANEL_ALT_W-1;
  const lv_point_t triang[] = {{X1,Y0}, {X1-7,Y0-5}, {X1-7,Y0+5}, {X1,Y0}};
  rect.bg_color = COLOR_BLACK;
  rect.bg_opa = LV_OPA_COVER;
  lv_canvas_draw_polygon(cvs_alt, triang, 4, &rect);
  lv_canvas_draw_rect(cvs_alt, 0, Y0-12, 27, 24, &rect);
  lv_canvas_draw_rect(cvs_alt, 27, 0, 22, 40, &rect);

  const lv_point_t border[] = {{0,Y0-12}, {27,Y0-12}, {27,Y0-20}, {49,Y0-20}, {49,Y0-5}, {55,Y0},
                               {49,Y0+4}, {49,Y0+19}, {27,Y0+19}, {27,Y0+11}, {0,Y0+11}, {0,Y0-12}};

  line.color = COLOR_WHITE;
  line.width = 1;
  lv_canvas_draw_line(cvs_alt, border, sizeof(border)/sizeof(*border), &line);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void draw_foreground(float pitch, float roll, uint8_t is_pitch_valid, uint8_t is_roll_valid)
{
  const uint16_t X0 = FOREGROUND_W/2;
  const uint16_t Y0 = FOREGROUND_H/2;
  const uint16_t PitchW = 32;
  const uint16_t PitchH = 140;
  const float R = (float)ROLL_SCALE_R;

  label.color = COLOR_WHITE;
  label.align = LV_TEXT_ALIGN_CENTER;
  line.color = COLOR_WHITE;

  lv_canvas_fill_bg(cvs_foreground, COLOR_BLACK, LV_OPA_TRANSP);

  if(is_pitch_valid)
  {
    const int LineStep = 30;
    const float pitch_deg = is_pitch_valid ? MODF(pitch*180.0f/PI + 90.0f, 180.0f) - 90.0f : 0.0f;
    int dy = (int)(MODF(pitch_deg,10.0f)/10.0f*(float)LineStep);
    for(int i=-3; i<=3; i++)
    {
      char text[6];
      int8_t deg_lbl = (int)floorf(pitch_deg/10.0f) + i;
      if(deg_lbl<0)
        deg_lbl=-deg_lbl;

      if(deg_lbl)
      {
        label.font = &lv_font_sans16;
        sprintf(text,"%1d0",deg_lbl);
        lv_canvas_draw_text(cvs_foreground, X0-PitchW/2, Y0+dy-i*LineStep-7, PitchW , &label, text);

        line.width = 2;
        lv_point_t line_left[] = {{X0-32,Y0+dy-i*LineStep}, {X0-15,Y0+dy-i*LineStep}};
        lv_canvas_draw_line(cvs_foreground, line_left, 2, &line);
        lv_point_t line_right[] = {{X0+32,Y0+dy-i*LineStep}, {X0+15,Y0+dy-i*LineStep}};
        lv_canvas_draw_line(cvs_foreground, line_right, 2, &line);
      }
      else
      {
        line.width = 2;
        lv_point_t line_up[] = {{X0-10,Y0+dy-i*LineStep-8}, {X0+10,Y0+dy-i*LineStep-8}};
        lv_canvas_draw_line(cvs_foreground, line_up, 2, &line);
        lv_point_t line_down[] = {{X0-10,Y0+dy-i*LineStep+7}, {X0+10,Y0+dy-i*LineStep+7}};
        lv_canvas_draw_line(cvs_foreground, line_down, 2, &line);

        line.width = 1;
        lv_point_t line_middle[] = {{X0-15,Y0+dy-i*LineStep}, {X0+15,Y0+dy-i*LineStep}};
        lv_canvas_draw_line(cvs_foreground, line_middle, 2, &line);
      }

      line.width = 1;
      lv_point_t line_tick[] = {{X0-15,Y0+dy-i*LineStep-15}, {X0+15,Y0+dy-i*LineStep-15}};
      lv_canvas_draw_line(cvs_foreground, line_tick, 2, &line);

    }
      // обрезоние шкалы тангажа сверху и снизу
    for(lv_coord_t x=X0-35; x<X0+35; x++)
    {
      for(lv_coord_t y=0; y<Y0-PitchH/2; y++)
        lv_canvas_set_px_opa(cvs_foreground, x, y, LV_OPA_TRANSP);
      for(lv_coord_t y=Y0+PitchH/2; y<FOREGROUND_H; y++)
        lv_canvas_set_px_opa(cvs_foreground, x, y, LV_OPA_TRANSP);
    }
  }
    // рисование мелких засечек
  for(int i=0; i<=180; i+=5)
  {
    if((i>60) && (i<120))
      continue;

    float sc = PI/180.0f;
    line.width = 1;
    lv_point_t line_tick[] = {{X0 + roundf((R-5.0f)*cosf(i*sc)), Y0 + roundf((R-5.0f)*sinf(i*sc))},
                              {X0 + roundf( R*cosf(i*sc)),       Y0 + roundf( R*sinf(i*sc))}};
    lv_canvas_draw_line(cvs_foreground, line_tick, 2, &line);
  }

    // рисование крупных засечек и подписей
  for(int i=0; i<=300; i+=15)
  {
    if((i==90) || (i==270) || ((i>180) && (i<240)))
      continue;

    float sc = PI/180.0f;
    line.width = 2;
    lv_point_t line_tick[] = {{X0 + roundf((R-7.0f)*cosf(i*sc)), Y0 + roundf((R-7.0f)*sinf(i*sc))},
                              {X0 + roundf( R*cosf(i*sc)),       Y0 + roundf( R*sinf(i*sc))}};
    lv_canvas_draw_line(cvs_foreground, line_tick, 2, &line);

    if(((i>0) && (i<60)) || ((i>120) && (i<180)))
    {
      char text[6];
      label.font = &lv_font_sans10;
      sprintf(text,"%d",(i<90 ? i : (180-i)));
      lv_canvas_draw_text(cvs_foreground, X0 + roundf((R+10.0f)*cosf(i*sc)) - 10,
                          Y0 + roundf((R+10.0f)*sinf(i*sc)) - 5, 20, &label, text);
    }
  }

  static const lv_point_t triang_down[] = {{X0,Y0+R}, {X0-3,Y0+R-6}, {X0+3,Y0+R-6}, {X0,Y0+R}};
  static const lv_point_t triang_up[]   = {{X0,Y0-R}, {X0-3,Y0-R+6}, {X0+3,Y0-R+6}, {X0,Y0-R}};
  rect.bg_color = COLOR_WHITE;
  rect.bg_opa = LV_OPA_COVER;
  lv_canvas_draw_polygon(cvs_foreground, triang_down, 4, &rect);
  lv_canvas_draw_polygon(cvs_foreground, triang_up, 4, &rect);

  if(is_roll_valid)
  {
    const float roll_deg = is_roll_valid ? MODF(roll*180.0f/PI+180.0f, 360.0f) - 180.0f : 0.0f;
    lv_img_set_angle(cvs_foreground, (int)(-roll_deg*10.0f));
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void draw_background(float pitch, float roll, uint8_t is_pitch_valid, uint8_t is_roll_valid)
{
  enum {LEFT,TOP,RIGHT,BOTTOM} skyline_sector[2];
  static const lv_point_t vertex[4] = {{0,0}, {MAX_X,0}, {MAX_X,MAX_Y}, {0,MAX_Y}};

    // координаты центра отсчета в системе координат дисплея
  const float X0 = (float)MAX_X/2.0f;
  const float Y0 = (float)MAX_Y/2.0f + (float)VERT_OFFS;

    // масштабный коэф-т смещения линии горизонта при тангаже
  const float PitchScale = 90.0f/(PI/2.0f);

    // приведение углов в допустимый диапазон
  pitch = is_pitch_valid ? MODF(pitch+PI/2.0f, PI) - PI/2.0f : 0.0f;
  roll = is_roll_valid ? MODF(roll+PI, 2.0f*PI) - PI : 0.0f;

    // опорные точки для получения уравнения линии горизонта в системе координат дисплея
  float c = cosf(roll);
  float s =-sinf(roll);
  float x1 = c*100.0f - s*(pitch*PitchScale) + X0;
  float y1 = s*100.0f + c*(pitch*PitchScale) + Y0;
  float x2 =-c*100.0f - s*(pitch*PitchScale) + X0;
  float y2 =-s*100.0f + c*(pitch*PitchScale) + Y0;

    // коэф-ты прямой, проходящей через опорные точки. ур-е прямой: kx*x + ky*y = ko
  float kx = (y1 - y2);
  float ky = (x2 - x1);
  float ko = x2*y1 - x1*y2;

    // линия горизонта на экране
  lv_point_t skyline[2];

  // левая точка линии горизонта
  if(ko/ky < 0.0f)
  {
    skyline[0].x = ko/kx;
    skyline[0].y = 0;
    skyline_sector[0] = TOP;
  }
  else if(ko/ky > (float)MAX_Y)
  {
    skyline[0].x = (ko - ky*MAX_Y)/kx;
    skyline[0].y = MAX_Y;
    skyline_sector[0] = BOTTOM;
  }
  else
  {
    skyline[0].x = 0;
    skyline[0].y = ko/ky;
    skyline_sector[0] = LEFT;
  }

    // правая точка линии горизонта
  if((ko - kx*MAX_X)/ky < 0.0f)
  {
    skyline[1].x = ko/kx;
    skyline[1].y = 0;
    skyline_sector[1] = TOP;
  }
  else if((ko - kx*MAX_X)/ky > (float)MAX_Y)
  {
    skyline[1].x = (ko - ky*MAX_Y)/kx;
    skyline[1].y = MAX_Y;
    skyline_sector[1] = BOTTOM;
  }
  else
  {
    skyline[1].x = MAX_X;
    skyline[1].y = (ko - kx*MAX_X)/ky;
    skyline_sector[1] = RIGHT;
  }

    // точки многоугольника для отображения неба и земли
  lv_point_t polygon[6];
  rect.bg_opa = LV_OPA_COVER;

  polygon[0] = skyline[0];
  int n = 1;
  for(int sect = skyline_sector[0]; sect != skyline_sector[1]; sect = (sect + 1)%4)
    polygon[n++] = vertex[sect];
  polygon[n++] = skyline[1];
  polygon[n++] = skyline[0];
  rect.bg_color = (fabsf(roll) < PI/2.0f) ? COLOR_BLUE : COLOR_BROWN;
  lv_canvas_draw_polygon(cvs_background, polygon, n, &rect);

  polygon[0] = skyline[1];
  n = 1;
  for(int sect = skyline_sector[1]; sect != skyline_sector[0]; sect = (sect + 1)%4)
    polygon[n++] = vertex[sect];
  polygon[n++] = skyline[0];
  polygon[n++] = skyline[1];
  rect.bg_color = (fabsf(roll) < PI/2.0f) ? COLOR_BROWN : COLOR_BLUE;
  lv_canvas_draw_polygon(cvs_background, polygon, n, &rect);

  line.color = COLOR_WHITE;
  line.width = 1;
  lv_canvas_draw_line(cvs_background, skyline, 2, &line);

    // неподвижные указатели крена
  static const lv_point_t triang_down[]  = {{X0,Y0+ROLL_SCALE_R}, {X0-3,Y0+ROLL_SCALE_R+6}, {X0+3,Y0+ROLL_SCALE_R+6}, {X0,Y0+ROLL_SCALE_R}};
  static const lv_point_t triang_up[]    = {{X0,Y0-ROLL_SCALE_R}, {X0-3,Y0-ROLL_SCALE_R-6}, {X0+3,Y0-ROLL_SCALE_R-6}, {X0,Y0-ROLL_SCALE_R}};
  rect.bg_color = COLOR_WHITE;
  lv_canvas_draw_polygon(cvs_background, triang_down, 4, &rect);
  lv_canvas_draw_polygon(cvs_background, triang_up, 4, &rect);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void draw_heading(float heading, uint8_t is_heading_valid, uint8_t is_icc)
{
  line.color = COLOR_WHITE;
  line.width = 2;
  label.color = is_icc ? COLOR_RED : COLOR_WHITE;
  label.align = LV_TEXT_ALIGN_CENTER;

  lv_canvas_fill_bg(cvs_hdng_scale, COLOR_BLACK, LV_OPA_60);
  static const lv_point_t triang[] = {{SCALE_HDNG_W/2-3,2}, {SCALE_HDNG_W/2+3,2}, {SCALE_HDNG_W/2,9}, {SCALE_HDNG_W/2-3,2}};
  rect.bg_color = COLOR_WHITE;
  rect.bg_opa = LV_OPA_COVER;
  lv_canvas_draw_polygon(cvs_hdng_scale, triang, 4, &rect);
  static const lv_point_t frame[] = {{1,SCALE_HDNG_H}, {1,1}, {SCALE_HDNG_W-1,1}, {SCALE_HDNG_W-1,SCALE_HDNG_H}};
  lv_canvas_draw_line(cvs_hdng_scale, frame, 4, &line);

  if(!is_heading_valid)
  {
    lv_obj_add_flag(cvs_hdng_panel, LV_OBJ_FLAG_HIDDEN);
    return;
  }

  heading = MODF(heading, 2.0f*PI);
  float heading_deg = heading*180.0f/PI;
  char text[6];
  sprintf(text,"%d",(int)roundf(heading_deg));
  lv_canvas_fill_bg(cvs_hdng_panel, COLOR_BLACK, LV_OPA_COVER);
  label.font = &lv_font_sans16;
  lv_canvas_draw_text(cvs_hdng_panel, 0, 4, PANEL_HDNG_W, &label, text);

  for(int i=-3; i<=4; i++)
  {
    int x = SCALE_HDNG_W/2 + i*30 - (int)roundf(MODF(heading_deg,10.0f)*3.0f);

    lv_point_t line_maj[] = {{x,0}, {x,8}};
    lv_canvas_draw_line(cvs_hdng_scale, line_maj, 2, &line);

    char text[4];
    int8_t deg_lbl = (int)floorf(heading_deg/10.0f) + i;
    if(deg_lbl < 0)
      deg_lbl += 36;
    else if(deg_lbl >= 36)
      deg_lbl -= 36;
    const char spec_lbl[4] = {'N','E','S','W'};
    if(deg_lbl%3 == 0)
    {
      if(deg_lbl%9 == 0)
        sprintf(text,"%c",spec_lbl[(deg_lbl/9)%4]);
      else
        sprintf(text,"%d",deg_lbl);
      label.font = &lv_font_sans14;
      lv_canvas_draw_text(cvs_hdng_scale, x-10, 10, 20, &label, text);
    }

    lv_point_t line_min[] = {{x-15,0}, {x-15,5}};
    lv_canvas_draw_line(cvs_hdng_scale, line_min, 2, &line);
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void draw_pressure(float pressure, gui_mode_t mode, gui_unit_pres_t unit)
{
  const uint16_t Y0 = PANEL_PRES_H/2;
  const uint16_t TextW = 64;
  const uint16_t TextH = 18;

  uint32_t p;
  char text[12];
  switch(unit)
  {
  case GUI_MERC:
    p = (int)roundf(pressure/133.32f);
    sprintf(text,"Рз %3lu",p);
    break;
  case GUI_PASC:
    p = (int)roundf(pressure/10.0f);
    sprintf(text,"%4lu.%1lu",p/10,p%10);
    break;
  }

  lv_canvas_fill_bg(cvs_pres, COLOR_BLACK, LV_OPA_COVER);
  if(mode==GUI_SET_PRES_VAL)
  {
    const lv_point_t border[] = {{1,1}, {PANEL_PRES_W-1,1}, {PANEL_PRES_W-1,PANEL_PRES_H-1}, {1,PANEL_PRES_H-1}, {1,1}};
    line.color = COLOR_GREEN;
    line.width = 2;
    lv_canvas_draw_line(cvs_pres, border, 5, &line);
  }
  else
  {
    const lv_point_t border[] = {{0,0}, {PANEL_PRES_W-1,0}, {PANEL_PRES_W-1,PANEL_PRES_H-1}, {0,PANEL_PRES_H-1}, {0,0}};
    line.color = COLOR_WHITE;
    line.width = 1;
    lv_canvas_draw_line(cvs_pres, border, 5, &line);
  }

  label.color = COLOR_WHITE;
  label.align = LV_TEXT_ALIGN_CENTER;
  label.font = &lv_font_sans18;
  lv_canvas_draw_text(cvs_pres, (PANEL_PRES_W-TextW)/2, Y0-TextH/2+2, TextW , &label, text);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void draw_pres_ref(gui_type_pres_t type, gui_mode_t mode)
{
  const uint16_t Y0 = PANEL_PRES_H/2;
  const uint16_t TextW = 64;
  const uint16_t TextH = 18;

  lv_canvas_fill_bg(cvs_ref, COLOR_BLACK, LV_OPA_TRANSP);
  if(mode == GUI_SET_PRES_REF)
  {
    const lv_point_t border[] = {{1,1}, {PANEL_REF_W-1,1}, {PANEL_REF_W-1,PANEL_REF_H-1}, {1,PANEL_REF_H-1}, {1,1}};
    line.color = COLOR_GREEN;
    line.width = 2;
    lv_canvas_draw_line(cvs_ref, border, 5, &line);
  }

  label.color = COLOR_WHITE;
  label.align = LV_TEXT_ALIGN_CENTER;
  label.font = &lv_font_sans16;
  switch(type)
  {
  case GUI_QFE:
    lv_canvas_draw_text(cvs_ref, (PANEL_REF_W-TextW)/2, Y0-TextH/2+2, TextW , &label, "QFE");
    break;
  case GUI_QNH:
    lv_canvas_draw_text(cvs_ref, (PANEL_REF_W-TextW)/2, Y0-TextH/2+2, TextW , &label, "QNH");
    break;
  case GUI_QNE:
    lv_canvas_draw_text(cvs_ref, (PANEL_REF_W-TextW)/2, Y0-TextH/2+2, TextW , &label, "QNE");
    break;
  }

}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void draw_alt_scale(float altitude, uint8_t is_alt_valid, gui_unit_alt_t unit)
{
  const uint16_t Y0 = MAX_Y/2 + VERT_OFFS;
  const uint16_t TextW = 36;
  const uint16_t LineW = SIDE_PANEL_W-TextW-2;
  const uint16_t LineStep = 40;

  rect.bg_color = COLOR_BLACK;
  rect.bg_opa = LV_OPA_60;

  if(!is_alt_valid)
  {
    lv_canvas_draw_rect(cvs_background, MAX_X-SIDE_PANEL_W, 0, SIDE_PANEL_W, MAX_Y, &rect);
    return;
  }

  if(unit == GUI_FOOT)
    altitude *= 3.28084f;

  lv_canvas_draw_rect(cvs_background, MAX_X-SIDE_PANEL_W, 0, SIDE_PANEL_W, Y0-25, &rect);
  lv_canvas_draw_rect(cvs_background, MAX_X-SIDE_PANEL_W, Y0+25, SIDE_PANEL_W, MAX_Y-(Y0+25), &rect);
  lv_canvas_draw_rect(cvs_background, MAX_X-LineW, Y0-25, SIDE_PANEL_W, 50, &rect);

  int dy = (int)(MODF(altitude,100.0f)/100.0f*(float)LineStep);
  for(int i=-4; i<=3; i++)
  {
    line.color = COLOR_WHITE;
    line.width = 2;
    lv_point_t line_maj[] = {{MAX_X-LineW,Y0+dy-i*LineStep}, {MAX_X,Y0+dy-i*LineStep}};
    lv_canvas_draw_line(cvs_background, line_maj, 2, &line);
    lv_point_t line_min[] = {{MAX_X-LineW/2,Y0+dy-i*LineStep+LineStep/2}, {MAX_X,Y0+dy-i*LineStep+LineStep/2}};
    lv_canvas_draw_line(cvs_background, line_min, 2, &line);

    char text[6];
    sprintf(text,"%2d00",(int)floorf(altitude/100.0f) + i);
    label.color = COLOR_WHITE;
    label.align = LV_TEXT_ALIGN_RIGHT;
    label.font = &lv_font_sans14;
    lv_canvas_draw_text(cvs_background, MAX_X-TextW-LineW-2, Y0+dy-i*LineStep-7, TextW , &label, text);
  }

  lv_canvas_draw_rect(cvs_background, MAX_X-SIDE_PANEL_W, Y0-25, SIDE_PANEL_W-LineW, 50, &rect);

  label.color = COLOR_WHITE;
  label.align = LV_TEXT_ALIGN_CENTER;
  label.font = &lv_font_sans10;
  switch(unit)
  {
  case GUI_METER:
    lv_canvas_draw_text(cvs_background, MAX_X-SIDE_PANEL_W-4, MAX_Y/2-6, 20 , &label, "М");
    break;
  case GUI_FOOT:
    lv_canvas_draw_text(cvs_background, MAX_X-SIDE_PANEL_W-4, MAX_Y/2-6, 20 , &label, "FT");
  default:
    break;
  };
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void draw_alt_label(float altitude, uint8_t is_alt_valid, gui_unit_alt_t unit)
{
  if(!is_alt_valid)
  {
    lv_obj_add_flag(cvs_alt, LV_OBJ_FLAG_HIDDEN);
    return;
  }

  if(unit == GUI_FOOT)
    altitude *= 3.28084f;

  const int Y0 = PANEL_ALT_H/2;
  const int MajW = 26;
  const int MinW = 20;
  const int MajH = 18;
  const int MinH = 14;

  rect.bg_color = COLOR_BLACK;
  rect.bg_opa = LV_OPA_COVER;
  label.color = COLOR_WHITE;

  char text[20];

  if(altitude < 0.0f)
    sprintf(text,"-%02d",(int)fabsf(truncf(altitude/100.0f)));
  else
    sprintf(text,"%02d",(int)truncf(altitude/100.0f));

  label.align = LV_TEXT_ALIGN_RIGHT;
  label.font = &lv_font_sans16;
  lv_canvas_draw_rect(cvs_alt, 2, Y0-MajH/2+2, MajW, MajH, &rect);
  lv_canvas_draw_text(cvs_alt, 2, Y0-MajH/2+2, MajW, &label, text);

  int step;
  if(unit == GUI_METER)
  {
    if(altitude < 50.0f)
      step = 1;
    else if(altitude < 100.0f)
      step = 2;
    else if(altitude < 1000.0f)
      step = 5;
    else
      step = 10;
  }
  else
  {
    if(altitude < 160.0f)
      step = 1;
    else if(altitude < 320.0f)
      step = 5;
    else if(altitude < 3200.0f)
      step = 15;
    else
      step = 30;
  }
  const float Scale = (float)(MinH+1);
  const int Tck = (((int)truncf(altitude))/step)*step;
  const int dY = (int)truncf(Scale*fmodf(fmodf(altitude, 100.0f), (float)step)/((float)step));

  sprintf(text,"%02d\n%02d\n%02d\n%02d\n%02d", abs((Tck+2*step)%100), abs((Tck+step)%100), abs((Tck)%100), abs((Tck-step)%100), abs((Tck-2*step)%100));
  label.align = LV_TEXT_ALIGN_CENTER;
  label.font = &lv_font_sans14;
  lv_canvas_draw_rect(cvs_alt, 28, 1, MinW, PANEL_ALT_H-2, &rect);
  lv_canvas_draw_text(cvs_alt, 28, -MinH-1+dY, MinW , &label, text);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void draw_vert_speed(float vert_speed, uint8_t is_vspeed_valid)
{
  const uint16_t Y0 = PANEL_VSPD_H/2;
  const uint16_t TextW = PANEL_VSPD_W-4;
  const uint16_t TextH = 16;

  if(!is_vspeed_valid)
  {
    lv_obj_add_flag(cvs_speed_v, LV_OBJ_FLAG_HIDDEN);
    return;
  }

  int v;
  char text[6];
  v = (int)roundf(vert_speed);
  sprintf(text,"%3d", v);
  rect.bg_color = COLOR_BLACK;
  rect.bg_opa = LV_OPA_COVER;
  label.color = COLOR_WHITE;
  label.align = LV_TEXT_ALIGN_CENTER;
  label.font = &lv_font_sans16;
  lv_canvas_draw_rect(cvs_speed_v, (PANEL_VSPD_W-TextW)/2, Y0-TextH/2+1, TextW, TextH, &rect);
  lv_canvas_draw_text(cvs_speed_v, 0, Y0-TextH/2+1, TextW , &label, text);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void draw_speed_scale(float speed, const uint8_t is_speed_valid)
{
  const uint16_t Y0 = MAX_Y/2 + VERT_OFFS;
  const uint16_t TextW = 32;
  const uint16_t LineW = SIDE_PANEL_W-TextW-2;
  const uint16_t UnitH = 20;
  const uint16_t LineStep = 40;

  rect.bg_color = COLOR_BLACK;
  rect.bg_opa = LV_OPA_60;

  if(!is_speed_valid)
  {
    lv_canvas_draw_rect(cvs_background,0,0,SIDE_PANEL_W,MAX_Y, &rect);
    return;
  }

  lv_canvas_draw_rect(cvs_background,0,0,SIDE_PANEL_W,MAX_Y-UnitH, &rect);

  int dy = (int)(MODF(speed,20.0f)/20.0f*(float)LineStep);
  for(int i=-4; i<=3; i++)
  {
    line.color = COLOR_WHITE;
    line.width = 2;
    lv_point_t line_maj[] = {{0,Y0+dy-i*LineStep}, {LineW,Y0+dy-i*LineStep}};
    lv_canvas_draw_line(cvs_background, line_maj, 2, &line);
    lv_point_t line_min[] = {{0,Y0+dy-i*LineStep+LineStep/2}, {LineW/2,Y0+dy-i*LineStep+LineStep/2}};
    lv_canvas_draw_line(cvs_background, line_min, 2, &line);

    char text[6];
    sprintf(text,"%2d0",(int)floorf(speed/10.0f) + 2*i);
    label.color = COLOR_WHITE;
    label.align = LV_TEXT_ALIGN_LEFT;
    label.font = &lv_font_sans16;
    lv_canvas_draw_text(cvs_background, LineW+2, Y0+dy-i*LineStep-7, TextW , &label, text);
  }

  label.color = COLOR_WHITE;
  label.align = LV_TEXT_ALIGN_CENTER;
  label.font = &lv_font_sans10;
  lv_canvas_draw_rect(cvs_background,0,MAX_Y-UnitH,SIDE_PANEL_W,UnitH, &rect);
  lv_canvas_draw_text(cvs_background, 0, MAX_Y-UnitH+5, SIDE_PANEL_W , &label, "км/ч");
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void draw_speed_label(float speed, uint8_t is_speed_valid)
{
  if(!is_speed_valid)
  {
    lv_obj_add_flag(cvs_speed, LV_OBJ_FLAG_HIDDEN);
    return;
  }

  rect.bg_color = COLOR_BLACK;
  rect.bg_opa = LV_OPA_COVER;
  label.color = COLOR_WHITE;
  label.font = &lv_font_sans18;

  const uint16_t Y0 = PANEL_SPD_H/2;
  const uint16_t MajW = 24;
  const uint16_t MinW = MajW/2;
  const uint16_t TextH = 18;

  lv_canvas_draw_rect(cvs_speed,9, Y0-TextH/2+1,MajW,TextH, &rect);

  char text[20];
  if(speed < 0.0f)
    sprintf(text,"-%2d",(int)fabsf(truncf(speed/10.0f)));
  else
    sprintf(text,"%2d",(int)truncf(speed/10.0f));
  label.align = LV_TEXT_ALIGN_RIGHT;
  lv_canvas_draw_text(cvs_speed, 8, Y0-TextH/2+1, MajW , &label, text);

  int step = 2;
  if(speed > 200.0f)
    step = 5;

  const float Scale = (float)(TextH+1);
  const int Tck = (((int)truncf(speed))/step)*step;
  const int dY = (int)truncf(Scale*fmodf(fmodf(speed, 10.0f), (float)step)/((float)step));

  sprintf(text,"%1d\n%1d\n%1d\n%1d\n%1d", abs((Tck+2*step)%10), abs((Tck+step)%10), abs((Tck)%10), abs((Tck-step)%10), abs((Tck-2*step)%10));
  lv_canvas_draw_rect(cvs_speed,9+MajW+2, 1, MinW, PANEL_SPD_H-2, &rect);
  label.align = LV_TEXT_ALIGN_CENTER;
  lv_canvas_draw_text(cvs_speed, 9+MajW+2, -TextH+dY, MinW , &label, text);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void view_message(const char* message)
{
  const uint16_t Y0 = PANEL_MSG_H/2;
  const uint16_t TextW = PANEL_MSG_W;
  const uint16_t TextH = 18;

  lv_obj_clear_flag(cvs_msg_box, LV_OBJ_FLAG_HIDDEN);
  lv_canvas_fill_bg(cvs_msg_box, COLOR_BLACK, LV_OPA_COVER);
  const lv_point_t border[] = {{1,1}, {PANEL_MSG_W-1,1}, {PANEL_MSG_W-1,PANEL_MSG_H-1}, {1,PANEL_MSG_H-1}, {1,1}};
  line.color = COLOR_WHITE;
  line.width = 2;
  lv_canvas_draw_line(cvs_msg_box, border, 5, &line);
  label.color = COLOR_RED;
  label.font = &lv_font_sans18;
  label.align = LV_TEXT_ALIGN_CENTER;
  lv_canvas_draw_text(cvs_msg_box, 0, Y0-TextH/2+2, TextW , &label, message);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void clear_message(const char* message)
{
  lv_obj_add_flag(cvs_msg_box, LV_OBJ_FLAG_HIDDEN);
}
//------------------------------------------------------------------------------
