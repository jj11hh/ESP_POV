#pragma once
#include "common.h"
#include "EspTLC5947.h"

const uint8_t led_map[48] = {
  24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
   9, 10, 11,  6,  7,  8,  3,  4,  5,  0,  1,  2,
  45, 46, 47, 42, 43, 44, 39, 40, 41, 36, 37, 38,
  12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23 
};

void setupDisplay();
void ICACHE_RAM_ATTR handleHall (void);
void ICACHE_RAM_ATTR pov_update (void);
SQ15x16 polorDistance(SQ15x16 r1, SQ15x16 a1, SQ15x16 r2, SQ15x16 a2);
SQ15x16 calcSdfCircle(SQ15x16 r, SQ15x16 theta, SQ15x16 R, SQ15x16 pr, SQ15x16 ptheta);
void clearCircle(SQ15x16 r, SQ15x16 theta, SQ15x16 R);
void fillCircle(SQ15x16 r, SQ15x16 theta, SQ15x16 R, Color color);
void clearText5x7(uint32_t r, int32_t s, const char* text, bool flip = false);
void drawText5x7(uint32_t r, int32_t s, const char* text, Color color, bool flip = false);

extern uint8_t brightness;
extern uint32_t scanRotate;
extern uint32_t data_array[TLC_NUM * TLC_WORD];
extern uint8_t pov_buf_bg[SCANLINE * SUBPIXEL_NUM];
extern uint8_t pov_buf[SCANLINE * SUBPIXEL_NUM];
extern uint8_t gamma8[256];


inline void commitBg(){
  memcpy(pov_buf_bg, pov_buf, SCANLINE * SUBPIXEL_NUM);
}

inline void clearBg(){
  memset(pov_buf_bg, 0, SCANLINE * SUBPIXEL_NUM);
  memset(pov_buf, 0, SCANLINE * SUBPIXEL_NUM);
}

inline void clearScr(){
  memset(pov_buf, 0, SCANLINE * SUBPIXEL_NUM);
}

inline void clearFg(){
  memcpy(pov_buf, pov_buf_bg, SCANLINE * SUBPIXEL_NUM);
}
