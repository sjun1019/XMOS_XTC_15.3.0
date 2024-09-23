// Copyright (c) 2020-2024, XMOS Ltd, All rights reserved
#pragma once

#include <xs1.h>

#ifdef __cplusplus
extern "C" {
#endif


#if !defined(__XS1B__) && !defined(__XS1C__) && !defined(__XS2A__) && !defined(__XS3A__)
#error Unsupported platform!
#endif

// Dual Issue
#if defined(__XS1B__) || defined(__XS1C__)
#else
#define _XCORE_HAS_DUAL_ISSUE
#endif

// Reference Clock
#if defined(__XS1B__) || defined(__XS1C__)
#else
#define _XCORE_HAS_REFERENCE_CLOCK
#endif

// Software Memory
#if !defined(__XS1B__) && !defined(__XS1C__) && !defined(__XS2A__)
#define _XCORE_HAS_MINICACHE
#define _XCORE_HAS_SWMEM
#endif

// High-priority threads
#if !defined(__XS1B__) && !defined(__XS1C__)
#define _XCORE_HAS_HIGH_PRIORITY_THREADS
#endif

// In XS2 onwards, OUTPW has three register operands
// OUTPWI takes two registers and an immediate (bit count)
// In XS1, OUTPW is equivalent to XS2's OUTPWI and there
// is no way to specify bit count in a reigster. We define
// _XCORE_HAS_OUTPW_WITH_REGISTERS if OUTPW takes bit count
// as a register.
#if !defined(__XS1B__) && !defined(__XS1C__)
#define _XCORE_HAS_OUTPW_WITH_REGISTERS
#endif

// On XS1, the environment vector was stored as an offset such that its range was valid RAM
// addresses only.
#if defined(__XS1B__) || defined(__XS1C__)
#define _XCORE_HAS_SHORT_RESOURCE_EV
#endif

#ifdef __cplusplus
}
#endif

