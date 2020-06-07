#pragma once

#define LATCH_PIN 4
#define BLANK_PIN -1
#define TLC_NUM   2
#define SUBPIXEL_NUM (TLC_NUM * TLC_CHAN)
#define PIXEL_NUM (SUBPIXEL_NUM / 3)
#define LEDNUM 16
#define HALL_PIN 5
#define SCANLINE 128

#define KEY_UP      (1 << 0)
#define KEY_DOWN    (1 << 1)
#define KEY_LEFT    (1 << 2)
#define KEY_RIGHT   (1 << 3)
#define KEY_A       (1 << 4)
#define KEY_B       (1 << 5)

#define PMOD(n, M) ((((n) % (M)) + (M)) % (M))

#define POV_GET_PIXEL(pov, n, s, pc) \
  do {\
    (pc)->r = (pov)[(s) * SUBPIXEL_NUM + 3 * (n)]; \
    (pc)->g = (pov)[(s) * SUBPIXEL_NUM + 3 * (n) + 1]; \
    (pc)->b = (pov)[(s) * SUBPIXEL_NUM + 3 * (n) + 2]; \
  } while(0)

#define POV_SET_PIXEL(pov, n, s, c) \
  do {\
    (pov)[(s) * SUBPIXEL_NUM + 3 * (n) ] = (c).r; \
    (pov)[(s) * SUBPIXEL_NUM + 3 * (n) + 1] = (c).g; \
    (pov)[(s) * SUBPIXEL_NUM + 3 * (n) + 2] = (c).b; \
  } while(0)

struct Color {
 uint8_t r;
 uint8_t g;
 uint8_t b;

 Color(uint8_t r, uint8_t g, uint8_t b): r(r), g(g), b(b) {}
 Color(): r(0), g(0), b(0) {}
};
