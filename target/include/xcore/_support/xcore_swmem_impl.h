// Copyright (c) 2020-2024, XMOS Ltd, All rights reserved
#pragma once

#include <xcore/_support/xcore_feature_test_impl.h>

#ifndef _XCORE_HAS_SWMEM
#error Software memory not available on this platform
#endif

#include <stdint.h>
#include <string.h>
#include <xs1.h>
#include <xcore/_support/xcore_common.h>
#include <xcore/_support/xcore_resource_impl.h>

#ifdef __cplusplus
extern "C" {
#endif

/** \brief Number of words which must be filled per fill request
 * \hideinitializer
 */
#define _XCORE_SWMEM_LINE_WORDS (8) 

_XCORE_INLINE void __xcore_swmem_res_enable(resource_t __res)
{
  asm volatile("setc res[%[__res]], %[__value]"
               : /* No outputs */
               : [__res] "r" (__res)
               , [__value] "nr" (XS1_SETC_INUSE_ON));
}

_XCORE_INLINE void __xcore_swmem_res_free(resource_t __r)
{
  asm volatile("setc res[%[__res]], %[__value]"
               : /* No outputs */
               : [__res] "r" (__r)
               , [__value] "nr" (XS1_SETC_INUSE_OFF));
}

_XCORE_INLINE void *__xcore_swmem_res_in_address(resource_t __r)
{
  volatile uint32_t *__addr;
  asm volatile("in %[__addr], res[%[__res]]"
               : [__addr] "=r" (__addr)
               : [__res] "r" (__r));
  return (void *)__addr;
}

_XCORE_INLINE void __xcore_swmem_fill_populate_word(_XCORE_UNUSED resource_t __r, const void *__slot, unsigned __word, uint32_t __value)
{
  // We cast back to a volatile, non-const because the only valid source of the address is
  // __xcore_swmem_fill_in_address which gets a non-const pointer.
  ((volatile uint32_t *)__slot)[__word] = __value;
}

_XCORE_INLINE uint32_t __xcore_swmem_evict_read_word(_XCORE_UNUSED resource_t __r, const void *__slot, unsigned __word)
{
  return ((volatile uint32_t *)__slot)[__word];
}

_XCORE_INLINE void __xcore_swmem_res_request_done(resource_t __r)
{
  asm volatile("setc res[%[__res]], %[__value]"
               : /* No outputs */
               : [__res] "r" (__r)
               , [__value] "nr" (XS1_SETC_RUN_STARTR));
}

_XCORE_INLINE void __xcore_swmem_fill_populate_slot(resource_t __r, const void *__slot, const uint32_t *__buf)
{
  // This could be faster if we used the vector unit to do the fill in one go...
 
  // We cast back to a non-const because the only valid source of the address is
  // __xcore_swmem_fill_in_address which gets a non-const pointer.
  memcpy((void *)__slot, (const void *)__buf, sizeof(uint32_t) * _XCORE_SWMEM_LINE_WORDS);
}

_XCORE_INLINE uint32_t __xcore_swmem_res_get_data(resource_t __res)
{
  register uint32_t __data;
  asm volatile("getd %[__data], res[%[__res]]"
               : [__data] "=r" (__data)
               : [__res] "r" (__res));
  return __data;
}

_XCORE_INLINE void __xcore_swmem_evict_to_buf(resource_t __res, const void *__slot, uint32_t *__buf)
{
  memcpy((void *)__buf, (const void *)__slot, sizeof(uint32_t) * _XCORE_SWMEM_LINE_WORDS);
}

#ifdef __cplusplus
}
#endif
