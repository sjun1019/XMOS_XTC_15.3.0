// Copyright (c) 2016-2020, XMOS Ltd, All rights reserved
#pragma once

/** \file
 *  \brief API for transaction-based channel communications
 */

#include <stdint.h>
#include <stddef.h>
#include <xcore/_support/xcore_common.h>
#include <xcore/chanend.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  __xcore_streaming_chanend_t __c;
  unsigned __last_out;
} __xcore_transacting_chanend_t;

/** \brief An opaque type for handling transactions
 *
 *  Users must not access its raw underlying type.
 */
typedef __xcore_transacting_chanend_t transacting_chanend_t;

// Manage direction changes.
// As specified in the Tools Development Guide, the last_out state is managed
// to control when CT_END tokens are sent or expected.
_XCORE_INLINE void __xcore_t_chan_change_to_input(transacting_chanend_t *__tc) _XCORE_NOTHROW
{
  if (__tc->__last_out) {
    chanend_out_end_token(__tc->__c);
    __tc->__last_out = 0;
  }
}

_XCORE_INLINE void __xcore_t_chan_change_to_output(transacting_chanend_t *__tc) _XCORE_NOTHROW
{
  if (!__tc->__last_out) {
    chanend_check_end_token(__tc->__c);
    __tc->__last_out = 1;
  }
}

/** \brief Start a transaction (master).
 *
 *  This initiates a transaction on a channel.
 *
 *  A transacting_chanend_t is used to temporarily open a transaction
 *  route through a channel.
 *  During the transaction, you can use transaction channel operations for
 *  increased efficiency.
 *  You can create a transacting chanend from a normal *chanend* using
 *  chan_init_transaction_master() and chan_init_transaction_slave().
 *
 *  This called must be matched by a call to chan_init_transaction_slave()
 *  on the other end of the channel.
 *
 *  \note A transaction must be closed with chan_complete_transaction().
 *
 *  \param __c  chan-end to initialize the transaction on. 
 *  \return     the intialized master transacting_chanend_t
 *
 *  \exception  ET_LINK_ERROR         chan-end destination is not set.
 *  \exception  ET_ILLEGAL_RESOURCE   not an allocated chan-end.
 *  \exception  ET_RESOURCE_DEP       another core is actively using the chan-end.
 */
_XCORE_INLINE transacting_chanend_t chan_init_transaction_master(chanend_t __c) _XCORE_NOTHROW
{
  chanend_out_end_token(__c);

  transacting_chanend_t __tc;
  __tc.__last_out = 0;
  __tc.__c = __c;

  return __tc;
}

/** \brief Start a transaction (slave).
 *
 *  This call must be matched by a call to chan_init_transaction_master()
 *  on the other end of the channel.
 *
 *  \note A transaction must be closed with chan_complete_transaction().
 *
 *  \warning The original channed \a c must not be used until the transaction is closed.
 *
 *  \param __c  chan-end to initialize the transaction on. chanend is invalidated
 *  \return     the intialized slave transacting_chanend_t
 *
 *  \exception  ET_ILLEGAL_RESOURCE   not an allocated chan-end,
 *                                    or does not contain CT_END token.
 *  \exception  ET_RESOURCE_DEP       another core is actively using the chan-end.
 */
_XCORE_INLINE transacting_chanend_t chan_init_transaction_slave(chanend_t __c) _XCORE_NOTHROW
{
  chanend_check_end_token(__c);

  transacting_chanend_t __tc;
  __tc.__last_out = 1;
  __tc.__c = __c;

  return __tc;
}

/** \brief Completes a transaction. 
 *  After this call the route between the
 *  two ends of the channel is freed allowing other channels to use the
 *  communication network.
 *
 *  Whilst the transacting_chanend_t is now invalid, the channel remains allocated,
 *  awaiting another transaction or deallocation.
 *
 *  \note This call must be accompanied by a call to chan_complete_transaction() on
 *  the other end of the channel.
 *
 *  \param __tc  Transacting chan-end to close.
 *  \return      The original chan-end which is valid once again.
 *
 *  \exception  ET_LINK_ERROR         chan-end destination is not set.
 *  \exception  ET_ILLEGAL_RESOURCE   not an allocated chan-end,
 *                                    or channel handshaking corrupted.
 *  \exception  ET_RESOURCE_DEP       another core is actively using the chan-end.
 */
