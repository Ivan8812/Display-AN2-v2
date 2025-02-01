/*******************************************************************************
 * Size: 10 px
 * Bpp: 2
 * Opts: 
 ******************************************************************************/

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#ifndef LV_FONT_SANS10
#define LV_FONT_SANS10 1
#endif

#if LV_FONT_SANS10

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0020 " " */

    /* U+002D "-" */
    0x2, 0xf1, 0x0,

    /* U+002E "." */
    0x21, 0xc0,

    /* U+002F "/" */
    0x7, 0x3, 0x80, 0xc0, 0x70, 0x38, 0xc, 0x7,
    0x3, 0x80,

    /* U+0030 "0" */
    0x1f, 0x47, 0xac, 0xb1, 0xcb, 0xd, 0xb0, 0xd7,
    0x1c, 0x3a, 0xc1, 0xf4,

    /* U+0031 "1" */
    0xb, 0xb, 0xc3, 0x70, 0x1c, 0x7, 0x1, 0xc0,
    0x70, 0x1c,

    /* U+0032 "2" */
    0x2f, 0x47, 0x7c, 0x1, 0xc0, 0x2c, 0xb, 0x1,
    0xd0, 0x79, 0x4b, 0xfd,

    /* U+0033 "3" */
    0x3f, 0x42, 0x6c, 0x2, 0xc2, 0xf0, 0x6, 0xc0,
    0x1d, 0x57, 0xc7, 0xf4,

    /* U+0034 "4" */
    0x3, 0x80, 0xb8, 0xf, 0x83, 0x78, 0x73, 0x8f,
    0xfe, 0x3, 0xc0, 0x38,

    /* U+0035 "5" */
    0x3f, 0xc3, 0x94, 0x70, 0x7, 0xf4, 0x16, 0xc0,
    0x1c, 0x57, 0xc7, 0xf4,

    /* U+0036 "6" */
    0xb, 0xc3, 0x94, 0x70, 0xb, 0xf8, 0xb6, 0xcb,
    0xd, 0x7a, 0xc1, 0xf4,

    /* U+0037 "7" */
    0xbf, 0xd5, 0x6c, 0x2, 0xc0, 0x34, 0x7, 0x0,
    0xe0, 0x1d, 0x2, 0xc0,

    /* U+0038 "8" */
    0x2f, 0x47, 0x6c, 0x72, 0xc2, 0xf4, 0x3b, 0x8b,
    0xd, 0xb5, 0xc2, 0xf4,

    /* U+0039 "9" */
    0x2f, 0x47, 0x6c, 0xb0, 0xdb, 0x1d, 0x3f, 0xd0,
    0x1c, 0x17, 0x83, 0xe0,

    /* U+0046 "F" */
    0x3f, 0xce, 0x53, 0x40, 0xd0, 0x3f, 0xce, 0x43,
    0x40, 0xd0,

    /* U+0054 "T" */
    0xff, 0xe5, 0xf4, 0xe, 0x0, 0xe0, 0xe, 0x0,
    0xe0, 0xe, 0x0, 0xe0,

    /* U+005F "_" */
    0x2a, 0x80,

    /* U+0060 "`" */
    0x10, 0x1c,

    /* U+041C "М" */
    0x3c, 0xb, 0x8f, 0x43, 0xe3, 0xe1, 0xf8, 0xec,
    0xae, 0x37, 0x37, 0x8d, 0xec, 0xe3, 0x5f, 0x38,
    0xd3, 0x4e,

    /* U+043A "к" */
    0x34, 0xe0, 0xdb, 0x3, 0xb0, 0xe, 0xd0, 0x36,
    0xc0, 0xd3, 0xc0,

    /* U+043C "м" */
    0x3c, 0x2e, 0x3d, 0x3e, 0x3a, 0x7a, 0x37, 0xaa,
    0x33, 0xda, 0x31, 0xca,

    /* U+0447 "ч" */
    0x70, 0xb7, 0xb, 0x70, 0xf3, 0xff, 0x0, 0xb0,
    0xb
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 42, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 0, .adv_w = 51, .box_w = 3, .box_h = 3, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 3, .adv_w = 46, .box_w = 3, .box_h = 2, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 5, .adv_w = 66, .box_w = 5, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 15, .adv_w = 91, .box_w = 6, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 27, .adv_w = 91, .box_w = 5, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 37, .adv_w = 91, .box_w = 6, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 49, .adv_w = 91, .box_w = 6, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 61, .adv_w = 91, .box_w = 6, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 73, .adv_w = 91, .box_w = 6, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 85, .adv_w = 91, .box_w = 6, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 97, .adv_w = 91, .box_w = 6, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 109, .adv_w = 91, .box_w = 6, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 121, .adv_w = 91, .box_w = 6, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 133, .adv_w = 88, .box_w = 5, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 143, .adv_w = 93, .box_w = 6, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 155, .adv_w = 66, .box_w = 6, .box_h = 1, .ofs_x = -1, .ofs_y = -2},
    {.bitmap_index = 157, .adv_w = 97, .box_w = 4, .box_h = 2, .ofs_x = 1, .ofs_y = 7},
    {.bitmap_index = 159, .adv_w = 151, .box_w = 9, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 177, .adv_w = 99, .box_w = 7, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 188, .adv_w = 135, .box_w = 8, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 200, .adv_w = 105, .box_w = 6, .box_h = 6, .ofs_x = 0, .ofs_y = 0}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/

static const uint16_t unicode_list_0[] = {
    0x0, 0xd, 0xe, 0xf, 0x10, 0x11, 0x12, 0x13,
    0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x26, 0x34,
    0x3f, 0x40, 0x3fc, 0x41a, 0x41c, 0x427
};

/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 32, .range_length = 1064, .glyph_id_start = 1,
        .unicode_list = unicode_list_0, .glyph_id_ofs_list = NULL, .list_length = 22, .type = LV_FONT_FMT_TXT_CMAP_SPARSE_TINY
    }
};



/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LVGL_VERSION_MAJOR >= 8
/*Store all the custom data of the font*/
static  lv_font_fmt_txt_glyph_cache_t cache;
static const lv_font_fmt_txt_dsc_t font_dsc = {
#else
static lv_font_fmt_txt_dsc_t font_dsc = {
#endif
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = NULL,
    .kern_scale = 0,
    .cmap_num = 1,
    .bpp = 2,
    .kern_classes = 0,
    .bitmap_format = 0,
#if LVGL_VERSION_MAJOR >= 8
    .cache = &cache
#endif
};


/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LVGL_VERSION_MAJOR >= 8
const lv_font_t lv_font_sans10 = {
#else
lv_font_t lv_font_sans10 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 11,          /*The maximum line height required by the font*/
    .base_line = 2,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -1,
    .underline_thickness = 0,
#endif
    .dsc = &font_dsc,          /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
};



#endif /*#if LV_FONT_SANS10*/

