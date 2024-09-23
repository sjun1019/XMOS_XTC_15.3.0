// Copyright (c) 2016-2020, XMOS Ltd, All rights reserved
#pragma once

/** \file
 *  \brief Helper functions for port usage patterns
 */

#include <stdint.h>
#include <xcore/_support/xcore_common.h>
#include <xcore/_support/xcore_port_impl.h>
#include <xcore/_support/xcore_clock_impl.h>
#include <xcore/clock.h>
#include <xcore/port.h>

#ifdef __cplusplus
extern "C" {
#endif

/** \brief Configure a port to be a clocked input port in handshake mode.
 *
 *  If the ready-in or ready-out ports are not 1-bit ports, an exception is raised.
 *  The ready-out port is asserted on the falling edge of the clock when the
 *  port's buffer is not full. The port samples its pins on its sampling edge
 *  when both the ready-in and ready-out ports are asserted.
 *
 *  By default the port's sampling edge is the rising edge of clock. This can be
 *  changed by the function port_set_sample_falling_edge().
 *
 *  \note A handshaken port must be buffered, so this function will also make
 *        the port buffered.
 *
 *  \param __p         The port to configure
 *  \param __readyin   A 1-bit port to use for the ready-in signal
 *  \param __readyout  A 1-bit port to use for the ready-out signal
 *  \param __clk       The clock used to configure the port
 *
 *  \exception  ET_ILLEGAL_RESOURCE   not a valid port/clock
 *                                    or clock is running,
 *                                    or readyin/readyout not a one bit port.
 *  \exception  ET_RESOURCE_DEP       another core is actively changing a port/clock
 */
_XCORE_INLINE void port_protocol_in_handshake(port_t __p, port_t __readyin, port_t __readyout, xclock_t __clk) _XCORE_NOTHROW
{
  __xcore_port_set_inout_data(__p);
  __xcore_port_set_buffered(__p);
  /* ensure port is in input mode. */
  (void) __xcore_port_in(__p);
  __xcore_clock_set_ready_src(__clk, __readyin);
  __xcore_port_set_clock(__p, __clk);
  __xcore_port_set_ready_handshake(__p);
  __xcore_port_clear_buffer(__p);
  __xcore_port_set_out_ready(__readyout, __p);
}

/** \brief Configures a port to be a clocked output port in handshake mode.
 *
 *  If the ready-in or ready-out ports are not 1-bit ports an exception is
 *  raised. the port drives the initial value on its pins until an
 *  output statement changes the value driven.
 *
 *  The ready-in port is read on the sampling edge of the port. outputs are driven
 *  on the next falling edge of the clock where the previous value read from the
 *  ready-in port was high.
 *
 *  On the falling edge of the clock the ready-out port is driven high
 *  if data is output on that edge, otherwise it is driven low.
 *
 *  By default the port's sampling edge is the rising edge of clock. this can be
 *  changed by the function port_set_sample_falling_edge().
 *
 *  \note A handshaken port must be buffered, so this function will also make
 *        the port buffered.
 *
 *  \param __p         the port to configure
 *  \param __readyin   a 1-bit port to use for the ready-in signal
 *  \param __readyout  a 1-bit port to use for the ready-out signal
 *  \param __clk       the clock used to configure the port
 *  \param __initial   the initial value to output on the port
 *
 *  \exception  ET_ILLEGAL_RESOURCE   not a valid port/clock
 *                                    or clock is running,
 *                                    or readyin/readyout not a one bit port.
 *  \exception  ET_RESOURCE_DEP       another core is actively changing a port/clock
 */
_XCORE_INLINE void port_protocol_out_handshake(port_t __p, port_t __readyin, port_t __readyout, xclock_t __clk, uint32_t __initial) _XCORE_NOTHROW
{
  __xcore_port_set_inout_data(__p);
  __xcore_port_set_buffered(__p);
  /* ensure port is in output mode. */
  __xcore_port_out(__p, __initial);
  __xcore_clock_set_ready_src(__clk, __readyin);
  __xcore_port_set_clock(__p, __clk);
  __xcore_port_set_ready_handshake(__p);
  __xcore_port_clear_buffer(__p);
  __xcore_port_set_out_ready(__readyout, __p);
}

/** \brief Configures a port to be a clocked input port in strobed master mode.
 *
 *  If the ready-out port is not a 1-bit port, an exception is raised.
 *  the ready-out port is asserted on the falling edge of the clock when the
 *  port's buffer is not full. the port samples its pins on its sampling edge
 *  after the ready-out port is asserted.
 *
 *  By default the port's sampling edge is the rising edge of clock. this can be
 *  changed by the function set_port_sample_delay().
 *
 *  \note A strobed port must be buffered, so this function will also make the
 *        port buffered.
 *
 *  \param __p         the port to configure
 *  \param __readyout  a 1-bit port to use for the ready-out signal
 *  \param __clk       the clock used to configure the port
 *
 *  \exception  ET_ILLEGAL_RESOURCE   not a valid port/clock
 *                                    or clock is running,
 *                                    or readyout not a one bit port.
 *  \exception  ET_RESOURCE_DEP       another core is actively changing a port/clock
 */
_XCORE_INLINE void port_protocol_in_strobed_master(port_t __p, port_t __readyout, xclock_t __clk) _XCORE_NOTHROW
{
  __xcore_port_set_inout_data(__p);
  __xcore_port_set_buffered(__p);
  /* ensure port is in input mode. */
  (void) __xcore_port_in(__p);
  __xcore_port_set_clock(__p, __clk);
  __xcore_port_set_ready_strobed(__p);
  __xcore_port_set_master(__p);
  __xcore_port_clear_buffer(__p);
  __xcore_port_set_out_ready(__readyout, __p);
}

/** \brief Configures a port to be a clocked output port in strobed master mode.
 *
 *  If the ready-out port is not a 1-bit port, an exception is raised.
 *  the port drives the initial value on its pins until an
 *  output statement changes the value driven. outputs are driven on the next
 *  falling edge of the clock. on the falling edge of the clock the ready-out
 *  port is driven high if data is output on that edge, otherwise it is driven
 *  low.
 *
 *  \note A strobed port must be buffered, so this function will also make the
 *        port buffered.
 *
 *  \param __p         the port to configure
 *  \param __readyout  a 1-bit port to use for the ready-out signal
 *  \param __clk       the clock used to configure the port
 *  \param __initial   the initial value to output on the port
 *
 *  \exception  ET_ILLEGAL_RESOURCE   not a valid port/clock
 *                                    or clock is running,
 *                                    or readyout not a one bit port.
 *  \exception  ET_RESOURCE_DEP       another core is actively changing a port/clock
 */
_XCORE_INLINE void port_protocol_out_strobed_master(port_t __p, port_t __readyout, xclock_t __clk, uint32_t __initial) _XCORE_NOTHROW
{
  __xcore_port_set_inout_data(__p);
  __xcore_port_set_buffered(__p);
  /* ensure port is in output mode. */
  __xcore_port_out(__p, __initial);
  __xcore_port_set_clock(__p, __clk);
  __xcore_port_set_ready_strobed(__p);
  __xcore_port_set_master(__p);
  __xcore_port_clear_buffer(__p);
  __xcore_port_set_out_ready(__readyout, __p);
}

/** \brief Configures a port to be a clocked input port in strobed slave mode.
 *
 *  If the ready-in port is not a 1-bit port, an exception is raised.
 *  the port samples its pins on its sampling edge when the ready-in signal is
 *  high. by default the port's sampling edge is the rising edge of clock. this
 *  can be changed by the function set_port_sample_delay().
 *
 *  \note A strobed port must be buffered, so this function will also make the
 *        port buffered.
 *
 *  \param __p        the port to configure
 *  \param __readyin  a 1-bit port to use for the ready-in signal
 *  \param __clk      the clock used to configure the port
 *
 *  \exception  ET_ILLEGAL_RESOURCE   not a valid port/clock
 *                                    or clock is running,
 *                                    or readyin not a one bit port.
 *  \exception  ET_RESOURCE_DEP       another core is actively changing a port/clock
 */
_XCORE_INLINE void port_protocol_in_strobed_slave(port_t __p, port_t __readyin, xclock_t __clk) _XCORE_NOTHROW
{
  __xcore_port_set_inout_data(__p);
  __xcore_port_set_buffered(__p);
  /* ensure port is in input mode. */
  (void) __xcore_port_in(__p);
  __xcore_clock_set_ready_src(__clk, __readyin);
  __xcore_port_set_clock(__p, __clk);
  __xcore_port_set_ready_strobed(__p);
  __xcore_port_set_slave(__p);
  __xcore_port_clear_buffer(__p);
}

/** \brief Configures a port to be a clocked output port in strobed slave mode.
 *
 *  If the ready-in port is not a 1-bit port, an exception is raised.
 *  the port drives the initial value on its pins until an
 *  output statement changes the value driven. the ready-in port is read on the
 *  port's sampling edge. outputs are driven on the next falling edge
 *  of the clock where the previous value read from the ready-in port is high.
 *  by default the port's sampling edge is the rising edge of clock. this
 *  can be changed by the function set_port_sample_delay().
 *
 *  \note A strobed port must be buffered, so this function will also make the
 *        port buffered.
 *
 *  \param __p        the port to configure
 *  \param __readyin  a 1-bit port to use for the ready-in signal
 *  \param __clk      the clock used to configure the port
 *  \param __initial  the initial value to output on the port
 *
 *  \exception  ET_ILLEGAL_RESOURCE   not a valid port/clock
 *                                    or clock is running,
 *                                    or readyin not a one bit port.
 *  \exception  ET_RESOURCE_DEP       another core is actively changing a port/clock
 */
_XCORE_INLINE void port_protocol_out_strobed_slave(port_t __p, port_t __readyin, xclock_t __clk, uint32_t __initial) _XCORE_NOTHROW
{
  __xcore_port_set_inout_data(__p);
  __xcore_port_set_buffered(__p);
  /* ensure port is in output mode. */
  __xcore_port_out(__p, __initial);
  __xcore_clock_set_ready_src(__clk, __readyin);
  __xcore_port_set_clock(__p, __clk);
  __xcore_port_set_ready_strobed(__p);
  __xcore_port_set_slave(__p);
}

#ifdef __cplusplus
}
#endif