_XCORE_INLINE chanend_t chan_complete_transaction(transacting_chanend_t __tc) _XCORE_NOTHROW
{
  if (__tc.__last_out)
  {
    chanend_out_end_token(__tc.__c);
    chanend_check_end_token(__tc.__c);
  }
  else
  {
    chanend_check_end_token(__tc.__c);
    chanend_out_end_token(__tc.__c);
  }

  return __tc.__c;
}

/** \brief Output a word over a transacting chan-end.
 *
 *  \param[in,out] __tc   Transacting chan-end
 *  \param __data         Word to be output
 *
 *  \exception  ET_LINK_ERROR         chan-end destination is not set.
 *  \exception  ET_ILLEGAL_RESOURCE   not an allocated chan-end,
 *                                    or channel handshaking corrupted.
 *  \exception  ET_RESOURCE_DEP       another core is actively using the chan-end.
 *  \exception  ET_LOAD_STORE         invalid \a tc argument.
 */
_XCORE_INLINE void t_chan_out_word(transacting_chanend_t *__tc, uint32_t __data) _XCORE_NOTHROW
{
  __xcore_t_chan_change_to_output(__tc);
  chanend_out_word(__tc->__c, __data);
}

/** \brief Output an byte over a transacting chan-end.
 *
 *  \param[in,out] __tc   Transacting chan-end
 *  \param __data         Byte to be output
 *
 *  \exception  ET_LINK_ERROR         chan-end destination is not set.
 *  \exception  ET_ILLEGAL_RESOURCE   not an allocated chan-end,
 *                                    or channel handshaking corrupted.
 *  \exception  ET_RESOURCE_DEP       another core is actively using the chan-end.
 *  \exception  ET_LOAD_STORE         invalid \a tc argument.
 */
_XCORE_INLINE void t_chan_out_byte(transacting_chanend_t *__tc, uint8_t __data) _XCORE_NOTHROW
{
  __xcore_t_chan_change_to_output(__tc);
  chanend_out_byte(__tc->__c, __data);
}

/** \brief Output a block of data over a transacting chan-end.
 *
 *  \param[in,out] __tc   Transacting chan-end
 *  \param[in] __buf      Pointer to the buffer containing the data to send
 *  \param __n            Number of words to send
 *
 *  \exception  ET_LINK_ERROR         chan-end destination is not set.
 *  \exception  ET_ILLEGAL_RESOURCE   not an allocated chan-end,
 *                                    or channel handshaking corrupted.
 *  \exception  ET_RESOURCE_DEP       another core is actively using the chan-end.
 *  \exception  ET_LOAD_STORE         invalid \a tc or \a buf[] argument.
 */
_XCORE_INLINE void t_chan_out_buf_word(transacting_chanend_t *__tc, const uint32_t __buf[], size_t __n) _XCORE_NOTHROW
{
  __xcore_t_chan_change_to_output(__tc);
  for (size_t __i = 0; __i < __n; __i++)
  {
    chanend_out_word(__tc->__c, __buf[__i]);
  }
}

/** \brief Output a block of data over a transacting chan-end.
 *
 *  \param[in,out] __tc   Transacting chan-end
 *  \param[in] __buf      Pointer to the buffer containing the data to send
 *  \param __n            Number of bytes to send
 *
 *  \exception  ET_LINK_ERROR         chan-end destination is not set.
 *  \exception  ET_ILLEGAL_RESOURCE   not an allocated chan-end,
 *                                    or channel handshaking corrupted.
 *  \exception  ET_RESOURCE_DEP       another core is actively using the chan-end.
 *  \exception  ET_LOAD_STORE         invalid \a tc or \a buf[] argument.
 */
