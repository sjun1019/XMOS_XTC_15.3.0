// Copyright (c) 2016-2020, XMOS Ltd, All rights reserved
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// Prefix for inline function defintions in headers.
// This can be defined as extern to allow building a library from the header definitions.
#ifndef _XCORE_INLINE
#define _XCORE_INLINE inline
#endif

#ifdef __cplusplus
#define _XCORE_CFUNC_EXCEPT_SPEC throw()
#else
#define _XCORE_CFUNC_EXCEPT_SPEC
#endif
#define _XCORE_NOTHROW _XCORE_CFUNC_EXCEPT_SPEC

#define _XCORE_UNUSED __attribute__((unused))

typedef unsigned resource_t;
typedef int __xcore_bool_t;

// Special directive to disable reporting of non-memory resources in xMap since resource
// requirements can't be calculated when resources can be 'leaked'.
asm (".globalresource 0x0,\"lib_xcore\",\"global\"");

#ifdef __cplusplus
}
#endif

