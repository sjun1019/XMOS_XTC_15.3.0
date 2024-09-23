// Copyright (c) 2016-2020, XMOS Ltd, All rights reserved
#pragma once
// This file contains private implementation details and is not part of the API.
// The contents may vary between releases.

#include <stdint.h>
#include <xcore/_support/xcore_common.h>

#ifdef __cplusplus
extern "C" {
#endif


_XCORE_INLINE void __xcore_clock_set_ready_src(resource_t __clk, resource_t __ready_source)
{
  asm volatile("setrdy res[%0], %1" :: "r" (__clk), "r" (__ready_source));
}

_XCORE_INLINE void __xcore_clock_set_source_port(resource_t __clk, resource_t __p)
{
  asm volatile("setclk res[%0], %1" :: "r" (__clk), "r" (__p));
}

_XCORE_INLINE void __xcore_clock_set_source_clk_ref(resource_t __clk)
{
  asm volatile("setclk res[%0], %1" :: "r" (__clk), "r" (XS1_CLK_REF));
}

_XCORE_INLINE void __xcore_clock_set_source_clk_xcore(resource_t __clk)
{
  asm volatile("setclk res[%0], %1" :: "r" (__clk), "r" (XS1_CLK_XCORE));
}

_XCORE_INLINE void __xcore_clock_set_divide(resource_t __clk, uint8_t __divide)
{
  asm volatile("setd res[%0], %1" :: "r" (__clk), "r" (__divide));
}

#ifdef __cplusplus
}
#endif

