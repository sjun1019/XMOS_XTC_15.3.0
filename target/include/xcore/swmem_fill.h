// Copyright (c) 2016-2020, XMOS Ltd, All rights reserved
#pragma once

/** \file
 *  \brief API for implementing software memory fill (read)
 */

#include <xcore/_support/xcore_feature_test_impl.h>

#ifndef _XCORE_HAS_SWMEM
#error Software memory not available on this platform
#endif

#include <stdint.h>
#include <xcore/_support/xcore_common.h>
#include <xcore/_support/xcore_resource_impl.h>
#include <xcore/_support/xcore_swmem_impl.h>

#ifdef __cplusplus
extern "C" {
#endif

/** \brief Number of words which must be filled per fill request
 * \hideinitializer
 */
#define SWMEM_FILL_SIZE_WORDS _XCORE_SWMEM_LINE_WORDS

/** \brief Buffer type which holds the correct number of words for a complete fill */
typedef uint32_t swmem_fill_buffer_t[SWMEM_FILL_SIZE_WORDS];
/** \brief Handle type for a SwMem fill resource */
typedef resource_t swmem_fill_t;
/** \brief Pointer to a fill slot - this is the lowest address which must be filled */
typedef const void* fill_slot_t;

/** \brief Gets and enables a swmem fill resource
 *
 *  The handle must be passed to swmem_fill_free() before this function is called again.
 *
 *  \return The swmem fill handle.
 *
 *  \see swmem_fill_free()
 */
_XCORE_INLINE swmem_fill_t swmem_fill_get(void)
{
  const resource_t __fill = XS1_SWMEM_FILL;
  __xcore_swmem_res_enable(__fill);
  return __fill;
}

/** \brief Disables a swmem fill resource as returned by swmem_fill_get()
 *
 *  \see swmem_fill_get()
 */
_XCORE_INLINE void swmem_fill_free(swmem_fill_t __r) _XCORE_NOTHROW
{
  __xcore_swmem_res_free(__r);
}

/** \brief Returns the slot handle of the waiting swmem fill request,
 *         blocks if there isn't one
 *
 *  When a fill request is raised, the complete 'fill slot' must be populated.
 *  There are two ways of doing this:
 *   \li Passing a complete buffer of length \ref SWMEM_FILL_SIZE_WORDS words
 *       to swmem_fill_populate_from_buffer()
 *   \li Calling swmem_fill_populate_word() for each individual word in the
 *       fill slot (i.e. at least \ref SWMEM_FILL_SIZE_WORDS times) and then
 *       calling swmem_fill_populate_word_done().
 *
 *  The handle returned is the lowest address in the fill request; the
 *  length of the fill request is always \ref SWMEM_FILL_SIZE_WORDS words.
 *
 *  \param __r  The swmem fill resource handle 
 *  \return     A fill slot handle which must be populated
 */
_XCORE_INLINE fill_slot_t swmem_fill_in_address(swmem_fill_t __r) _XCORE_NOTHROW
{
  return __xcore_swmem_res_in_address(__r);
}

/** \brief Populate a single word in a fill slot
 *  
 *  Fills a single word in a fill slot for a waiting fill request.
 *  The fill slot must be the last one returned by swmem_fill_in_address()
 *  and the fill request must not have been completed either by calling
 *  swmem_fill_populate_word_done() or by calling swmem_fill_populate_from_buffer().
 *  To fulfill a request this function should be called for each word in the
 *  fill slot (i.e. with word_index set to each of <tt>[0, SWMEM_FILL_SIZE_WORDS)</tt>.
 *  Once all words have been populated in the fill slot, swmem_fill_populate_word_done()
 *  should be called to complete the fulfillment of the request and unblock any threads
 *  waiting for the fill.
 *
 *  \note It is safe to call this multiple times for the same word_index of a fill
 *        request, but only the last such call will have any effect.
 *
 *  \param __r          The swmem fill resource handle
 *  \param __slot       The fill slot to populate (returned by swmem_fill_in_address())
 *  \param __word_index The index of the word (must be less than \ref SWMEM_FILL_SIZE_WORDS)
 *  \param __value      The value to set at the given index
 */
_XCORE_INLINE void swmem_fill_populate_word(swmem_fill_t __r, fill_slot_t __slot, unsigned __word_index, uint32_t __value) _XCORE_NOTHROW
{
  __xcore_swmem_fill_populate_word(__r, __slot, __word_index, __value);
}

/** \brief Complete a fill and unblock waiting threads
 * 
 *  This should be called after swmem_fill_populate_word() has been used to fill
 *  each word in the fill request.
 *
 *  \param __r     The swmem fill resource handle
 *  \param __slot  The fill slot for the ongoing fill request
 *
 *  \note It is not necessary (or safe) to call this if the request is fulfilled by
 *        calling swmem_fill_populate_from_buffer().
 */
_XCORE_INLINE void swmem_fill_populate_word_done(swmem_fill_t __r, _XCORE_UNUSED fill_slot_t __slot) _XCORE_NOTHROW
{
  __xcore_swmem_res_request_done(__r);
}

/** \brief Service a fill request using data in a suitably sized buffer
 *
 *  For a fill slot returned by swmem_fill_in_address(), services the waiting
 *  fill request using data from \a __source - this must point to a buffer
 *  of at least \ref SWMEM_FILL_SIZE_WORDS words; source[0] will be used to fill
 *  the 0th word in the fill slot, and so on.
 *
 *  \param __r      The swmem fill resource handle
 *  \param __slot   The fill slot to populate (returned by swmem_fill_in_address()) 
 *  \param __source Buffer of at least \ref SWMEM_FILL_SIZE_WORDS words to fill the slot from
 */
_XCORE_INLINE void swmem_fill_populate_from_buffer(swmem_fill_t __r, fill_slot_t __slot, const uint32_t *__source) _XCORE_NOTHROW
{
  __xcore_swmem_fill_populate_slot(__r, __slot, __source);
  swmem_fill_populate_word_done(__r, __slot);
}

#ifdef __cplusplus
}
#endif
