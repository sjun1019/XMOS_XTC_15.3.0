// Copyright (c) 2016-2020, XMOS Ltd, All rights reserved
#pragma once

/** \file
 *  \brief Cache maintenance interface for the XS3 MiniCache.
 *
 *  The minicache is fully associative and consists of a small number of naturally aligned lines.
 *  The minicache caches all reads and writes to and from software memory and external RAM.
 *  A single read or write can cause up to two lines to be filled or evicted (in the case that
 *  the address range affected spans multiple lines). When there are insufficient lines free to
 *  service a fill then allocated lines will be evicted, up to the number of lines to be filled.
 *  Evicted lines are chosen based a 'psuedo leaast recently used' algorithm.
 *
 *  Flush and invlidate operations queue until the cache is idle (i.e. until all ongoing fills
 *  and evicts have finished). During this wait, and whilst the operation completes, all new
 *  loads and stores through the cache will block.
 *
 *  This header is only available when targeting platforms with a compatible cache.
 */

#include <xcore/_support/xcore_feature_test_impl.h>

#ifndef _XCORE_HAS_MINICACHE
#error Minicache not available on this platform
#endif

#include <stdint.h>
#include <xcore/_support/xcore_common.h>
#include <xcore/_support/xcore_minicache_impl.h>

#ifdef __cplusplus
extern "C" {
#endif

/** \brief Prefetch an address into the cache without blocking
 *
 *  Begins filling a cache line corresponding to the given \a __address.
 *  The address must be word-aligned and within a read-enabled region.
 *  If the region is an 'uncached' one (e.g. RAM) then this has no effect.
 *  If the address is cache line-aligned then a single line will be filled,
 *  otherwise the line corresponding to the address will be prefetched and
 *  additionally the next line in the address space (if such a line would
 *  have a valid address).
 *
 *  For each line prefetched:
 *   \li If the address is already in the cache then no action is taken;
 *   \li Otherwise, a fill is started for that line;
 *   \li If there is not an available (unallocated) cache line, then an
 *       allocated one will be evicted before the fill can begin.
 *
 *  Prefetching an address is non-blocking even if it causes a fill and/or eviction.
 *
 *  If the address does not correspond to a valid region, is not word aligned,
 *  or is within a region for which filling is disabled then a trap will occur.
 *
 *  \param __address  The address to prefetch
 */
_XCORE_INLINE void minicache_prefetch(const volatile void *__address) _XCORE_NOTHROW
{
  __xcore_minicache_prefetch((void *)__address);
}

/** \brief Flush all dirty data in the cache back to its respective memory
 *
 *  Sets a flush operation waiting on the cache. This will wait for all ongoing
 *  fills and then write all dirty lines back to their respective memories.
 *  The data remains in the cache, however its dirty state is cleared.
 *  During the flush operation (and whilst waiting to start it) all access to
 *  cached memory is blocking.
 *
 *  \note If an invalidate operation is already waiting when a flush is queued, it
 *        is unspecified which will execute first.
 */
_XCORE_INLINE void minicache_flush(void) _XCORE_NOTHROW
{
  __xcore_minicache_flush();
}

/** \brief Invalidates all data in the cache
 *
 *  Sets an invalidate operation waiting on the cache. This will mark all cache lines
 *  invalid - meaning that any unflushed writes are effectively reversed.
 *
 *  /note If a flush is queued before an invlidate has completed it is unspecified
 *        which will execute first.
 *
 *  /warning This may cause the observed value at a cached address to change.
 */
_XCORE_INLINE void minicache_invalidate(void) _XCORE_NOTHROW
{
  __xcore_minicache_invalidate();
}

#ifdef __cplusplus
}
#endif
