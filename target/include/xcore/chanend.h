// Copyright (c) 2016, XMOS Ltd, All rights reserved
#pragma once

/** \file
 *  \brief Low level channel end API
 *  \attention It is strongly recommended that higher-level 'channel' protocols are used e.g. \ref xcore/channel.h or \ref xcore/channel_streaming.h
 */

#include <stdint.h>
#include <xcore/_support/xcore_common.h>
#include <xcore/_support/xcore_chanend_impl.h>
#include <xcore/_support/xcore_resource_impl.h>

#ifdef __cplusplus
extern "C" {
#endif

/** \typedef chanend_t
 *  \brief Opaque channel end type for use in C/C++ code.
 *
 *  \attention Users must not access its raw underlying type.
 */
typedef resource_t chanend_t;


/** \brief Allocate a single chanend.
 *
 *  If there are no channel ends available the function returns 0.
 *  \note When the channel end is no longer required, chanend_free() should be called
 *  to deallocate it.
 *
 *  \return    Allocated chanend (0 if none are available)
 */
_XCORE_INLINE chanend_t chanend_alloc() _XCORE_NOTHROW
{
  return __xcore_chanend_alloc();
}

/** \brief Deallocate a single chanend.
 * 
 *  \attention The last transfer on the chanend must have been a CT_END token.
 *
 *  \param __c    chanend to free.
 *
 *  \exception  ET_ILLEGAL_RESOURCE   not an allocated chanend,
 *                                    an input/output is pending,
 *                                    or it has not received/sent a CT_END token.
 *  \exception  ET_RESOURCE_DEP       another core is actively using the chanend.
 */
_XCORE_INLINE void chanend_free(chanend_t __c) _XCORE_NOTHROW
{
  __xcore_chanend_free(__c);
}

/** \brief Set the destination of a chanend
 *
 *  \param __c    chanend to set.
 *  \param __dst  Destination chanend.
 *
 *  \exception  ET_ILLEGAL_RESOURCE   not an allocated chanend.
 *  \exception  ET_RESOURCE_DEP       another core is actively using the chanend.
*/
_XCORE_INLINE void chanend_set_dest(chanend_t __c, chanend_t __dst) _XCORE_NOTHROW
{
  __xcore_chanend_set_dest(__c, __dst);
}

_XCORE_INLINE chanend_t chanend_get_dest(chanend_t __c) _XCORE_NOTHROW
{
  return __xcore_chanend_get_dest(__c);
}

_XCORE_INLINE void chanend_out_byte(chanend_t __c, char __b) _XCORE_NOTHROW
{
  __xcore_chanend_out_byte(__c, __b);
}

_XCORE_INLINE void chanend_out_word(chanend_t __c, uint32_t __w) _XCORE_NOTHROW
{
  __xcore_chanend_out_word(__c, __w);
}

_XCORE_INLINE void chanend_out_control_token(chanend_t __c, char __ct) _XCORE_NOTHROW
{
  __xcore_chanend_out_ct(__c, __ct);
}

_XCORE_INLINE char chanend_in_byte(chanend_t __c) _XCORE_NOTHROW
{
  return __xcore_chanend_in_byte(__c);
}

_XCORE_INLINE uint32_t chanend_in_word(chanend_t __c) _XCORE_NOTHROW
{
  return __xcore_chanend_in_word(__c);
}

_XCORE_INLINE char chanend_in_control_token(chanend_t __c) _XCORE_NOTHROW
{
  return __xcore_chanend_in_ct(__c);
}

_XCORE_INLINE void chanend_check_control_token(chanend_t __c, const char __ct) _XCORE_NOTHROW
{
  __xcore_chanend_check_ct(__c, __ct);
}

_XCORE_INLINE int chanend_test_control_token_next_byte(chanend_t __c) _XCORE_NOTHROW
{
  return __xcore_chanend_test_control_token(__c);
}

_XCORE_INLINE int chanend_test_control_token_next_word(chanend_t __c) _XCORE_NOTHROW
{
  return __xcore_chanend_test_control_token_word(__c);
}

_XCORE_INLINE int chanend_test_dest_local(chanend_t __c) _XCORE_NOTHROW
{
  return __xcore_chanend_test_dest_local(__c);
}

_XCORE_INLINE void chanend_set_network(chanend_t __c, uint32_t __net) _XCORE_NOTHROW
{
  __xcore_channend_set_network(__c, __net);
}

_XCORE_INLINE unsigned chanend_get_network(chanend_t __c) _XCORE_NOTHROW
{
  return __xcore_channend_get_network(__c);
}

_XCORE_INLINE void chanend_out_end_token(resource_t __c) _XCORE_NOTHROW
{
  __xcore_chanend_out_ct(__c, XS1_CT_END);
}

_XCORE_INLINE void chanend_check_end_token(resource_t __c) _XCORE_NOTHROW
{
  __xcore_chanend_check_ct(__c, XS1_CT_END);
}

#ifdef __cplusplus
}
#endif

