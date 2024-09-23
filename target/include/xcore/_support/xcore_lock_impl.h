// Copyright (c) 2016-2020, XMOS Ltd, All rights reserved
#pragma once

// This file contains private implementation details and is not part of the API.
// The contents may vary between releases.
#include <xs1.h>
#include <xcore/_support/xcore_common.h>
#include <xcore/_support/xcore_resource_impl.h>

#ifdef __cplusplus
extern "C" {
#endif

_XCORE_INLINE resource_t __xcore_lock_alloc(void)
{
  resource_t __l;
  _XCORE_RESOURCE_ALLOC(__l, XS1_RES_TYPE_LOCK);
  return __l;
}

_XCORE_INLINE void __xcore_lock_acquire(resource_t __l)
{
  asm volatile("in %[__lock], res[%[__lock]]"
               : /* 'in' on a Lock is guaranteed input the handle, so we pretend this has no output */
               : [__lock] "r" (__l)
               : "memory");
}

_XCORE_INLINE void __xcore_lock_release(resource_t __l)
{
  asm volatile("out res[%[__lock]], %[__lock]" :: [__lock] "r" (__l) : "memory");
}

#ifdef __cplusplus
}
#endif
