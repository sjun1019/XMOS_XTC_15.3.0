// Copyright (c) 2016-2020, XMOS Ltd, All rights reserved
#pragma once

// This file contains private implementation details and is not part of the API.
// The contents may vary between releases.
#include <stdint.h>
#include <xs1.h>
#include <xcore/_support/xcore_common.h>
#include <xcore/_support/xcore_resource_impl.h>
#include <xcore/_support/xcore_meta_macro.h>


#ifdef __cplusplus
extern "C" {
#endif

typedef resource_t __xcore_streaming_chanend_t;

_XCORE_INLINE __xcore_streaming_chanend_t __xcore_chanend_alloc(void)
{
  __xcore_streaming_chanend_t __c;
  _XCORE_RESOURCE_ALLOC(__c, XS1_RES_TYPE_CHANEND);
  return __c;
}

_XCORE_INLINE void __xcore_chanend_free(resource_t __c)
{
  __xcore_resource_free(__c);
}

_XCORE_INLINE void __xcore_chanend_set_dest(resource_t __c, resource_t __dst)
{
  asm("setd res[%0], %1" :: "r" (__c), "r" (__dst));
}

_XCORE_INLINE resource_t __xcore_chanend_get_dest(resource_t __c)
{
  uint32_t __data;
  asm volatile("getd %[__dest], res[%[__chanend]]" : [__dest] "=r" (__data) : [__chanend] "r" (__c));
  return __data;
}

_XCORE_INLINE void __xcore_chanend_out_word(resource_t __c, uint32_t __data)
{
  asm volatile("out res[%0], %1" :: "r" (__c), "r" (__data));
}

_XCORE_INLINE void __xcore_chanend_out_byte(resource_t __c, uint8_t __data)
{
  asm volatile("outt res[%0], %1" :: "r" (__c), "r" (__data));
}

_XCORE_INLINE uint32_t __xcore_chanend_in_word(resource_t __c)
{
  uint32_t __data;
  asm volatile("in %0, res[%1]" : "=r" (__data): "r" (__c));
  return __data;
}

_XCORE_INLINE uint8_t __xcore_chanend_in_byte(resource_t __c)
{
  uint8_t __data;
  asm volatile("int %0, res[%1]" : "=r" (__data): "r" (__c));
  return __data;
}

_XCORE_INLINE uint8_t __xcore_chanend_in_ct(resource_t __c)
{
  uint8_t __data;
  asm volatile("inct %[__token], res[%[__chanend]]" : [__token] "=r" (__data) : [__chanend] "r" (__c));
  return __data;
}

#define _XCORE_CHANEND_OUTCTI_CASE_N_ASM(_N) \
  case _N: \
    asm volatile("outct res[%[__chanend]], %[__token]" :: [__chanend] "r" (__c), [__token] "n" (_N)); \
    break;

_XCORE_INLINE void __xcore_chanend_out_ct(resource_t __c, uint8_t __ct)
{
  if (__builtin_constant_p(__ct) && __ct <= 11)
  {
    switch (__ct)
    {
      _XCORE_APPLY_NOSEP(_XCORE_CHANEND_OUTCTI_CASE_N_ASM, 0,1,2,3,4,5,6,7,8,9,10,11)
      default: __builtin_unreachable(); break;
    }
  }
  else
  {
    asm volatile("outct res[%0], %1" :: "r" (__c), "r" (__ct));
  }
}

#define _XCORE_CHANEND_CHKCTI_CASE_N_ASM(_N) \
  case _N: \
    asm volatile("chkct res[%[__chanend]], %[__token]" :: [__chanend] "r" (__c), [__token] "n" (_N)); \
    break;

_XCORE_INLINE void __xcore_chanend_check_ct(resource_t __c, const uint8_t __ct)
{
  if (__builtin_constant_p(__ct) && __ct <= 11)
  {
    switch (__ct)
    {
      _XCORE_APPLY_NOSEP(_XCORE_CHANEND_CHKCTI_CASE_N_ASM, 0,1,2,3,4,5,6,7,8,9,10,11)
      default: __builtin_unreachable(); break;
    }
  }
  else
  {
    asm volatile("chkct res[%0], %1" :: "r" (__c), "r" (__ct));
  }
}

_XCORE_INLINE int __xcore_chanend_test_control_token(resource_t __c)
{
  unsigned __res;
  asm volatile("testct %[__res], res[%[__chanend]]" : [__res] "=r" (__res) : [__chanend] "r" (__c));
  return __res;
}


_XCORE_INLINE unsigned __xcore_chanend_test_control_token_word(resource_t __c)
{
  unsigned __res;
  asm volatile("testwct %[__res], res[%[__chanend]]" : [__res] "=r" (__res) : [__chanend] "r" (__c));
  return __res;
}

_XCORE_INLINE __xcore_bool_t __xcore_chanend_test_dest_local(resource_t __c)
{
  unsigned __res;
  asm volatile("testlcl %[__res], res[%[__chanend]]" : [__res] "=r" (__res) : [__chanend] "r" (__c));
  return __res;
}

_XCORE_INLINE void __xcore_channend_set_network(resource_t __c, unsigned __n)
{
  asm volatile("setn res[%[__chanend]], %[__net]" :: [__chanend] "r" (__c), [__net] "r" (__n));
}

_XCORE_INLINE unsigned __xcore_channend_get_network(resource_t __c)
{
  unsigned __net;
  asm volatile("getn %[__net], res[%[__chanend]]" : [__net] "=r" (__net) : [__chanend] "r" (__c));
  return __net;
}

#ifdef __cplusplus
}
#endif

