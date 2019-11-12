#pragma once
#include <FixedPoints.h>
#include <FixedPointsCommon.h>

inline SQ15x16 modfp(SQ15x16 x, SQ15x16 y)
{
  return x - (x/y).getInteger() * y;  
}

inline SQ15x16 minfp(SQ15x16 x, SQ15x16 y)
{
  return x > y ? y : x;  
}

inline SQ15x16 maxfp(SQ15x16 x, SQ15x16 y)
{
  return x < y ? y : x;  
}

inline SQ15x16 absfp(SQ15x16 x)
{
  return x < 0 ? -x : x;  
}

SQ15x16 sinfp(SQ15x16 a);

inline SQ15x16 cosfp(SQ15x16 a)
{
  return sinfp(a + SQ15x16::Pi / 2);
}

SQ15x16 atan2fp(SQ15x16 y, SQ15x16 x);

inline SQ15x16 sqrtfp(SQ15x16 x)
{
  SQ15x16 k = 8;
  k = (k + x/k)/2;
  k = (k + x/k)/2;
  k = (k + x/k)/2;
  k = (k + x/k)/2;
  k = (k + x/k)/2;
  return k;
}

inline SQ15x16 atanfp(SQ15x16 z){
  const SQ15x16 n1 = 0.97239411;
  const SQ15x16 n2 = -0.19194795;
  return (n1 + n2 * z * z) * z;
}

inline SQ15x16 uipowfp(SQ15x16 x, uint32_t n){
  SQ15x16 r = x;
  for (uint32_t i = 1; i < n; i ++){
    r *= x;
  }
  return r;
}
