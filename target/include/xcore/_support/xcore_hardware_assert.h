// Copyright (c) 2020, XMOS Ltd, All rights reserved
#pragma once

#include <limits.h>
#include <stdint.h>
#include <xcore/_support/xcore_common.h>
#include <xcore/_support/xcore_reference_time.h>

#ifdef __cplusplus
extern "C" {
#endif


_XCORE_INLINE void __xcore_ecallt(int __value)
{
  if (!(__builtin_constant_p(__value) && !__value))
  {
    asm volatile("ecallt %[__value]" : : [__value] "r" (__value));
  }
  __builtin_assume(__value == 0);
}

_XCORE_INLINE void __xcore_ecallf(int __value)
{
  if (!(__builtin_constant_p(__value) && __value))
  {
    asm volatile("ecallf %[__value]" : : [__value] "r" (__value));
  }
  __builtin_assume(__value != 0);
}

#ifdef _XCORE_HAS_REFERENCE_CLOCK

_XCORE_INLINE int __xcore_not_after_reference_time(uint32_t __v)
{
    const uint32_t __r = __xcore_get_reference_time();
    const unsigned __tolerance = 1 << ((sizeof(int)*CHAR_BIT)-1);
    return __v < __tolerance
      ? (__r <= __v || __r > __v+__tolerance)
      : (__r <= __v && __r > __v-__tolerance); 
}

_XCORE_INLINE void __xcore_elate(uint32_t __value)
{
  asm volatile("elate %[__value]" : : [__value] "r" (__value));
}

#endif

#ifdef __cplusplus
}
#endif

