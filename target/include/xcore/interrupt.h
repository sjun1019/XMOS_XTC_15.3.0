// Copyright (c) 2016, XMOS Ltd, All rights reserved
#pragma once

/** \file
 *  \brief API for configuring interrupts.
 */

#include <xs1.h>
#include <xcore/_support/xcore_common.h>
#include <xcore/_support/xcore_meta_macro.h>

#ifdef __cplusplus
extern "C" {
#endif

/** \brief Mask all interrupts on this logical core.
 *
 *  Prevent any enabled triggerable_setup_interrupt_callback() functions
 *  from triggering.
 *  This has no effect on TRIGGERABLE_SETUP_EVENT_VECTOR() triggering.
 *  They can be restored by using interrupt_unmask_all().
 */
_XCORE_INLINE void interrupt_mask_all(void) _XCORE_NOTHROW
{
  asm volatile("clrsr" _XCORE_STRINGIFY(XS1_SR_IEBLE_MASK));
}

/** \brief Unmask all interrupts on this logical core.
 *
 *  Allow triggerable_setup_interrupt_callback() functions to trigger.
 *  They can be suppressed by using interrupt_mask_all().
 *  \hideinitializer
 */
_XCORE_INLINE void interrupt_unmask_all(void) _XCORE_NOTHROW
{
  asm volatile("setsr" _XCORE_STRINGIFY(XS1_SR_IEBLE_MASK));
}

#ifdef __cplusplus
}
#endif

