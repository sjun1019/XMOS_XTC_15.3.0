// Copyright (c) 2016-2024, XMOS Ltd, All rights reserved
#pragma once

#include <xcore/_support/xcore_feature_test_impl.h>

#ifdef __cplusplus
extern "C" {
#endif


// This file contains private implementation details and is not part of the API.
// The contents may vary between releases.

#ifdef _XCORE_HAS_DUAL_ISSUE
#define _XCORE_CODE_ALIGNMENT   4
#else
#define _XCORE_CODE_ALIGNMENT   2
#endif

#ifdef _XCORE_HAS_DUAL_ISSUE
#define _XCORE_STACK_ALIGN(__n)   ((((__n)+1)/2)*2)
#else
#define _XCORE_STACK_ALIGN(__n)   (__n)
#endif

#define _XCORE_ENTSP_SIZE_STR(__n) #__n
#ifdef _XCORE_HAS_DUAL_ISSUE
#define _XCORE_ENTSP(__n)   "ENTSP_lu6 " _XCORE_ENTSP_SIZE_STR(__n)
#else
#define _XCORE_ENTSP(__n)   "entsp " _XCORE_ENTSP_SIZE_STR(__n)
#endif

#ifdef _XCORE_HAS_DUAL_ISSUE
#define _XCORE_STACK_ALIGN_REQUIREMENT 8
#define _XCORE_STACK_ALIGN_REQUIREMENT_WORDS 2
#else
#define _XCORE_STACK_ALIGN_REQUIREMENT 4
#define _XCORE_STACK_ALIGN_REQUIREMENT_WORDS 1
#endif

#ifdef _XCORE_HAS_DUAL_ISSUE
#define _XCORE_SINGLE_ISSUE   "ENTSP_lu6 0"
#else
#define _XCORE_SINGLE_ISSUE   ""
#endif

#ifdef _XCORE_HAS_SHORT_RESOURCE_EV
#define _XCORE_ENUM_ID_BASE 0x10000
#else
#define _XCORE_ENUM_ID_BASE 0
#endif

#ifdef __cplusplus
}
#endif

