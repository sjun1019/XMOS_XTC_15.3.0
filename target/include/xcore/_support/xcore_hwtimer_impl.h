// Copyright (c) 2016-2020, XMOS Ltd, All rights reserved
#pragma once

// This file contains private implementation details and is not part of the API.
// The contents may vary between releases.


#include <stdint.h>
#include <xs1.h>
#include <xcore/_support/xcore_common.h>
#include <xcore/_support/xcore_resource_impl.h>

#ifdef __cplusplus
extern "C" {
#endif

_XCORE_INLINE void __xcore_hwtimer_realloc_xc_timer(void)
{
  // __init_threadlocal_timer has resource ID in r2 and it may be zero.
  // Implement a checked version here instead.
  unsigned __tmr, __addr;
  _XCORE_RESOURCE_ALLOC(__tmr, XS1_RES_TYPE_TIMER);
  asm volatile( "ecallf %0" :: "r" (__tmr));
  asm volatile( "ldaw %0, dp[__timers]" : "=r" (__addr));
  asm volatile( "get r11, id" ::: /* clobbers */ "r11");
  asm volatile( "stw  %0, %1[r11]" : : "r" (__tmr), "r" (__addr));
}

extern void __free_threadlocal_timer(void);
_XCORE_INLINE void __xcore_hwtimer_free_xc_timer(void)
{
  __free_threadlocal_timer();
}

_XCORE_INLINE resource_t __xcore_hwtimer_alloc(void)
{
  resource_t __t;
  _XCORE_RESOURCE_ALLOC(__t, XS1_RES_TYPE_TIMER);
  return __t;
}

_XCORE_INLINE void __xcore_hwtimer_free(resource_t __t)
{
  __xcore_resource_free(__t);
}

_XCORE_INLINE uint32_t __xcore_hwtimer_get_time(resource_t __t)
{
  register uint32_t __now;
  asm volatile("in %0, res[%1]" : "=r" (__now): "r" (__t));
  return __now;
}

_XCORE_INLINE uint32_t __xcore_hwtimer_get_trigger_time(resource_t __t)
{
  uint32_t __tval;
  asm volatile ("getd %0, res[%1]" : "=r" (__tval) : "r" (__t));
  return __tval;
}

_XCORE_INLINE void __xcore_hwtimer_change_trigger_time(resource_t __t, uint32_t __time)
{
  asm volatile("setd res[%0], %1" :: "r" (__t), "r" (__time));
}

_XCORE_INLINE void __xcore_hwtimer_set_trigger_time(resource_t __t, uint32_t __time)
{
  _XCORE_RESOURCE_SETCI(__t, XS1_SETC_COND_AFTER);
  __xcore_hwtimer_change_trigger_time(__t, __time);
}

_XCORE_INLINE void __xcore_hwtimer_clear_trigger_time(resource_t __t)
{
  _XCORE_RESOURCE_SETCI(__t, XS1_SETC_COND_NONE);
  // hwtimer_get_time() will respond immediately
}

#ifdef __cplusplus
}
#endif
