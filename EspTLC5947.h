#pragma once
#include <stdint.h>

#define TLC_BYTE 36
#define TLC_BIT  36 * 8
#define TLC_WORD 9
#define TLC_CHAN 24

void tlc_setpwm(volatile uint32_t* pwmbuffer, uint8_t numdrivers, uint16_t chan, uint16_t pwm);
uint16_t tlc_getpwm(volatile uint32_t* pwmbuffer, uint8_t numdrivers, uint16_t chan);
void tlc_write(volatile uint32_t* pwmbuffer, uint32_t numdrivers, int8_t latch_pin, int8_t blank_pin);
void tlc_clear(volatile uint32_t* pwmbuffer, uint8_t numdrivers);
