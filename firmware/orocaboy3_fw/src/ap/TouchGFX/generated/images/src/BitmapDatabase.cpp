// 4.12.2 dither_algorithm=2 alpha_dither=yes layout_rotation=0 opaque_image_format=RGB565 non_opaque_image_format=ARGB8888 section=ExtFlashSection extra_section=ExtFlashSection generate_png=no 0x96facb82
// Generated by imageconverter. Please, do not edit!

#include <touchgfx/Bitmap.hpp>
#include <BitmapDatabase.hpp>

extern const unsigned char _a1[]; // BITMAP_A1_ID = 0, Size: 200x79 pixels
extern const unsigned char _a2[]; // BITMAP_A2_ID = 1, Size: 41x24 pixels
extern const unsigned char _a4[]; // BITMAP_A4_ID = 2, Size: 24x24 pixels
extern const unsigned char _b1[]; // BITMAP_B1_ID = 3, Size: 24x24 pixels
extern const unsigned char _blue_buttons_round_edge_small[]; // BITMAP_BLUE_BUTTONS_ROUND_EDGE_SMALL_ID = 4, Size: 170x60 pixels
extern const unsigned char _dark_buttons_round_edge_small[]; // BITMAP_DARK_BUTTONS_ROUND_EDGE_SMALL_ID = 5, Size: 170x60 pixels
extern const unsigned char _dark_buttons_round_edge_small_pressed[]; // BITMAP_DARK_BUTTONS_ROUND_EDGE_SMALL_PRESSED_ID = 6, Size: 170x60 pixels
extern const unsigned char _dark_pageindicator_dot_indicator_small_highlight[]; // BITMAP_DARK_PAGEINDICATOR_DOT_INDICATOR_SMALL_HIGHLIGHT_ID = 7, Size: 15x15 pixels
extern const unsigned char _dark_pageindicator_dot_indicator_small_normal[]; // BITMAP_DARK_PAGEINDICATOR_DOT_INDICATOR_SMALL_NORMAL_ID = 8, Size: 15x15 pixels
extern const unsigned char _doom[]; // BITMAP_DOOM_ID = 9, Size: 200x95 pixels
extern const unsigned char _fmsx[]; // BITMAP_FMSX_ID = 10, Size: 200x72 pixels
extern const unsigned char _orocaboy1[]; // BITMAP_OROCABOY1_ID = 11, Size: 24x24 pixels
extern const unsigned char _orocaboy2[]; // BITMAP_OROCABOY2_ID = 12, Size: 24x24 pixels
extern const unsigned char _orocaboy3[]; // BITMAP_OROCABOY3_ID = 13, Size: 24x24 pixels
extern const unsigned char _orocaboy4[]; // BITMAP_OROCABOY4_ID = 14, Size: 24x24 pixels

const touchgfx::Bitmap::BitmapData bitmap_database[] =
{
    { _a1, 0, 200, 79, 32, 4, 136, (uint8_t)(touchgfx::Bitmap::ARGB8888) >> 3, 39, (uint8_t)(touchgfx::Bitmap::ARGB8888) & 0x7 },
    { _a2, 0, 41, 24, 3, 3, 27, (uint8_t)(touchgfx::Bitmap::ARGB8888) >> 3, 3, (uint8_t)(touchgfx::Bitmap::ARGB8888) & 0x7 },
    { _a4, 0, 24, 24, 1, 7, 13, (uint8_t)(touchgfx::Bitmap::ARGB8888) >> 3, 10, (uint8_t)(touchgfx::Bitmap::ARGB8888) & 0x7 },
    { _b1, 0, 24, 24, 9, 8, 5, (uint8_t)(touchgfx::Bitmap::ARGB8888) >> 3, 9, (uint8_t)(touchgfx::Bitmap::ARGB8888) & 0x7 },
    { _blue_buttons_round_edge_small, 0, 170, 60, 11, 4, 148, (uint8_t)(touchgfx::Bitmap::ARGB8888) >> 3, 50, (uint8_t)(touchgfx::Bitmap::ARGB8888) & 0x7 },
    { _dark_buttons_round_edge_small, 0, 170, 60, 11, 4, 148, (uint8_t)(touchgfx::Bitmap::ARGB8888) >> 3, 50, (uint8_t)(touchgfx::Bitmap::ARGB8888) & 0x7 },
    { _dark_buttons_round_edge_small_pressed, 0, 170, 60, 11, 4, 148, (uint8_t)(touchgfx::Bitmap::ARGB8888) >> 3, 50, (uint8_t)(touchgfx::Bitmap::ARGB8888) & 0x7 },
    { _dark_pageindicator_dot_indicator_small_highlight, 0, 15, 15, 3, 3, 9, (uint8_t)(touchgfx::Bitmap::ARGB8888) >> 3, 9, (uint8_t)(touchgfx::Bitmap::ARGB8888) & 0x7 },
    { _dark_pageindicator_dot_indicator_small_normal, 0, 15, 15, 3, 3, 9, (uint8_t)(touchgfx::Bitmap::ARGB8888) >> 3, 9, (uint8_t)(touchgfx::Bitmap::ARGB8888) & 0x7 },
    { _doom, 0, 200, 95, 0, 0, 200, (uint8_t)(touchgfx::Bitmap::RGB565) >> 3, 95, (uint8_t)(touchgfx::Bitmap::RGB565) & 0x7 },
    { _fmsx, 0, 200, 72, 0, 0, 200, (uint8_t)(touchgfx::Bitmap::RGB565) >> 3, 72, (uint8_t)(touchgfx::Bitmap::RGB565) & 0x7 },
    { _orocaboy1, 0, 24, 24, 0, 0, 24, (uint8_t)(touchgfx::Bitmap::RGB565) >> 3, 24, (uint8_t)(touchgfx::Bitmap::RGB565) & 0x7 },
    { _orocaboy2, 0, 24, 24, 0, 0, 24, (uint8_t)(touchgfx::Bitmap::RGB565) >> 3, 24, (uint8_t)(touchgfx::Bitmap::RGB565) & 0x7 },
    { _orocaboy3, 0, 24, 24, 0, 0, 24, (uint8_t)(touchgfx::Bitmap::RGB565) >> 3, 24, (uint8_t)(touchgfx::Bitmap::RGB565) & 0x7 },
    { _orocaboy4, 0, 24, 24, 0, 0, 24, (uint8_t)(touchgfx::Bitmap::RGB565) >> 3, 24, (uint8_t)(touchgfx::Bitmap::RGB565) & 0x7 }
};

namespace BitmapDatabase
{
const touchgfx::Bitmap::BitmapData* getInstance()
{
    return bitmap_database;
}
uint16_t getInstanceSize()
{
    return (uint16_t)(sizeof(bitmap_database) / sizeof(touchgfx::Bitmap::BitmapData));
}
}
