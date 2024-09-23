// Copyright (c) 2016-2020, XMOS Ltd, All rights reserved
#pragma once

// This file contains private implementation details and is not part of the API.
// The contents may vary between releases.

#include <stdint.h>
#include <xcore/_support/xcore_common.h>
#include <xcore/_support/xcore_macros.h>
//#include <xcore/assert.h>

#ifdef __cplusplus
extern "C" {
#endif

/** wrapped interrupt callback function
 *
 *  This is an opaque type returned by the INTERRUPT_CALLBACK() macro.
 *
 *  Users must not access its raw underlying type.
 */
typedef void(*__xcore_interrupt_callback_t)(void);
typedef void(*__xcore_select_callback_t)(void);

extern void __xcore_select_non_callback(void);  // Implemented in xcore_c_select.S

_XCORE_INLINE void __xcore_resource_setup_callback(resource_t __r, void *__data, void(*__func)(void), uint32_t __type) _XCORE_NOTHROW
{
#if !defined(__XS2A__)
  //xassert( ((uint32_t)__data >> 16) == 0x1 ); // && msg("On XS1 bit 16 will always be set in the data returned from an event"));
#endif
  {
    register void(* const __func_reg)(void) asm("r11") = __func; 
    asm volatile("setv res[%[__res]], %[__func_reg]"
                 : /* No outputs */
                 : [__res] "r" (__r)
                 , [__func_reg] "r" (__func_reg));
  }
  {
    register void * const __ev_reg asm("r11") = __data;
    asm volatile("setev res[%[__res]], %[__ev_reg]"
                 : /* No outputs */
                 : [__res] "r" (__r)
                 , [__ev_reg] "r" (__ev_reg));
  }
  asm volatile("setc res[%[__res]], %[__type]"
               : /* No outputs */
               : [__res] "r" (__r)
               , [__type] "r" (__type)); // Raise interrupts or events
}

_XCORE_INLINE void __xcore_resource_setup_interrupt_callback(resource_t __r, void *__data, __xcore_interrupt_callback_t __intrpt) _XCORE_NOTHROW
{
  __xcore_resource_setup_callback(__r, __data, __intrpt, 0xA);  // Raise interrupts instead of events
}

#define _XCORE_RESOURCE_ALLOC(__res, __id) asm volatile( "getr %0, %1" : "=r" (__res) : "n" (__id))

_XCORE_INLINE void __xcore_resource_free(resource_t __r) _XCORE_NOTHROW
{
  asm volatile("freer res[%0]" :: "r" (__r));
}

#define _XCORE_RESOURCE_SETCI(__res, __c) asm volatile( "setc res[%0], %1" :: "r" (__res), "n" (__c))

/** Wait for a select event to trigger.
 *
 *  This function waits for an event to trigger and then returns the value the
 *  user has registered with the resource that triggered the event.
 *
 *  \returns  The enum_id registered with the resource when events were enabled
 */
uint32_t select_wait(void) _XCORE_NOTHROW;

/** Check whether any select events have triggered, otherwise return.
 *
 *  This function tests for an event to being ready. If there is one ready then
 *  it returns the enum_id the user has registered with the resource that
 *  triggered the event. If no events are ready then returns the no_wait_id
 *  passed in by the user.
 *
 *  **select_callback_t events are handled, but are not considered 'select events'**
 *
 *  \param no_wait_id  The enum_id to return if no 'select event' is triggered
 *
 *  \returns  The enum_id registered with the resource which triggered an event
 *            or the no_wait_id passed in if no event fired
 */
uint32_t select_no_wait(uint32_t __no_wait_id) _XCORE_NOTHROW;

/** Wait for an select event from a list of resources using an ordered enable sequence
 *
 *  This function:
 *    - Starts by clearing all select events that have been configured for this core.
 *      This includes select_callback_t functions but not interrupt_callback_t functions.
 *    - Enables select events on each resource in turn so that there is a defined
 *      order in which pending events will be taken
 *
 *  **Enabled select_callback_t resources will be taken, but will not terminate
 *  the process. A user may wish to place these at the front of the list**
 *
 *  \param ids  Null-terminated list of resources to enable events on
 *
 *  \returns    The enum_id registered with the resource which triggers an event
 *
 *  \exception  ET_LOAD_STORE         invalid *ids[]* argument.
 */
uint32_t select_wait_ordered(const resource_t __ids[]) _XCORE_NOTHROW;

/** Wait for a select event from a list of resources using an ordered enable sequence
 *
 *  This function does the same as select_wait_ordered, but will return the
 *  no_wait_id if no select event fires by the end of the enabling sequence.
 *
 *  **select_callback_t events are handled, but are not considered 'select events'**
 *
 *  \param no_wait_id  The enum_id to return if no 'select event' is triggered
 *  \param ids         Null-terminated list of resources to enable events on
 *
 *  \returns  The enum_id registered with the resource which triggered an event
 *            or the no_wait_id passed in if no event fired
 *
 *  \exception  ET_LOAD_STORE         invalid *ids[]* argument.
 */
uint32_t select_no_wait_ordered(uint32_t __no_wait_id, const resource_t __ids[]) _XCORE_NOTHROW;



// Start of support for new style

_XCORE_INLINE void __xcore_resource_event_enable_unconditional(const resource_t __resource) _XCORE_NOTHROW
{
  asm volatile(
    "eeu res[%[__res]] \n"
    : /* No outputs */
    : [__res] "r"(__resource));
}

_XCORE_INLINE void __xcore_resource_event_disable_unconditional(const resource_t __resource) _XCORE_NOTHROW
{
  asm volatile(
    "edu res[%[__res]] \n"
    : /* No outputs */
    : [__res] "r"(__resource));
}

_XCORE_INLINE void __xcore_resource_event_set_enable(const resource_t __resource, const int __condition) _XCORE_NOTHROW
{
  asm volatile(
    "eet %[__cond], res[%[__res]] \n"
    : /* No outputs */
    : [__res] "r"(__resource)
    , [__cond] "r"(__condition));
}

_XCORE_INLINE void __xcore_resource_event_set_enable_inv(const resource_t __resource, const int __condition) _XCORE_NOTHROW
{
  asm volatile(
    "eef %[__cond], res[%[__res]] \n"
    : /* No outputs */
    : [__res] "r" (__resource)
    , [__cond] "r" (__condition));
}

_XCORE_INLINE void __xcore_resource_event_enable_if_true(const resource_t __resource, const int __condition) _XCORE_NOTHROW
{
  if (__builtin_constant_p(__condition))
  {
    if (__condition)
    {
      __xcore_resource_event_enable_unconditional(__resource);
    }
    else
    {
      __xcore_resource_event_disable_unconditional(__resource);
    }
  }
  else 
  {
    __xcore_resource_event_set_enable(__resource, __condition);
  }
}

_XCORE_INLINE void __xcore_resource_event_enable_if_false(const resource_t __resource, const int __condition) _XCORE_NOTHROW
{
  if (__builtin_constant_p(__condition))
  {
    if (__condition)
    {
      __xcore_resource_event_disable_unconditional(__resource);
    }
    else
    {
      __xcore_resource_event_enable_unconditional(__resource);
    }
  }
  else 
  {
    __xcore_resource_event_set_enable_inv(__resource, __condition);
  }
}

_XCORE_INLINE void __xcore_resource_register_event_vector(const resource_t __resource, void * const __vector) _XCORE_NOTHROW
{
  register void * const __vector_reg asm("r11") = __vector; 
  asm volatile (
    "setv res[%[__res]], %[__target] \n"
    : /* No outputs */ 
    : [__res] "r" (__resource)
    , [__target] "r"(__vector_reg));
}

_XCORE_INLINE void __xcore_resource_setc(resource_t __r, uint32_t __word)
{
  asm volatile ("setc res[%[__r]], %[__value]"
                : /* No outputs */
                : [__r] "r" (__r)
                , [__value] "r" (__word));
}

#define _XCORE_RESOURCE_SETCI_CASE_N_ASM(_N) \
  case _N: \
    asm volatile ("setc res[%[__r]], %[__value]" \
                  : /* No outputs */ \
                  : [__r] "r" (__r) \
                  , [__value] "n" (_N)); \
    return

_XCORE_INLINE void __xcore_resource_write_control_word(resource_t __r, uint32_t __word)
{
  if (__builtin_constant_p(__word))
  {
    switch (__word)
    {
      // Cover all SETCI legal values in the ISA
      _XCORE_RESOURCE_SETCI_CASE_N_ASM(0x0000);
      _XCORE_RESOURCE_SETCI_CASE_N_ASM(0x0008);
      _XCORE_RESOURCE_SETCI_CASE_N_ASM(0x0001);
      _XCORE_RESOURCE_SETCI_CASE_N_ASM(0x0009);
      _XCORE_RESOURCE_SETCI_CASE_N_ASM(0x0011);
      _XCORE_RESOURCE_SETCI_CASE_N_ASM(0x0019);
      _XCORE_RESOURCE_SETCI_CASE_N_ASM(0x0021);
      _XCORE_RESOURCE_SETCI_CASE_N_ASM(0x0029);
      _XCORE_RESOURCE_SETCI_CASE_N_ASM(0x0002);
      _XCORE_RESOURCE_SETCI_CASE_N_ASM(0x000a);
      _XCORE_RESOURCE_SETCI_CASE_N_ASM(0x0003);
      _XCORE_RESOURCE_SETCI_CASE_N_ASM(0x000b);
      _XCORE_RESOURCE_SETCI_CASE_N_ASM(0x0013);
      _XCORE_RESOURCE_SETCI_CASE_N_ASM(0x0007);
      _XCORE_RESOURCE_SETCI_CASE_N_ASM(0x000f);
      _XCORE_RESOURCE_SETCI_CASE_N_ASM(0x0017);
      _XCORE_RESOURCE_SETCI_CASE_N_ASM(0x1007);
      _XCORE_RESOURCE_SETCI_CASE_N_ASM(0x100f);
      _XCORE_RESOURCE_SETCI_CASE_N_ASM(0x2007);
      _XCORE_RESOURCE_SETCI_CASE_N_ASM(0x200f);
      _XCORE_RESOURCE_SETCI_CASE_N_ASM(0x3007);
      _XCORE_RESOURCE_SETCI_CASE_N_ASM(0x300f);
      _XCORE_RESOURCE_SETCI_CASE_N_ASM(0x3017);
      _XCORE_RESOURCE_SETCI_CASE_N_ASM(0x4007);
      _XCORE_RESOURCE_SETCI_CASE_N_ASM(0x400f);
      _XCORE_RESOURCE_SETCI_CASE_N_ASM(0x5007);
      _XCORE_RESOURCE_SETCI_CASE_N_ASM(0x500f);
      _XCORE_RESOURCE_SETCI_CASE_N_ASM(0x5017);
      _XCORE_RESOURCE_SETCI_CASE_N_ASM(0x6007);
      _XCORE_RESOURCE_SETCI_CASE_N_ASM(0x600f);
      default: break;
    }
  }
  __xcore_resource_setc(__r, __word);
}

#ifdef __cplusplus
}
#endif

