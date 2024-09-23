// Copyright (c) 2016-2020, XMOS Ltd, All rights reserved
#pragma once

/** \file
 *  \brief Streaming channel API
 */

#include <stdint.h>
#include <stddef.h>
#include <xcore/_support/xcore_common.h>
#include <xcore/chanend.h>

#ifdef __cplusplus
extern "C" {
#endif

/** \typedef streaming_channel_t
 *  \brief Helper type for passing around both ends of a streaming channel.
 */
typedef struct {
  chanend_t end_a;
  chanend_t end_b;
} streaming_channel_t;

/** \brief Allocate a streaming channel by allocating two hardware chan-ends and joining them.
 *
 *  If there are not enough chan-ends available the function returns a
 *  streaming_channel_t with both fields set to 0.
 *  \note When the streaming_channel_t is no longer required, s_chan_free() should be
 *  called to deallocate it.
 *
 *  \attention The chan-ends must be accessed on the same tile
 *
 *  \return    streaming_channel_t variable holding the two initialised and
 *             joined chan-ends or 0s.
 */
_XCORE_INLINE streaming_channel_t s_chan_alloc() _XCORE_NOTHROW
{
  streaming_channel_t __c;

  if ((__c.end_a = chanend_alloc()))
  {
    if ((__c.end_b = chanend_alloc()))
    {
      // exception safe calls to __xcore_s_chanend_set_dest()
      chanend_set_dest(__c.end_a, __c.end_b);
      chanend_set_dest(__c.end_b, __c.end_a);
    }
    else
    {
      chanend_free(__c.end_a);
      __c.end_a = 0;
      __c.end_b = 0;
    }
  }
  else
  {
    __c.end_a = 0;
    __c.end_b = 0;
  }

  return __c;
}

/** \brief Deallocate a streaming_channel_t by freeing its two hardware chan-ends.
 *
 *  \param __c    streaming_channel_t to free.
 *
 *  \exception  ET_LINK_ERROR         a chan-end destination is not set.
 *  \exception  ET_ILLEGAL_RESOURCE   not an allocated channel,
 *                                    or an input/output is pending.
 *  \exception  ET_RESOURCE_DEP       another core is actively using the channel.
 */
_XCORE_INLINE void s_chan_free(streaming_channel_t __c) _XCORE_NOTHROW
{
  chanend_out_end_token(__c.end_a);
  chanend_out_end_token(__c.end_b);
  chanend_check_end_token(__c.end_a);
  chanend_check_end_token(__c.end_b);
  chanend_free(__c.end_a);
  chanend_free(__c.end_b);
}

/** \brief Output a word over a streaming_channel_t.
 *
 *  \param __c    The streaming chan-end
 *  \param __data The word to be output
 *
 *  \exception  ET_LINK_ERROR         chan-end destination is not set.
 *  \exception  ET_ILLEGAL_RESOURCE   not an allocated chan-end.
 *  \exception  ET_RESOURCE_DEP       another core is actively using the chan-end.
 */
_XCORE_INLINE void s_chan_out_word(chanend_t __c, uint32_t __data) _XCORE_NOTHROW
{
  chanend_out_word(__c, __data);
}

/** \brief Output an byte over a streaming_channel_t.
 *
 *  \param __c    The streaming chan-end
 *  \param __data The byte to be output
 *
 *  \exception  ET_LINK_ERROR         chan-end destination is not set.
 *  \exception  ET_ILLEGAL_RESOURCE   not an allocated chan-end.
 *  \exception  ET_RESOURCE_DEP       another core is actively using the chan-end.
 */
_XCORE_INLINE void s_chan_out_byte(chanend_t __c, uint8_t __data) _XCORE_NOTHROW
{
  chanend_out_byte(__c, __data);
}

/** \brief Output a block of data over a streaming_channel_t.
 *
 *  \param __c    The streaming chan-end
 *  \param __buf  A pointer to the buffer containing the data to send
 *  \param __n    The number of words to send
 *
 *  \exception  ET_LINK_ERROR         chan-end destination is not set.
 *  \exception  ET_ILLEGAL_RESOURCE   not an allocated chan-end.
 *  \exception  ET_RESOURCE_DEP       another core is actively using the chan-end.
 *  \exception  ET_LOAD_STORE         invalid *buf[]* argument.
 */
_XCORE_INLINE void s_chan_out_buf_word(chanend_t __c, const uint32_t __buf[], size_t __n) _XCORE_NOTHROW
{
  for (size_t __i = 0; __i < __n; __i++)
  {
    chanend_out_word(__c, __buf[__i]);
  }
}

/** \brief Output a block of data over a streaming_channel_t.
 *
 *  \param __c    The streaming chan-end
 *  \param __buf  A pointer to the buffer containing the data to send
 *  \param __n    The number of bytes to send
 *
 *  \exception  ET_LINK_ERROR         chan-end destination is not set.
 *  \exception  ET_ILLEGAL_RESOURCE   not an allocated chan-end.
 *  \exception  ET_RESOURCE_DEP       another core is actively using the chan-end.
 *  \exception  ET_LOAD_STORE         invalid *buf[]* argument.
 */
_XCORE_INLINE void s_chan_out_buf_byte(chanend_t __c, const uint8_t __buf[], size_t __n) _XCORE_NOTHROW
{
  for (size_t __i = 0; __i < __n; __i++)
  {
    chanend_out_byte(__c, __buf[__i]);
  }
}

/** \brief Input a word from a streaming_channel_t.
 *
 *  \param __c    The streaming chan-end
 *  \return     The word read from the channel
 *
 *  \exception  ET_ILLEGAL_RESOURCE   not an allocated chan-end,
 *                                    or has pending control token.
 *  \exception  ET_RESOURCE_DEP       another core is actively using the chan-end.
 */
_XCORE_INLINE uint32_t s_chan_in_word(chanend_t __c) _XCORE_NOTHROW
{
  return chanend_in_word(__c);
}

/** \brief Input a byte from a streaming_channel_t.
 *
 *  \param __c    The streaming chan-end
 *  \return     The byte read from the channel
 *
 *  \exception  ET_ILLEGAL_RESOURCE   not an allocated chan-end,
 *                                    or has pending control token.
 *  \exception  ET_RESOURCE_DEP       another core is actively using the chan-end.
 */
_XCORE_INLINE uint8_t s_chan_in_byte(chanend_t __c) _XCORE_NOTHROW
{
  return chanend_in_byte(__c);
}

/** \brief Input a block of data from a streaming_channel_t.
 *
 *  \param __c    The streaming chan-end
 *  \param __buf  A pointer to the memory region to fill
 *  \param __n    The number of words to receive
 *
 *  \exception  ET_ILLEGAL_RESOURCE   not an allocated chan-end,
 *                                    or has pending control token.
 *  \exception  ET_RESOURCE_DEP       another core is actively using the chan-end.
 *  \exception  ET_LOAD_STORE         invalid *buf[]* argument.
 */
_XCORE_INLINE void s_chan_in_buf_word(chanend_t __c, uint32_t __buf[], size_t __n) _XCORE_NOTHROW
{
  for (size_t __i = 0; __i < __n; __i++)
  {
    __buf[__i] = chanend_in_word(__c);
  }
}

/** \brief Input a block of data from a streaming_channel_t.
 *
 *  \param __c    The streaming chan-end
 *  \param __buf  A pointer to the memory region to fill
 *  \param __n    The number of bytes to receive
 *
 *  \exception  ET_ILLEGAL_RESOURCE   not an allocated chan-end,
 *                                    or has pending control token.
 *  \exception  ET_RESOURCE_DEP       another core is actively using the chan-end.
 *  \exception  ET_LOAD_STORE         invalid *buf[]* argument.
 */
_XCORE_INLINE void s_chan_in_buf_byte(chanend_t __c, uint8_t __buf[], size_t __n) _XCORE_NOTHROW
{
  for (size_t __i = 0; __i < __n; __i++)
  {
    __buf[__i] = chanend_in_byte(__c);
  }
}

#ifdef __cplusplus
}
#endif

