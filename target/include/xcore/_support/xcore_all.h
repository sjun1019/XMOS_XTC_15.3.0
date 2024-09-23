#pragma once

#include <xcore/assert.h>
#include <xcore/chanend.h>
#include <xcore/channel.h>
#include <xcore/channel_streaming.h>
#include <xcore/channel_transaction.h>
#include <xcore/clock.h>
#include <xcore/hwtimer.h>
#include <xcore/interrupt.h>
#include <xcore/lock.h>
#include <xcore/parallel.h>
#include <xcore/port.h>
#include <xcore/port_protocol.h>
#include <xcore/select.h>
#include <xcore/thread.h>
#include <xcore/triggerable.h>

#include <xcore/_support/xcore_feature_test_impl.h>

#ifdef _XCORE_HAS_SWMEM
#include <xcore/swmem_fill.h>
#include <xcore/swmem_evict.h>
#endif

#ifdef _XCORE_HAS_MINICACHE
#include <xcore/minicache.h>
#endif
