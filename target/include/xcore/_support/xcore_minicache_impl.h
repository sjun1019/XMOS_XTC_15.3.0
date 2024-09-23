// Copyright (c) 2016-2020, XMOS Ltd, All rights reserved
#pragma once

#include <xcore/_support/xcore_feature_test_impl.h>

#ifndef _XCORE_HAS_MINICACHE
#error Minicache not available on this platform
#endif

#include <stdint.h>
#include <xcore/_support/xcore_common.h>

#ifdef __cplusplus
extern "C" {
#endif

_XCORE_INLINE void __xcore_minicache_prefetch(void *__address_)
{
  const register unsigned long __addr asm("r11") = (unsigned long)__address_;
  asm volatile("prefetch %[__addr]"
               : /* No outputs */
               : [__addr] "r" (__addr));
}

_XCORE_INLINE void __xcore_minicache_flush(void)
{
  asm volatile("flush");
}

_XCORE_INLINE void __xcore_minicache_invalidate(void)
{
  // This clobbers memory as invalidating a dirty line means its
  // value will effetively change when it is next filled.
  asm volatile("invalidate"
               : /* No outputs*/
               : /* No inputs */
               : "memory");
}

#ifdef __cplusplus
}
#endif
