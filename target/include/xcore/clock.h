// Copyright (c) 2016-2020, XMOS Ltd, All rights reserved
#pragma once

/** \file
 *  \brief Hardware clock API
 */

#include <stdint.h>
#include <xcore/_support/xcore_common.h>
#include <xcore/_support/xcore_clock_impl.h>
#include <xcore/_support/xcore_resource_impl.h>

#ifdef __cplusplus
extern "C" {
#endif

/** \brief Clock handle type. */
typedef resource_t xclock_t;


/** \brief Enables a specified clock block so that it may be used.
 *
 *  Should be called before any other operations are performed on the given \c id.
 *  When the clock is no longer required is should be disabled again with clock_disable().
 *
 *  \param __id   The id of the clock to enable
 *
 *  \exception  ET_ILLEGAL_RESOURCE   not a valid clock.
 *  \exception  ET_RESOURCE_DEP       another core is actively changing the clock.
 */
_XCORE_INLINE void clock_enable(xclock_t __id) _XCORE_NOTHROW
{
  _XCORE_RESOURCE_SETCI(__id, XS1_SETC_INUSE_ON);
}

/** \brief Disable a clock
 *
 *  /note Once disabled, a the clock must be re-enabled using clock_enable() 
 *        before it can be used again.
 *
 *  \param __clk  The clock to be disabled
 *
 *  \exception  ET_ILLEGAL_RESOURCE   not a valid clock.
 *  \exception  ET_RESOURCE_DEP       another core is actively changing the clock.
 */
_XCORE_INLINE void clock_disable(xclock_t __clk) _XCORE_NOTHROW
{
  _XCORE_RESOURCE_SETCI(__clk, XS1_SETC_INUSE_OFF);
}

/** \brief Start a clock
 *
 *  \param __clk  The clock to start running
 *
 *  \exception  ET_ILLEGAL_RESOURCE   not a valid clock.
 *  \exception  ET_RESOURCE_DEP       another core is actively changing the clock.
 */
_XCORE_INLINE void clock_start(xclock_t __clk) _XCORE_NOTHROW
{
  _XCORE_RESOURCE_SETCI(__clk, XS1_SETC_RUN_STARTR);
}

/** \brief Stop a clock
 *
 *  Waits until the clock is low and then pauses a clock.
 *
 *  \param __clk  The clock to stop
 *
 *  \exception  ET_ILLEGAL_RESOURCE   not a valid clock.
 *  \exception  ET_RESOURCE_DEP       another core is actively changing the clock.
 */
_XCORE_INLINE void clock_stop(xclock_t __clk) _XCORE_NOTHROW
{
  _XCORE_RESOURCE_SETCI(__clk, XS1_SETC_RUN_STOPR);
}

/** \brief Configure a clock's source to a 1-bit port
 *
 *  A clock can be a 1-bit port, the reference clock or the xCORE clock. Note
 *  that if the xCORE clock is used then a non-zero divide must be used for
 *  ports to function correctly.
 *
 *  \param __clk  The clock to configure
 *  \param __p  The 1-bit port to set as the clock input. Attempting to set a
 *            port which is not 1-bit as the input will cause an exception.
 *
 *  \exception  ET_ILLEGAL_RESOURCE   not a valid clock or port,
 *                                    or the clock is running,
 *                                    or p not a one bit port.
 *  \exception  ET_RESOURCE_DEP       another core is actively changing the clock.
 */
_XCORE_INLINE void clock_set_source_port(xclock_t __clk, resource_t __p) _XCORE_NOTHROW
{
  __xcore_clock_set_source_port(__clk, __p);
}

/** \brief Configure a clock's source to be the 100MHz reference clock
 *
 *  \param __clk  The clock to configure
 *
 *  \exception  ET_ILLEGAL_RESOURCE   not a valid clock,
 *                                    or the clock is running.
 *  \exception  ET_RESOURCE_DEP       another core is actively changing the clock.
 */
_XCORE_INLINE void clock_set_source_clk_ref(xclock_t __clk) _XCORE_NOTHROW
{
  __xcore_clock_set_source_clk_ref(__clk);
}

/** \brief Configure a clock's source to be the xCORE clock.
 *
 *  \note When using the xCORE clock as the clock input a divide of > 0 must
 *  be used for the ports to function correclty.
 *
 *  \param __clk  The clock to configure
 *
 *  \exception  ET_ILLEGAL_RESOURCE   not a valid clock,
 *                                    or the clock is running.
 *  \exception  ET_RESOURCE_DEP       another core is actively changing the clock.
 */
_XCORE_INLINE void clock_set_source_clk_xcore(xclock_t __clk) _XCORE_NOTHROW
{
  __xcore_clock_set_source_clk_xcore(__clk);
}

/** \brief Configure the divider for a clock.
 *
 *  A clock can divide its input signal by an integer value which this function
 *  specifies. The XS2 architecture supports dividing the signal from a 1-bit
 *  port while the XS1 architecture will raise a trap if a non-zero divide is
 *  used with a 1-bit port input.
 *
 *  If the divide is 0 then the value signal will be passed through the clock.
 *  If the value is non-zero then the clock output will be divided by 2*divide.
 *
 *  \warning If the clock has been started then this will raise a trap.
 *
 *  \param __clk    The clock to configure
 *  \param __divide The divide value
 *
 *  \exception  ET_ILLEGAL_RESOURCE   not a valid clock,
 *                                    or the clock is running.
 *  \exception  ET_RESOURCE_DEP       another core is actively changing the clock.
 */
_XCORE_INLINE void clock_set_divide(xclock_t __clk, uint8_t __divide) _XCORE_NOTHROW
{
  __xcore_clock_set_divide(__clk, __divide);
}

/** \brief Sets a clock to use a 1-bit port for the ready-in signal.
 *
 *  If the port is not a 1-bit port then an exception is raised. The ready-in
 *  port controls when data is sampled from the pins.
 *
 *  \param __clk          The clock to configure.
 *  \param __ready_source The 1-bit port to use for the ready-in signal.
 *
 *  \exception  ET_ILLEGAL_RESOURCE   not a valid clock,
 *                                    or ready_source not a one bit port.
 *  \exception  ET_RESOURCE_DEP       another core is actively changing the clock.
 */
_XCORE_INLINE void clock_set_ready_src(xclock_t __clk, resource_t __ready_source) _XCORE_NOTHROW
{
  __xcore_clock_set_ready_src(__clk, __ready_source);
}

#ifdef __cplusplus
}
#endif

