// Copyright (c) 2016-2020, XMOS Ltd, All rights reserved
#pragma once

// This file contains private implementation details and is not part of the API.
// The contents may vary between releases.

#include <stdint.h>
#include <xcore/_support/xcore_common.h>
#include <xcore/_support/xcore_macros.h>
#include <xcore/_support/xcore_meta_macro.h>
#include <xcore/_support/xcore_resource_impl.h>

#ifdef __cplusplus
extern "C" {
#endif

__thread
extern int __xcore_select_clobbered;

_XCORE_INLINE void __xcore_select_disable_trigger_all(void) _XCORE_NOTHROW
{
  asm volatile("clre");
}


#define _XCORE_GUARD_NONE _XCORE_GTYPE_NONE, /* Guard expression should never be used */
#define _XCORE_GUARD_TRUE(_EXPR) _XCORE_GTYPE_TRUE, _EXPR
#define _XCORE_GUARD_FALSE(_EXPR) _XCORE_GTYPE_FALSE, _EXPR

#ifdef __cplusplus
}
#endif