_XCORE_INLINE void t_chan_out_buf_byte(transacting_chanend_t *__tc, const uint8_t __buf[], size_t __n) _XCORE_NOTHROW
{
  __xcore_t_chan_change_to_output(__tc);
  for (size_t __i = 0; __i < __n; __i++)
  {
    chanend_out_byte(__tc->__c, __buf[__i]);
  }
}

/** \brief Input a word from a transacting chan-end.
 *
 *  \param[in,out] __tc   Transacting chan-end
 *  \return Word read from \a tc
 *
 *  \exception  ET_LINK_ERROR         chan-end destination is not set.
 *  \exception  ET_ILLEGAL_RESOURCE   not an allocated chan-end,
 *                                    or channel handshaking corrupted.
 *  \exception  ET_RESOURCE_DEP       another core is actively using the chan-end.
 *  \exception  ET_LOAD_STORE         invalid \a tc argument.
 */
_XCORE_INLINE uint32_t t_chan_in_word(transacting_chanend_t *__tc) _XCORE_NOTHROW
{
  __xcore_t_chan_change_to_input(__tc);
  return chanend_in_word(__tc->__c);
}

/** \brief Input a byte from a transacting chan-end.
 *
 *  \param[in,out] __tc   Transacting chan-end
 *  \return Byte read from \a tc
 *
 *  \exception  ET_LINK_ERROR         chan-end destination is not set.
 *  \exception  ET_ILLEGAL_RESOURCE   not an allocated chan-end,
 *                                    or channel handshaking corrupted.
 *  \exception  ET_RESOURCE_DEP       another core is actively using the chan-end.
 *  \exception  ET_LOAD_STORE         invalid \a tc or \a data argument.
 */
_XCORE_INLINE uint8_t t_chan_in_byte(transacting_chanend_t *__tc) _XCORE_NOTHROW
{
  __xcore_t_chan_change_to_input(__tc);
  return chanend_in_byte(__tc->__c);
}

/** \brief Input a block of data from a transacting chan-end.
 *
 *  \param[in,out] __tc   Transacting chan-end
 *  \param[in] __buf      Pointer to the memory region to fill
 *  \param __n            The number of words to receive
 *
 *  \exception  ET_LINK_ERROR         chan-end destination is not set.
 *  \exception  ET_ILLEGAL_RESOURCE   not an allocated chan-end,
 *                                    or channel handshaking corrupted.
 *  \exception  ET_RESOURCE_DEP       another core is actively using the chan-end.
 *  \exception  ET_LOAD_STORE         invalid \a tc or \a buf[] argument.
 */
_XCORE_INLINE void t_chan_in_buf_word(transacting_chanend_t *__tc, uint32_t __buf[], size_t __n) _XCORE_NOTHROW
{
  __xcore_t_chan_change_to_input(__tc);
  for (size_t __i = 0; __i < __n; __i++)
  {
    __buf[__i] = chanend_in_word(__tc->__c);
  }
}

/** \brief Input a block of data from a transacting chan-end.
 *
 *  \param[in,out] __tc   Transacting chan-end
 *  \param[in] __buf      Pointer to the memory region to fill
 *  \param __n            The number of bytes to receive
 *
 *  \exception  ET_LINK_ERROR         chan-end destination is not set.
 *  \exception  ET_ILLEGAL_RESOURCE   not an allocated chan-end,
 *                                    or channel handshaking corrupted.
 *  \exception  ET_RESOURCE_DEP       another core is actively using the chan-end.
 *  \exception  ET_LOAD_STORE         invalid \a __tc or \a __buf argument.
 */
_XCORE_INLINE void t_chan_in_buf_byte(transacting_chanend_t *__tc, uint8_t __buf[], size_t __n) _XCORE_NOTHROW
{
  __xcore_t_chan_change_to_input(__tc);
  for (size_t __i = 0; __i < __n; __i++)
  {
    __buf[__i] = chanend_in_byte(__tc->__c);
  }
}

#ifdef __cplusplus
}
#endif

