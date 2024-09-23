// Copyright (c) 2024, XMOS Ltd, All rights reserved
#pragma once

// Utility macros for creating compile-time type assertions

#include <xcore/_support/xcore_meta_macro.h>

#ifdef __cplusplus
extern "C" {
#endif

// Predicates
//   These aim to expand to a constant expression with a nonzero value if the predicate holds for
//   the given parameter. This is as opposed to directly causing a compilation failure; making
//   predicates suitable for constant evaluation contexts.

// Unconditional fail predicate
#define _XCORE_TC_PRED_FALSE(_T_) 0

// Size of the given type is no more than sizeof(int)
#define _XCORE_TC_PRED_TYPE_SIZE_LEQ_INT(_T) \
    (sizeof(_T) <= sizeof(int))

// Fails (0 expression) if the given type is not naturally aligned (i.e. its size and alignment are
// not equal).
#define _XCORE_TC_PRED_TYPE_NATURALLY_ALIGNED(_T) \
    (__alignof__(_T) == sizeof(_T))

// Size of the given paramter declarator (named) is no more than sizeof(int)
#define _XCORE_TC_PRED_ARG_SIZE_LEQ_INT(_A) \
    (sizeof(union { _A ; char __u; }) <= sizeof(int))

// Fails (0 expression) if the given paramter declarator (named) has a type which is not naturally
// aligned (i.e. its size and alignment are not equal).
#define _XCORE_TC_PRED_ARG_NATURALLY_ALIGNED(_A) \
    (__alignof__(union { _A ; char __u; }) == sizeof(union { _A ; char __u; }))

// Fails (0 expression) if the given parameter declarator (named) represents a parameter which
// definitely can't be passed in a single XCORE general purpose register.
// This is approximated by checking that the type is as small as an `int`, and naturally aligned.
// This will miss:
//   * Structs with only one member, where that member would not fail this predicate;
//   * Unions whose most aligned members are also their largest, and that member would not fail
//     this predicate;
//   * Arrays of length 1 of types which would not fail this predicate
#define _XCORE_TC_PRED_ARG_PROBABLY_PASSED_IN_REGISTER(_A) \
  (_XCORE_TC_PRED_ARG_SIZE_LEQ_INT(_A) && _XCORE_TC_PRED_ARG_NATURALLY_ALIGNED(_A))

// Like _XCORE_TC_PRED_ARG_PROBABLY_PASSED_IN_REGISTER but checks a bare typename instead
#define _XCORE_TC_PRED_TYPE_PROBABLY_RETURNED_IN_REGISTER(_T) \
  (_XCORE_TC_PRED_TYPE_SIZE_LEQ_INT(_T) && _XCORE_TC_PRED_TYPE_NATURALLY_ALIGNED(_T))

// Checks
//   These will cause a compilation failure if their conditions fail.

#define _XCORE_TC_CHECK_NAMED(_T, _PRED, _NAME) \
    typedef char _XCORE_UNIQUE_LABEL(_NAME)[1-2*!(_PRED(_T))];

#ifdef __cplusplus
}
#endif
