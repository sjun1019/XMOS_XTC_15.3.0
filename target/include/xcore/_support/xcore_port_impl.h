// Copyright (c) 2016-2020, XMOS Ltd, All rights reserved
#pragma once

// This file contains private implementation details and is not part of the API.
// The contents may vary between releases.

#include <stdint.h>
#include <stddef.h>
#include <xs1.h>

#include <xcore/_support/xcore_common.h>
#include <xcore/_support/xcore_feature_test_impl.h>
#include <xcore/_support/xcore_resource_impl.h>
#include <xcore/_support/xcore_meta_macro.h>

#ifdef __cplusplus
extern "C" {
#endif


#define _XCORE_PORT_TIMESTAMP_MIN 0
#define _XCORE_PORT_TIMESTAMP_MAX 65535


_XCORE_INLINE void __xcore_port_set_transfer_width(resource_t __p, size_t __width)
{
  asm volatile("settw res[%0], %1" :: "r" (__p), "r" (__width));
}

_XCORE_INLINE void __xcore_port_enable(unsigned __id)
{
  _XCORE_RESOURCE_SETCI((resource_t)__id, XS1_SETC_INUSE_ON);
}

_XCORE_INLINE void __xcore_port_reset(resource_t __p)
{
  _XCORE_RESOURCE_SETCI(__p, XS1_SETC_INUSE_ON);
}

_XCORE_INLINE void __xcore_port_free(resource_t __p)
{
  _XCORE_RESOURCE_SETCI(__p, XS1_SETC_INUSE_OFF);
}

_XCORE_INLINE void __xcore_port_set_buffered(resource_t __p)
{
  _XCORE_RESOURCE_SETCI(__p, XS1_SETC_BUF_BUFFERS);
}

_XCORE_INLINE void __xcore_port_set_unbuffered(resource_t __p)
{
  _XCORE_RESOURCE_SETCI(__p, XS1_SETC_BUF_NOBUFFERS);
}

_XCORE_INLINE void __xcore_port_set_clock(resource_t __p, resource_t __clk)
{
  asm volatile("setclk res[%0], %1" :: "r" (__p), "r" (__clk));
}

_XCORE_INLINE void __xcore_port_set_inout_data(resource_t p)
{
  _XCORE_RESOURCE_SETCI(p, XS1_SETC_PORT_DATAPORT);
}

_XCORE_INLINE void __xcore_port_set_out_clock(resource_t __p)
{
  _XCORE_RESOURCE_SETCI(__p, XS1_SETC_PORT_CLOCKPORT);
}

_XCORE_INLINE void __xcore_port_set_out_ready(resource_t __p, resource_t __ready_source)
{
  _XCORE_RESOURCE_SETCI(__p, XS1_SETC_PORT_READYPORT);
  asm volatile("setrdy res[%0], %1" :: "r" (__p), "r" (__ready_source));
}

_XCORE_INLINE void __xcore_port_set_invert(resource_t __p)
{
  _XCORE_RESOURCE_SETCI(__p, XS1_SETC_INV_INVERT);
}

_XCORE_INLINE void __xcore_port_set_no_invert(resource_t __p)
{
  _XCORE_RESOURCE_SETCI(__p, XS1_SETC_INV_NOINVERT);
}

_XCORE_INLINE void __xcore_port_set_sample_falling_edge(resource_t __p)
{
  _XCORE_RESOURCE_SETCI(__p, XS1_SETC_SDELAY_SDELAY);
}

_XCORE_INLINE void __xcore_port_set_sample_rising_edge(resource_t __p)
{
  _XCORE_RESOURCE_SETCI(__p, XS1_SETC_SDELAY_NOSDELAY);
}

_XCORE_INLINE void __xcore_port_set_master(resource_t __p)
{
  _XCORE_RESOURCE_SETCI(__p, XS1_SETC_MS_MASTER);
}

_XCORE_INLINE void __xcore_port_set_slave(resource_t __p)
{
  _XCORE_RESOURCE_SETCI(__p, XS1_SETC_MS_SLAVE);
}

_XCORE_INLINE void __xcore_port_set_no_ready(resource_t __p)
{
  _XCORE_RESOURCE_SETCI(__p, XS1_SETC_RDY_NOREADY);
}

_XCORE_INLINE void __xcore_port_set_ready_strobed(resource_t __p)
{
  _XCORE_RESOURCE_SETCI(__p, XS1_SETC_RDY_STROBED);
}

_XCORE_INLINE void __xcore_port_set_ready_handshake(resource_t __p)
{
  _XCORE_RESOURCE_SETCI(__p, XS1_SETC_RDY_HANDSHAKE);
}

_XCORE_INLINE uint32_t __xcore_port_get_trigger_time(resource_t __p)
{
  uint32_t __ts;
  asm volatile("getts %0, res[%1]" : "=r" (__ts) : "r" (__p));
  return __ts;
}

_XCORE_INLINE void __xcore_port_set_trigger_time(resource_t __p, uint32_t __t)
{
  asm volatile("setpt res[%0], %1" :: "r" (__p), "r" (__t));
}

_XCORE_INLINE void __xcore_port_clear_trigger_time(resource_t __p)
{
  asm volatile("clrpt res[%0]" :: "r" (__p));
}

_XCORE_INLINE void __xcore_port_set_trigger_value(resource_t __p, uint32_t __d)
{
  asm volatile("setd res[%0], %1" :: "r" (__p), "r" (__d));
}

_XCORE_INLINE void __xcore_port_set_trigger_in_equal(resource_t __p, uint32_t __d)
{
  _XCORE_RESOURCE_SETCI(__p, XS1_SETC_COND_EQ);
  __xcore_port_set_trigger_value(__p, __d);
}

_XCORE_INLINE void __xcore_port_set_trigger_in_not_equal(resource_t __p, uint32_t __d)
{
  _XCORE_RESOURCE_SETCI(__p, XS1_SETC_COND_NEQ);
  __xcore_port_set_trigger_value(__p, __d);
}

_XCORE_INLINE void __xcore_port_clear_trigger_in(resource_t __p)
{
  _XCORE_RESOURCE_SETCI(__p, XS1_SETC_COND_NONE);
}

_XCORE_INLINE uint32_t __xcore_port_peek(resource_t __p)
{
  uint32_t __data;
  asm volatile("peek %[__r], res[%[__p]]"
    : [__r] "=r" (__data)
    : [__p] "r" (__p));
  return __data;
}

_XCORE_INLINE void __xcore_port_out(resource_t __p, uint32_t __data)
{
  asm volatile("out res[%0], %1" :: "r" (__p), "r" (__data));
}

_XCORE_INLINE uint32_t __xcore_port_in(resource_t __p)
{
  uint32_t __data;
  asm volatile("in %0, res[%1]" : "=r" (__data): "r" (__p));
  return __data;
}

_XCORE_INLINE uint32_t __xcore_port_out_shift_right(resource_t __p, uint32_t __data)
{
  // We read-write data
  asm volatile("outshr res[%1], %0" : "+r" (__data) : "r" (__p));
  return __data;
}

_XCORE_INLINE uint32_t __xcore_port_in_shift_right(resource_t __p, uint32_t __data)
{
  // We read-write data
  asm volatile("inshr %0, res[%1]" : "+r" (__data) : "r" (__p));
  return __data;
}

_XCORE_INLINE size_t __xcore_port_endin(resource_t __p)
{
  size_t __num;
  asm volatile("endin %0, res[%1]" : "=r" (__num) : "r" (__p));
  return __num;
}

_XCORE_INLINE void __xcore_port_clear_buffer(resource_t __p)
{
  _XCORE_RESOURCE_SETCI(__p, XS1_SETC_RUN_CLRBUF);
}

_XCORE_INLINE void __xcore_port_sync(resource_t __p)
{
  asm volatile("syncr res[%[__port]]"
               : /* No outputs */
               : [__port] "r" (__p));
}

_XCORE_INLINE void __xcore_port_shift_count_set(resource_t __p, uint32_t __sc)
{
  asm volatile("setpsc res[%[__port]], %[__scount]"
               : /* No outputs */
               : [__port] "r" (__p)
               , [__scount] "r" (__sc));
}

#define _XCORE_PORT_OUTPW_CASE_N_ASM(_N) \
  case _N: \
    asm volatile("outpw res[%[__port]], %[__data], %[__bitp]" \
                 : /* No outputs */ \
                 : [__port] "r" (__p) \
                 , [__data] "r" (__d) \
                 , [__bitp] "n" (_N)); \
    return;

_XCORE_INLINE void __xcore_port_output_partial_word(resource_t __p, uint32_t __d, uint32_t __bitp)
{
  if (__builtin_constant_p(__bitp))
  {
    switch (__bitp)
    {
      _XCORE_APPLY_NOSEP(_XCORE_PORT_OUTPW_CASE_N_ASM, 1,2,3,4,5,6,7,8,16,24,32)
      default: break;
    }
  }
#ifdef _XCORE_HAS_OUTPW_WITH_REGISTERS
  asm volatile("outpw res[%[__port]], %[__data], %[__bitp]"
               : /* No outputs */
               : [__port] "r" (__p)
               , [__data] "r" (__d)
               , [__bitp] "r" (__bitp));
#else
  __xcore_port_shift_count_set(__p, __bitp);
  __xcore_port_out(__p, __d);
#endif
}

#ifdef __cplusplus
}
#endif

