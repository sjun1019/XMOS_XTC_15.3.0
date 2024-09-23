// Copyright (c) 2019-2024, XMOS Ltd, All rights reserved
#pragma once

#include <stdint.h>
#include <xs1.h>
#include <xcore/_support/xcore_common.h>
#include <xcore/_support/xcore_feature_test_impl.h>
#include <xcore/_support/xcore_resource_impl.h>

#ifdef __cplusplus
extern "C" {
#endif

// Available thread mask bits
enum {
  __xcore_thread_mode_mask_fast =  XS1_SR_FAST_MASK,

#ifdef _XCORE_HAS_HIGH_PRIORITY_THREADS
  __xcore_thread_mode_mask_high_priority = (1 << 10),
#else
  __xcore_thread_mode_mask_high_priority = 0,
#endif
};

typedef resource_t __xcore_thread_t;

void __xcore_synchronised_thread_end(void);
void __xcore_unsynchronised_thread_end(void);

_XCORE_INLINE resource_t __xcore_allocate_thread_group(void) _XCORE_NOTHROW
{
  resource_t __res;
  asm volatile("getr %[__res], %[__type]" 
               : [__res] "=r" (__res) 
               : [__type] "n" (0x3));
  return __res;
}

_XCORE_INLINE __xcore_thread_t __xcore_create_synchronised_thread(const resource_t __sync) _XCORE_NOTHROW
{
  __xcore_thread_t __xthread;
  asm volatile("getst %[__xthread], res[%[__sync]]"
               : [__xthread] "=r" (__xthread)
               : [__sync] "r" (__sync));
  return __xthread;
}

_XCORE_INLINE void __xcore_set_thread_worker(const __xcore_thread_t __xthread,
  void(* const __func)(void *)) _XCORE_NOTHROW
{
  asm volatile("init t[%[__xthread]]:pc, %[__new_pc]"
               : /* No outputs */
               : [__xthread] "r" (__xthread)
               , [__new_pc] "r" (__func));
}

_XCORE_INLINE void __xcore_set_thread_stack(const __xcore_thread_t __xthread,
                                     void * const __stack_base) _XCORE_NOTHROW
{
  asm volatile("init t[%[__xthread]]:sp, %[__new_sp]"
               : /* No outputs */
               : [__xthread] "r" (__xthread)
               , [__new_sp] "r" (__stack_base));
}

_XCORE_INLINE void __xcore_set_thread_parameter0(const __xcore_thread_t __xthread,
                                          void * const __parameter) _XCORE_NOTHROW
{
  asm volatile("set t[%[__xthread]]:r0, %[__arg]"
               : /* No outputs */
               : [__xthread] "r" (__xthread)
               , [__arg] "r" (__parameter));
}

_XCORE_INLINE void __xcore_set_thread_terminator(const __xcore_thread_t __xthread,
                                          void(* const __terminator)(void)) _XCORE_NOTHROW
{
  asm volatile("init t[%[__xthread]]:lr, %[__new_lr]"
               : /* No outputs */
               : [__xthread] "r" (__xthread)
               , [__new_lr] "r" (__terminator));
}

_XCORE_INLINE __xcore_thread_t __xcore_allocate_unsynchronised_thread(void) _XCORE_NOTHROW
{
  __xcore_thread_t __res;
  asm volatile("getr %[__res], %[__type]" 
               : [__res] "=r" (__res) 
               : [__type] "n" (0x4));
  return __res;
}

_XCORE_INLINE void __xcore_thread_group_sync_parent(const resource_t __group) _XCORE_NOTHROW
{
  asm volatile("msync res[%[__sync]]" 
               : /* No outputs */
               : [__sync] "r" (__group)
               : "memory"); // Other threads may use our variables.
}

_XCORE_INLINE void __xcore_thread_group_sync_child(void) _XCORE_NOTHROW
{
  asm volatile("ssync"
               : /* No outputs */
               : /* No inputs */
               : "memory"); // Other threads may use our variables.
}


_XCORE_INLINE void __xcore_thread_group_start(const resource_t __group) _XCORE_NOTHROW
{
  __xcore_thread_group_sync_parent(__group);
}


_XCORE_INLINE void __xcore_thread_group_join(const resource_t __group) _XCORE_NOTHROW
{
  asm volatile("mjoin res[%[__sync]]" 
               : /* No outputs */
               : [__sync] "r" (__group) 
               : "memory"); // Other threads may have clobbered our variables.
}

_XCORE_INLINE void __xcore_unsynronised_thread_start(const __xcore_thread_t __xthread) _XCORE_NOTHROW
{
  asm volatile("start t[%[__xthread]]" 
               : /* No outputs */
               : [__xthread] "r" (__xthread)
               : "memory"); // Other thread may use our variables.
}


#define _XCORE_THREAD_SETSR_ASM(_M) \
    asm volatile("setsr %[__mbits]" \
                 : /* No outputs */ \
                 : [__mbits] "n" (_M))

#define _XCORE_THREAD_CLRSR_ASM(_M) \
    asm volatile("clrsr %[__mbits]" \
                 : /* No outputs */ \
                 : [__mbits] "n" (_M))

_XCORE_INLINE void __xcore_set_local_thread_mode_bits(unsigned __mode)
{
  if (__mode == __xcore_thread_mode_mask_high_priority)
  {
    _XCORE_THREAD_SETSR_ASM(__xcore_thread_mode_mask_high_priority);
  }
  else if (__mode == __xcore_thread_mode_mask_fast)
  {
    _XCORE_THREAD_SETSR_ASM(__xcore_thread_mode_mask_fast);
  }
  else if (__mode == ( __xcore_thread_mode_mask_high_priority
                     | __xcore_thread_mode_mask_fast ))
  {
    _XCORE_THREAD_SETSR_ASM(__xcore_thread_mode_mask_high_priority |
                           __xcore_thread_mode_mask_fast);
  }
}

_XCORE_INLINE unsigned __xcore_get_local_thread_mode_bits(void)
{
  register uint32_t __sr asm("r11");
  asm volatile("getsr r11, %[__bits]"
               : "=r" (__sr)
               : [__bits] "n" ( __xcore_thread_mode_mask_high_priority
                              | __xcore_thread_mode_mask_fast ));
  return __sr;
}

_XCORE_INLINE void __xcore_clear_local_thread_mode_bits(unsigned __mode)
{
  if (__mode == __xcore_thread_mode_mask_high_priority)
  {
    _XCORE_THREAD_CLRSR_ASM(__xcore_thread_mode_mask_high_priority);
  }
  else if (__mode == __xcore_thread_mode_mask_fast)
  {
    _XCORE_THREAD_CLRSR_ASM(__xcore_thread_mode_mask_fast);
  }
  else if (__mode == ( __xcore_thread_mode_mask_high_priority
                     | __xcore_thread_mode_mask_fast ))
  {
    _XCORE_THREAD_CLRSR_ASM(__xcore_thread_mode_mask_high_priority |
                            __xcore_thread_mode_mask_fast);
  }
}

#ifdef __cplusplus
}
#endif

