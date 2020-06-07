#include "EspTLC5947.h"
#include <stdlib.h>
#include <Arduino.h>

inline void SPI_setDataBits(uint16_t bits) {
  const uint32_t mask = ~((SPIMMOSI << SPILMOSI) | (SPIMMISO << SPILMISO));
  bits--;
  SPI1U1 = ((SPI1U1 & mask) | ((bits << SPILMOSI) | (bits << SPILMISO)));
}

void tlc_write(volatile uint32_t* pwmbuffer, uint32_t numdrivers, int8_t latch_pin, int8_t blank_pin) {
  //if (latch_pin > 0) digitalWrite(latch_pin, 0);
  for (int i = 0; i < numdrivers; i ++){
    while(SPI1CMD & SPIBUSY);
    SPI_setDataBits(24 * 12);
    
    uint32_t * fifoPtr = (uint32_t*)&SPI1W0;
    
    for (uint8_t j = 0; j < TLC_WORD; j ++) {
        fifoPtr[j] = pwmbuffer[i * TLC_WORD + j];
    }
    __sync_synchronize();
    SPI1CMD |= SPIBUSY; 

  }
  // Latch
  while(SPI1CMD & SPIBUSY);
  if (latch_pin >= 0) GPOS = (1 << latch_pin);
  if (blank_pin >= 0) GPOS = (1 << blank_pin);
  if (blank_pin >= 0) GPOC = (1 << blank_pin);
  if (latch_pin >= 0) GPOC = (1 << latch_pin);
}

void tlc_setpwm(volatile uint32_t* pwmbuffer, uint8_t numdrivers, uint16_t chan, uint16_t pwm) {
  if (pwm > 4095) pwm = 4095;
  if (chan >= 24 * numdrivers) return;

  chan = numdrivers * TLC_CHAN - chan - 1;

  volatile uint8_t* buf = (volatile uint8_t *) pwmbuffer;
  if (chan % 2 == 0){
    uint32_t index = chan * 3 / 2;
    buf[index] = pwm >> 4;
    buf[index + 1] &= 0x0F;
    buf[index + 1] |= pwm << 4;
  }
  else {
    uint32_t index = (chan - 1) * 3 / 2  +  1;
    buf[index] &= 0xF0;
    buf[index] |= pwm >> 8;
    buf[index + 1] = pwm;
  }
}

uint16_t tlc_getpwm(volatile uint32_t* pwmbuffer, uint8_t numdrivers, uint16_t chan){
  uint16_t pwm;
  volatile uint8_t* buf = (volatile uint8_t *) pwmbuffer;
  if (chan >= 24 * numdrivers) return 0;
  if (chan % 2 == 0){
    uint32_t index = chan * 3 / 2;
    //buf[index] = pwm >> 4;
    //buf[index + 1] &= 0x0F;
    //buf[index + 1] |= (pwm << 4) & 0xF0;
    pwm = buf[index] << 4;
    pwm |= buf[index + 1] >> 4;
  }
  else {
    uint32_t index = (chan - 1) * 3 / 2  +  1;
    //buf[index] &= 0xF0;
    //buf[index] |= pwm >> 8;
    //buf[index + 1] = pwm;
    pwm = buf[index] << 8;
    pwm |= buf[index + 1];
  }
}

void tlc_clear(volatile uint32_t* pwmbuffer, uint8_t numdrivers){
  for (int i = 0; i < numdrivers; i ++){
    for (uint8_t j = 0; j < TLC_WORD; j ++) {
      pwmbuffer[i * TLC_WORD + j] = 0;
    }
  }
}
