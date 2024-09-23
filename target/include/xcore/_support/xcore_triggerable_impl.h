// Copyright (c) 2020, XMOS Ltd, All rights reserved

#include <xs1.h>

#ifdef __cplusplus
extern "C" {
#endif


#if defined(__XMOS_LEGACY_C_CPP_COMPILER__) || defined(NO_XCLANG_ADDITIONS)

#include <xcore/_support/xcore_resource_impl.h>

#define _XCORE_TRIGGERABLE_SETUP_EVENT_VECTOR(_RESOURCE, _LABEL) \
  do { __xcore_resource_setup_callback((_RESOURCE), (&&_LABEL), __xcore_select_non_callback, 0x2); } while (0)

#define _XCORE_TRIGGERABLE_WAIT_EVENT(...) \
  do { goto* (void*)select_wait(); } while(0)

#define _XCORE_TRIGGERABLE_TAKE_EVENT(...) \
  do { \
    void * const __xmm_triggerable_vector = select_no_wait(NULL); \
    if (__xmm_triggerable_vector != NULL) { \
      goto* __xmm_triggerable_vector; \
    } \
   } while(0)

#else

#define _XCORE_TRIGGERABLE_SETUP_EVENT_VECTOR(_RESOURCE, _LABEL) \
  do { __xcore_resource_register_event_vector((_RESOURCE), (&&_LABEL)); } while (0)

#define _XCORE_TRIGGERABLE_WAIT_EVENT(_LABELS...) \
  do { \
    asm goto("waiteu" \
      : /* Can't have outputs */ \
      : /* No inputs */ \
      : /* No clobbers */ \
      : _LABELS); \
    __builtin_unreachable(); \
  } while (0)

#define _XCORE_TRIGGERABLE_TAKE_EVENT(_LABELS...) \
  do { \
    asm goto("setsr %[__mask] \n" \
             "clrsr %[__mask] \n" \
             : /* Can't have outputs */ \
             : [__mask] "n" (XS1_SR_EEBLE_MASK) \
             : /* No clobbers */ \
             : _LABELS); \
  } while (0)

#endif

_XCORE_INLINE void __triggerable_disable_all(void) _XCORE_NOTHROW
{
  asm volatile("clre");
}

#ifdef __cplusplus
}
#endif

