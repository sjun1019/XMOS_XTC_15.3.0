// Copyright (c) 2019-2024, XMOS Ltd, All rights reserved
#pragma once

#include <xcore/_support/xcore_common.h>
#include <xcore/_support/xcore_select_impl_common.h>

#ifdef __cplusplus
extern "C" {
#endif


#define _XCORE_SELECT_RES_HANDLER_SETUP_I(_RES, _LABEL, ...) __xcore_resource_register_event_vector(_RES, &&_LABEL);

#define _XCORE_SELECT_RES_HANDLER_SETUP(_PACK) _XCORE_PSHIM(_XCORE_SELECT_RES_HANDLER_SETUP_I, _PACK)


#define _XCORE_SELECT_RES_ENABLER_ONEOFF__XCORE_GTYPE_NONE(_RES, _EXPR_) __xcore_resource_event_enable_unconditional(_RES);

#define _XCORE_SELECT_RES_ENABLER_ONEOFF__XCORE_GTYPE_TRUE(...) /* None */
#define _XCORE_SELECT_RES_ENABLER_ONEOFF__XCORE_GTYPE_FALSE(...) /* None */

#define _XCORE_SELECT_RES_ENABLER_REPEAT__XCORE_GTYPE_NONE(...) /* None */

#define _XCORE_SELECT_RES_ENABLER_REPEAT__XCORE_GTYPE_TRUE(_RES, _COND) __xcore_resource_event_enable_if_true(_RES, _COND);
#define _XCORE_SELECT_RES_ENABLER_REPEAT__XCORE_GTYPE_FALSE(_RES, _COND) __xcore_resource_event_enable_if_false(_RES, _COND);

#define _XCORE_SELECT_RES_ENABLER_ONEOFF_NAME(_GTYPE)  _XCORE_SELECT_RES_ENABLER_ONEOFF_ ## _GTYPE
#define _XCORE_SELECT_RES_ENABLER_ONEOFF_I(_RES, _LABEL_, _GTYPE, _EXPR) _XCORE_SELECT_RES_ENABLER_ONEOFF_NAME(_GTYPE)(_RES, _EXPR)
#define _XCORE_SELECT_RES_ENABLER_ONEOFF(_PACK) _XCORE_PSHIM(_XCORE_SELECT_RES_ENABLER_ONEOFF_I, _PACK)

#define _XCORE_SELECT_RES_ENABLER_REPEAT_NAME(_GTYPE)  _XCORE_SELECT_RES_ENABLER_REPEAT_ ## _GTYPE
#define _XCORE_SELECT_RES_ENABLER_REPEAT_I(_RES, _LABEL_, _GTYPE, _EXPR) _XCORE_SELECT_RES_ENABLER_REPEAT_NAME(_GTYPE)(_RES, _EXPR)
#define _XCORE_SELECT_RES_ENABLER_REPEAT(_PACK) _XCORE_PSHIM(_XCORE_SELECT_RES_ENABLER_REPEAT_I, _PACK)

#define _XCORE_FILTER_SEL_DEFAULT__XCORE_SEL_RES(_PACK_)
#define _XCORE_FILTER_SEL_DEFAULT__XCORE_SEL_DEFAULT(_PACK) _PACK,
#define _XCORE_FILTER_SEL_RES__XCORE_SEL_RES(_PACK) , _PACK
#define _XCORE_FILTER_SEL_RES__XCORE_SEL_DEFAULT(_PACK)


#define _XCORE_FILTER_SEL_DEFAULT_NAME(_SUFF) _XCORE_FILTER_SEL_DEFAULT_ ## _SUFF
#define _XCORE_FILTER_SEL_DEFAULT_I(_SUFF, _PACK) _XCORE_FILTER_SEL_DEFAULT_NAME(_SUFF)(_PACK)
#define _XCORE_SELECT_RES_FILTER_DEFAULT_I(_PACK) _XCORE_PSHIM(_XCORE_FILTER_SEL_DEFAULT_I, _PACK)

#define _XCORE_FILTER_SEL_RES_NAME(_SUFF) _XCORE_FILTER_SEL_RES_ ## _SUFF 
#define _XCORE_FILTER_SEL_RES_I(_SUFF, _PACK) _XCORE_FILTER_SEL_RES_NAME(_SUFF)(_PACK)
#define _XCORE_SELECT_RES_FILTER_RES_I(_PACK) _XCORE_PSHIM(_XCORE_FILTER_SEL_RES_I, _PACK)

#define _XCORE_SELECT_DEFAULT_DEFAULT (, _XCORE_NO_DEFAULT_CASE, )
#define _XCORE_SELECT_RES_FILTER_DEFAULT(...) _XCORE_SHIM(_XCORE_HEAD, _XCORE_APPLY_NOSEP(_XCORE_SELECT_RES_FILTER_DEFAULT_I, __VA_ARGS__) _XCORE_SELECT_DEFAULT_DEFAULT)
#define _XCORE_SELECT_RES_FILTER_RES(...) _XCORE_SHIM(_XCORE_TAIL, _XCORE_APPLY_NOSEP(_XCORE_SELECT_RES_FILTER_RES_I, __VA_ARGS__))


#define _XCORE_LABEL_I(_VAL_, _LABEL, ...) _LABEL
#define _XCORE_LABEL(_PACK) _XCORE_PSHIM(_XCORE_LABEL_I, _PACK)


#define _XCORE_SELECT_TAKE_EVENT_NONBLOCKING(...) \
  asm goto("setsr %[__mask] \n" \
           "clrsr %[__mask] \n" \
           : /* Can't have outputs */ \
           : [__mask] "n" (XS1_SR_EEBLE_MASK) \
           : /* No clobbers */ \
           : __VA_ARGS__)

#define _XCORE_SELECT_WAIT_HANDLER__XCORE_GTYPE_NONE(_COND_, _LABEL, ...) \
  _XCORE_SELECT_TAKE_EVENT_NONBLOCKING(__VA_ARGS__); \
  goto _LABEL;


#define _XCORE_SELECT_WAIT_HANDLER__XCORE_NO_DEFAULT_CASE(_COND_, LABEL_, ...) \
  asm goto("waiteu" \
    : /* Can't have outputs */ : /* No inputs */ : /* No clobbers */ \
    : __VA_ARGS__); \
  __builtin_unreachable();


#define _XCORE_SELECT_WAIT_HANDLER__XCORE_GTYPE_TRUE(_COND, _LABEL, ...) \
  { \
    const int __xcore_wait_cond = _COND; \
    if (__builtin_constant_p(__xcore_wait_cond)) { \
      if (__xcore_wait_cond) { _XCORE_SELECT_TAKE_EVENT_NONBLOCKING(__VA_ARGS__); goto _LABEL; } \
      else { _XCORE_SELECT_WAIT_HANDLER__XCORE_NO_DEFAULT_CASE(,, __VA_ARGS__) } \
    } else { \
      _XCORE_SELECT_TAKE_EVENT_NONBLOCKING(__VA_ARGS__); \
      asm goto("waitef %[__cond]" \
        : /* Can't have outputs */ \
        : [__cond] "r"(__xcore_wait_cond) \
        : /* No clobbers */ \
        : __VA_ARGS__); \
      __builtin_assume(__xcore_wait_cond); \
      goto _LABEL; \
    } \
  }

#define _XCORE_SELECT_WAIT_HANDLER__XCORE_GTYPE_FALSE(_COND, _LABEL, ...) \
  { \
    const int __xcore_wait_cond = _COND; \
    if (__builtin_constant_p(__xcore_wait_cond)) { \
      if (__xcore_wait_cond) { _XCORE_SELECT_WAIT_HANDLER__XCORE_NO_DEFAULT_CASE(,, __VA_ARGS__); } \
      else { _XCORE_SELECT_TAKE_EVENT_NONBLOCKING(__VA_ARGS__); goto _LABEL; } \
    } else { \
      _XCORE_SELECT_TAKE_EVENT_NONBLOCKING(__VA_ARGS__); \
      asm goto("waitet %[__cond]" \
        : /* Can't have outputs */ \
        : [__cond] "r"(__xcore_wait_cond) \
        : /* No clobbers */ \
        : __VA_ARGS__); \
      __builtin_assume(!__xcore_wait_cond); \
      goto _LABEL; \
    } \
  }

#define _XCORE_SELECT_WAIT_HANDLER_NAME(_SUFF) _XCORE_SELECT_WAIT_HANDLER_ ## _SUFF
#define _XCORE_SELECT_WAIT_HANDLER_I(_LABEL, _TYPE, _COND, ...) _XCORE_SELECT_WAIT_HANDLER_NAME(_TYPE)(_COND, _LABEL, __VA_ARGS__)
#define _XCORE_SELECT_WAIT_HANDLER(_DEFAULT_PACK, ...) _XCORE_PSHIM(_XCORE_SELECT_WAIT_HANDLER_I, _DEFAULT_PACK, __VA_ARGS__)

#define _XCORE_SELECT_RES_II(_LNAME, _DEFAULT_PACK, ...) \
  switch (0) \
    for(int __xcore_select_local_clobber_level; 0;) \
      for (const void *__xcore_select_reset, *__xcore_select_noreset;;) \
        if (1) \
  { \
    if (__builtin_expect(__xcore_select_clobbered != __xcore_select_local_clobber_level, 0)) \
    { \
    _LNAME ## __reset: \
      __xcore_select_clobbered = __xcore_select_local_clobber_level; \
      __xcore_select_disable_trigger_all(); \
      _XCORE_APPLY_NOSEP(_XCORE_SELECT_RES_HANDLER_SETUP, __VA_ARGS__) \
      _XCORE_APPLY_NOSEP(_XCORE_SELECT_RES_ENABLER_ONEOFF, __VA_ARGS__) \
    } \
  _LNAME ## __noreset: \
    __builtin_assume(__xcore_select_clobbered == __xcore_select_local_clobber_level); \
    _XCORE_APPLY_NOSEP(_XCORE_SELECT_RES_ENABLER_REPEAT, __VA_ARGS__) \
    _XCORE_SELECT_WAIT_HANDLER(_DEFAULT_PACK, _XCORE_APPLY(_XCORE_LABEL, __VA_ARGS__)) \
  } \
  else if (0) \
  { \
  case 0: \
  default: \
    __xcore_select_reset = &&_LNAME ## __reset; \
    __xcore_select_noreset = &&_LNAME ## __noreset; \
    __xcore_select_local_clobber_level = __xcore_select_clobbered + 1; \
    goto _LNAME ## __reset; \
  } \
  else

#define _XCORE_SELECT_RES_I(_LNAME, _DEFAULT_PACK, ...) _XCORE_SELECT_RES_II(_LNAME, _DEFAULT_PACK, __VA_ARGS__)
#define _XCORE_SELECT_RES(...) _XCORE_SELECT_RES_I(_XCORE_UNIQUE_LABEL(__xcore_sel_reset), _XCORE_SELECT_RES_FILTER_DEFAULT(__VA_ARGS__), _XCORE_SELECT_RES_FILTER_RES(__VA_ARGS__))

#define _XCORE_SELECT_RES_ENABLER_ORDERED__XCORE_GTYPE_TRUE(_RES, _EXPR, ...) \
  { \
    register const int __xcore_enable_cond = _EXPR; \
    __xcore_resource_event_enable_if_true(_RES, __xcore_enable_cond); \
    /*if (__xcore_enable_cond) { _XCORE_SELECT_TAKE_EVENT_NONBLOCKING(__VA_ARGS__); } */\
    _XCORE_SELECT_TAKE_EVENT_NONBLOCKING(__VA_ARGS__); \
  }

#define _XCORE_SELECT_RES_ENABLER_ORDERED__XCORE_GTYPE_FALSE(_RES, _EXPR, ...) \
  { \
    register const int __xcore_enable_cond = _EXPR; \
    __xcore_resource_event_enable_if_false(_RES, __xcore_enable_cond); \
    /* if (!__xcore_enable_cond) { _XCORE_SELECT_TAKE_EVENT_NONBLOCKING(__VA_ARGS__); } */\
    _XCORE_SELECT_TAKE_EVENT_NONBLOCKING(__VA_ARGS__); \
  }

#define _XCORE_SELECT_RES_ENABLER_ORDERED__XCORE_GTYPE_NONE(_RES, _EXPR_, ...) \
  __xcore_resource_event_enable_unconditional(_RES); \
  _XCORE_SELECT_TAKE_EVENT_NONBLOCKING(__VA_ARGS__);


#define _XCORE_SELECT_RES_ENABLER_ORDERED_NAME(_GTYPE)  _XCORE_SELECT_RES_ENABLER_ORDERED_ ## _GTYPE
#define _XCORE_SELECT_RES_ENABLER_ORDERED_III(_RES, _LABEL_, _GTYPE, _EXPR, ...) _XCORE_SELECT_RES_ENABLER_ORDERED_NAME(_GTYPE)(_RES, _EXPR, __VA_ARGS__)
#define _XCORE_SELECT_RES_ENABLER_ORDERED_II(...) _XCORE_SELECT_RES_ENABLER_ORDERED_III(__VA_ARGS__)
#define _XCORE_SELECT_RES_ENABLER_ORDERED_I(_LABELS, _PACK) _XCORE_SELECT_RES_ENABLER_ORDERED_II(_XCORE_UNPACK(_XCORE_PACK_JOIN(_PACK , _LABELS)))
#define _XCORE_SELECT_RES_ENABLER_ORDERED(_PACK) _XCORE_SELECT_RES_ENABLER_ORDERED_I _PACK

#define _XCORE_SELECT_RES_ORDERED_II(_LNAME, _DEFAULT_PACK, _LABELS, ...) \
  switch (0) \
    for(int __xcore_select_local_clobber_level; 0;) \
      for (const void *__xcore_select_reset, *__xcore_select_noreset;;) \
        if (1) \
  { \
    if (__builtin_expect(__xcore_select_clobbered != __xcore_select_local_clobber_level, 0)) \
    { \
    _LNAME ## __reset: \
      __xcore_select_clobbered = __xcore_select_local_clobber_level; \
      _XCORE_APPLY_NOSEP(_XCORE_SELECT_RES_HANDLER_SETUP, __VA_ARGS__) \
    } \
  _LNAME ## __noreset: \
    __builtin_assume(__xcore_select_clobbered == __xcore_select_local_clobber_level); \
    __xcore_select_disable_trigger_all(); \
    _XCORE_SHIM(_XCORE_APPLY_NOSEP, _XCORE_SELECT_RES_ENABLER_ORDERED, _XCORE_I(_XCORE_TAG(_LABELS, __VA_ARGS__))) \
    _XCORE_SELECT_WAIT_HANDLER(_DEFAULT_PACK, _XCORE_APPLY(_XCORE_LABEL, __VA_ARGS__)) \
  } \
  else if (0) \
  {  \
  case 0: \
  default: \
    __xcore_select_reset = &&_LNAME ## __reset; \
    __xcore_select_noreset = &&_LNAME ## __noreset; \
    __xcore_select_local_clobber_level = __xcore_select_clobbered + 1; \
    goto _LNAME ## __reset; \
  } \
  else 

#define _XCORE_SELECT_RES_ORDERED_I(_LNAME, _DEFAULT_PACK, ...) \
    _XCORE_SELECT_RES_ORDERED_II(_LNAME, _DEFAULT_PACK, (_XCORE_APPLY(_XCORE_LABEL, __VA_ARGS__)), __VA_ARGS__)

#define _XCORE_SELECT_RES_ORDERED(...) \
  _XCORE_SELECT_RES_ORDERED_I( \
    _XCORE_UNIQUE_LABEL(__xcore_sel_reset), \
    _XCORE_SELECT_RES_FILTER_DEFAULT(__VA_ARGS__), \
    _XCORE_SELECT_RES_FILTER_RES(__VA_ARGS__))


#define _XCORE_SELECT_RESET_I do { goto* __xcore_select_reset; } while (0)
#define _XCORE_CONTINUE_NO_RESET_I do { goto* __xcore_select_noreset; } while (0)

#define _XCORE_CASE_RES(...) (_XCORE_SEL_RES, (__VA_ARGS__))
#define _XCORE_CASE_DEFAULT(...) (_XCORE_SEL_DEFAULT, (__VA_ARGS__))


#ifdef __cplusplus
}
#endif

