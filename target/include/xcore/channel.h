// Copyright (c) 2016-2020, XMOS Ltd, All rights reserved
#pragma once

/** \file
 *  \brief API for channel communications
 */

#include <stdint.h>
#include <stddef.h>
#include <xcore/_support/xcore_common.h>
#include <xcore/chanend.h>
#include <xcore/channel_streaming.h>

#ifdef __cplusplus
extern "C" {
#endif

/** \typedef channel_t
 *  \brief Helper type for passing around both ends of a channel.
 */
typedef streaming_channel_t channel_t;

/** \brief Allocates a channel by allocating two hardware chan-ends and joining them.
 *
 *  If there are not enough chan-ends available the function returns a
 *  channel_t with both fields set to 0.
 *  \note When the channel_t is no longer required, chan_free() should be
 *  called to deallocate it.
 *
 *  \warning The chan-ends must be accessed on the same tile.
 *
 *  \return     The channel_t (both fields will be 0 if allocation was not possible)
 */
_XCORE_INLINE channel_t chan_alloc() _XCORE_NOTHROW
{
  return (channel_t)s_chan_alloc();
}

/** \brief Deallocate a channel by freeing its constituent chan-ends.
 *
 *  \param __c  channel_t to free
 *
 *  \exception  ET_ILLEGAL_RESOURCE   not an allocated chan-end,
 *                                    or channel handshaking corrupted.
 *  \exception  ET_RESOURCE_DEP       another core is actively using the chanend.
 */
_XCORE_INLINE void chan_free(channel_t __c) _XCORE_NOTHROW
{
  // Not implemented in terms of s_chan_free() as we have already hand-shook a CT_END.
  chanend_free(__c.end_a);
  chanend_free(__c.end_b);
}

/** \brief Output a word over a channel.
 *
 *  \param __c    The chan-end
 *  \param __data The word to be output
 *
 *  \exception  ET_LINK_ERROR         chan-end destination is not set.
 *  \exception  ET_ILLEGAL_RESOURCE   not an allocated chan-end,
 *                                    or channel handshaking corrupted.
 *  \exception  ET_RESOURCE_DEP       another core is actively using the chan-end.
 */
_XCORE_INLINE void chan_out_word(chanend_t __c, uint32_t __data) _XCORE_NOTHROW
{
  chanend_out_end_token(__c);
  chanend_check_end_token(__c);
  chanend_out_word(__c, __data);
  chanend_out_end_token(__c);
  chanend_check_end_token(__c);
}

/** \brief Output a byte over a channel.
 *
 *  \param __c    The chan-end
 *  \param __data The byte to be output
 *
 *  \exception  ET_LINK_ERROR         chan-end destination is not set.
 *  \exception  ET_ILLEGAL_RESOURCE   not an allocated chan-end,
 *                                    or channel handshaking corrupted.
 *  \exception  ET_RESOURCE_DEP       another core is actively using the chan-end.
 */
_XCORE_INLINE void chan_out_byte(chanend_t __c, uint8_t __data) _XCORE_NOTHROW
{
  chanend_out_end_token(__c);
  chanend_check_end_token(__c);
  chanend_out_byte(__c, __data);
  chanend_out_end_token(__c);
  chanend_check_end_token(__c);
}

/** \brief Output a block of data over a channel.
 *
 *  \param __c        The chan-end
 *  \param[in] __buf  A pointer to the buffer containing the data to send
 *  \param __n        The number of words to send
 *
 *  \exception  ET_LINK_ERROR         chan-end destination is not set.
 *  \exception  ET_ILLEGAL_RESOURCE   not an allocated chan-end,
 *                                    or channel handshaking corrupted.
 *  \exception  ET_RESOURCE_DEP       another core is actively using the chan-end.
 *  \exception  ET_LOAD_STORE         invalid \a buf[] argument.
 */
_XCORE_INLINE void chan_out_buf_word(chanend_t __c, const uint32_t __buf[], size_t __n) _XCORE_NOTHROW
{
  chanend_out_end_token(__c);
  chanend_check_end_token(__c);
  for (size_t __i = 0; __i < __n; __i++)
  {
    chanend_out_word(__c, __buf[__i]);
  }
  chanend_out_end_token(__c);
  chanend_check_end_token(__c);
}

/** \brief Output a block of data over a channel.
 *
 *  \param __c        The chan-end
 *  \param[in] __buf  A pointer to the buffer containing the data to send
 *  \param __n        The number of bytes to send
 *
 *  \exception  ET_LINK_ERROR         chan-end destination is not set.
 *  \exception  ET_ILLEGAL_RESOURCE   not an allocated chan-end,
 *                                    or channel handshaking corrupted.
 *  \exception  ET_RESOURCE_DEP       another core is actively using the chan-end.
 *  \exception  ET_LOAD_STORE         invalid \a buf[] argument.
 */
_XCORE_INLINE void chan_out_buf_byte(chanend_t __c, const uint8_t __buf[], size_t __n) _XCORE_NOTHROW
{
  chanend_out_end_token(__c);
  chanend_check_end_token(__c);
  for (size_t __i = 0; __i < __n; __i++)
  {
    chanend_out_byte(__c, __buf[__i]);
  }
  chanend_out_end_token(__c);
  chanend_check_end_token(__c);
}

/** \brief Input a word from a channel.
 *
 *  \param __c    The chan-end
 *  \return     The word which was read from the channel
 *
 *  \exception  ET_ILLEGAL_RESOURCE   not an allocated chan-end,
 *                                    or channel handshaking corrupted.
 *  \exception  ET_RESOURCE_DEP       another core is actively using the chan-end.
 *  \exception  ET_LOAD_STORE         invalid *\*data* argument.
 */
_XCORE_INLINE uint32_t chan_in_word(chanend_t __c) _XCORE_NOTHROW
{
  chanend_check_end_token(__c);
  chanend_out_end_token(__c);
  uint32_t __data = chanend_in_word(__c);
  chanend_check_end_token(__c);
  chanend_out_end_token(__c);
  return __data;
}

/** \brief Input a byte from a channel.
 *
 *  \param __c    The chan-end
 *  \return     The byte which was read from the channel
 *
 *  \exception  ET_ILLEGAL_RESOURCE   not an allocated chan-end,
 *                                    or channel handshaking corrupted.
 *  \exception  ET_RESOURCE_DEP       another core is actively using the chan-end.
 *  \exception  ET_LOAD_STORE         invalid *\*data* argument.
 */
_XCORE_INLINE uint8_t chan_in_byte(chanend_t __c) _XCORE_NOTHROW
{
  chanend_check_end_token(__c);
  chanend_out_end_token(__c);
  uint8_t __data = chanend_in_byte(__c);
  chanend_check_end_token(__c);
  chanend_out_end_token(__c);
  return __data;
}

/** \brief Input a block of data from a channel.
 *
 *  \param __c    The chan-end
 *  \param[out] __buf  A pointer to the memory region to fill
 *  \param __n    The number of words to receive
 *
 *  \exception  ET_ILLEGAL_RESOURCE   not an allocated chan-end,
 *                                    or channel handshaking corrupted.
 *  \exception  ET_RESOURCE_DEP       another core is actively using the chan-end.
 *  \exception  ET_LOAD_STORE         invalid *buf[]* argument.
 */
_XCORE_INLINE void chan_in_buf_word(chanend_t __c, uint32_t __buf[], size_t __n) _XCORE_NOTHROW
{
  chanend_check_end_token(__c);
  chanend_out_end_token(__c);
  for (size_t __i = 0; __i < __n; __i++)
  {
    __buf[__i] = chanend_in_word(__c);
  }
  chanend_check_end_token(__c);
  chanend_out_end_token(__c);
}

/** \brief Input a block of data from a channel.
 *
 *  \param __c         The chan-end
 *  \param[out] __buf  A pointer to the memory region to fill
 *  \param __n         The number of bytes to receive
 *
 *  \exception  ET_ILLEGAL_RESOURCE   not an allocated chan-end,
 *                                    or channel handshaking corrupted.
 *  \exception  ET_RESOURCE_DEP       another core is actively using the chan-end.
 *  \exception  ET_LOAD_STORE         invalid *buf[]* argument.
 */
_XCORE_INLINE void chan_in_buf_byte(chanend_t __c, uint8_t __buf[], size_t __n) _XCORE_NOTHROW
{
  chanend_check_end_token(__c);
  chanend_out_end_token(__c);
  for (size_t __i = 0; __i < __n; __i++)
  {
    __buf[__i] = chanend_in_byte(__c);
  }
  chanend_check_end_token(__c);
  chanend_out_end_token(__c);
}

#ifdef __cplusplus
}
#endif

