// Copyright (c) 2020, XMOS Ltd, All rights reserved
#pragma once

/** \file
 *  \brief API for implementing software memory fill (write)
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

/** \brief Number of words which must be evicted per eviction request
 * \hideinitializer
 */
#define SWMEM_EVICT_SIZE_WORDS _XCORE_SWMEM_LINE_WORDS

/** \brief Handle type for a SwMem evict resource */
typedef resource_t swmem_evict_t;
/** \brief Pointer to an eviction slot - this is the lowest address which must be evicted */
typedef const void* evict_slot_t;
/*** \brief Type large enough to hold a per-byte dirty mask for an eviction */
typedef uint32_t evict_mask_t;


/** \brief Gets and enables a swmem evict resource
 *
 *  The handle must be passed to swmem_evict_free() before this function is called again.
 *
 *  \return The swmem evict handle.
 *
 *  \see swmem_evict_free()
 */
_XCORE_INLINE swmem_evict_t swmem_evict_get(void) _XCORE_NOTHROW
{
  const resource_t __evict = XS1_SWMEM_EVICT;
  __xcore_swmem_res_enable(__evict);
  return __evict;
}

/** \brief Disables a swmem evict resource as returned by swmem_evict_get()
 *
 *  \see swmem_evict_get()
 */
_XCORE_INLINE void swmem_evict_free(swmem_evict_t __r) _XCORE_NOTHROW
{
  __xcore_swmem_res_free(__r);
}

/** \brief Returns the slot handle of the waiting swmem eviction request,
 *         blocks if there isn't one
 *
 *  When a eviction request is raised, the contents of the 'eviction slot'
 *  are available to be read until the request is completed. Fulfilling the
 *  request may unblock another thread which is attempting to write.
 * 
 *  The handle returned is the lowest address in the eviction request; the
 *  length of the request is always \ref SWMEM_EVICT_SIZE_WORDS words.
 *
 *  Words can be read from the eviction slot individually using
 *  swmem_evict_read_word(); once all required words have been read
 *  swmem_evict_read_word_done() should be called to complete the request.
 *  Alternatively, swmem_evict_to_buffer() may be used to read and complete
 *  the request in a single call.
 *
 *  \param __r  The swmem evict resource handle 
 *  \return     An eviction slot handle
 *
 *  \see swmem_evict_free()
 */
_XCORE_INLINE evict_slot_t swmem_evict_in_address(swmem_evict_t __r) _XCORE_NOTHROW
{
  return __xcore_swmem_res_in_address(__r);
}

/** \brief Read a single word from an eviction slot
 *  
 *  Returns the word from the eviction slot at the given index.
 *  To complete the eviction swmem_evict_read_word_done() should be called.
 *
 *  \param __r          The swmem evict resource handle
 *  \param __slot       The eviction slot to read from (returned by swmem_evict_in_address())
 *  \param __word_index The index of the word (must be less than \ref SWMEM_EVICT_SIZE_WORDS)
 *  \return             The word at the given \a __word_index within the evict slot
 */
_XCORE_INLINE uint32_t swmem_evict_read_word(swmem_evict_t __r, evict_slot_t __slot, unsigned __word_index) _XCORE_NOTHROW
{
  return __xcore_swmem_evict_read_word(__r, __slot, __word_index);
}

/** \brief Gets the dirty mask for the current eviction
 *
 *  The mask returned is a bytewise dirty mask with one bit per byte in the
 *  entire eviction slot. This must only be called before the eviction request
 *  has been completed - i.e. it must be called \b before swmem_evict_read_word_done()
 *  or swmem_evict_to_buffer() for a given eviction request.
 *
 *  A byte is dirty if it has been written since it was last filled, or if it has been
 *  written but has never been filled (see \ref swmem_fill.h).
 *
 *  The least significant bit in the dirty mask corresponds to the lowest byte address in
 *  the fill slot and each subsequent byte address corresponds to the next least significant
 *  bit of the dirty mask.
 *
 *  \param __r     The evict resource
 *  \param __slot  The current eviction slot
 *  \return        A bytewise dirty mask for the given eviction on the given resource
 * 
 */
_XCORE_INLINE evict_mask_t swmem_evict_get_dirty_mask(swmem_evict_t __r, _XCORE_UNUSED evict_slot_t __slot) _XCORE_NOTHROW
{
  return __xcore_swmem_res_get_data(__r);
}

/** \brief Complete an eviction and unblock waiting threads
 * 
 *  This should be called after swmem_evict_read_word() has been used to get
 *  all required words from the evict request.
 *
 *  \param __r     The swmem evict resource handle
 *  \param __slot  The evict slot for the current eviction requesst
 *
 *  \note It is not necessary (or safe) to call this if the request is fulfilled by
 *        calling swmem_evict_to_buffer().
 */
_XCORE_INLINE void swmem_evict_read_word_done(swmem_evict_t __r, evict_slot_t __slot) _XCORE_NOTHROW
{
  __xcore_swmem_res_request_done(__r);
}

/** \brief Evict a slot into a buffer
 *
 *  For an evict slot returned by swmem_evict_in_address(), reads the eviction slot
 *  into the given buffer and completes the eviction.
 *
 *  \note If the dirty mask is required for the data read then it should be retrieved
 *        prior to calling this function. (Since the eviction will be complete on return.)
 *
 *  \param __r     The swmem evict resource handle
 *  \param __slot  The eviction slot to read (returned by swmem_evict_in_address()) 
 *  \param __buf   Buffer of at least \ref SWMEM_EVICT_SIZE_WORDS words to evict the slot to
 */
_XCORE_INLINE void swmem_evict_to_buffer(swmem_evict_t __r, evict_slot_t __slot, uint32_t *__buf) _XCORE_NOTHROW
{
  __xcore_swmem_evict_to_buf(__r, __slot, __buf);
  swmem_evict_read_word_done(__r, __slot);
}

#ifdef __cplusplus
}
#endif
