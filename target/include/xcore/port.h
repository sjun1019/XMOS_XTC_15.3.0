// Copyright (c) 2016-2020, XMOS Ltd, All rights reserved
#pragma once

/** \file
 *  \brief API for IO using ports
 */

#include <stdint.h>
#include <stddef.h>
#include <xcore/_support/xcore_common.h>
#include <xcore/_support/xcore_port_impl.h>
#include <xcore/_support/xcore_resource_impl.h>
#include <xcore/clock.h>

#ifdef __cplusplus
extern "C" {
#endif

/** \brief Enumeration to declare how the port was set up */
typedef enum {
  PORT_UNBUFFERED = 0,
  PORT_BUFFERED = 1,
} port_type_t;

/** \brief Port handle type. */
typedef resource_t port_t;

/** \brief Integer type capable of holding all possible values of a port timestamp.
 *  \note The actual valid range of timestamps is given by 
 *        \ref PORT_TIMESTAMP_MIN and \ref PORT_TIMESTAMP_MAX which are the lowest
 *             and highest valid timestamps respectively.
 */
typedef uint32_t port_timestamp_t;

/** \brief Lowest legal value for a port timestamp
 *  \hideinitializer
 */
#define PORT_TIMESTAMP_MIN _XCORE_PORT_TIMESTAMP_MIN

/** \brief Largest legal value for a port timestamp
 *  \hideinitializer
 */
#define PORT_TIMESTAMP_MAX _XCORE_PORT_TIMESTAMP_MAX

/** \brief Enables a port.
 *
 *  Either this function or port_start_buffered() must be called once for each
 *  variable of type \c port before use. port_disable() must be called afterwards.
 *
 *  The port's state is set to: input, unbuffered, inout_data, no_invert,
 *  rising_edge, master, no_ready, no triggers, clocked by XS1_CLKBLK_REF.
 *
 *  \param __p  The port to enable
 *
 *  \exception  ET_ILLEGAL_RESOURCE   not a valid port.
 *  \exception  ET_RESOURCE_DEP       another core is actively changing the port.
 *
 * \see port_enable_buffered
 */
_XCORE_INLINE void port_enable(port_t __p) _XCORE_NOTHROW
{
  __xcore_port_enable(__p);
}

/** \brief Reset a port.
 *
 *  Clears a ports settings back to the default state at port_enable().
 *
 *  \param __p  The port to be reset
 *
 *  \exception  ET_ILLEGAL_RESOURCE   not a valid port.
 *  \exception  ET_RESOURCE_DEP       another core is actively changing the port.
 */
_XCORE_INLINE void port_reset(port_t __p) _XCORE_NOTHROW
{
  __xcore_port_reset(__p);
}

/** \brief Allocates a port to buffer and serialise/deserialise data.
 *
 *  Either this function or port_enable() must be called once for each port_t
 *  before it is used.
 *
 *  \param __p               Value that identifies which port to drive
 *  \param __transfer_width  Number of bits to serialise; must be 1, 4, 8, or 32.
 *                           The number of bits must be >= to the physical port
 *                           width.
 *
 *  \exception  ET_ILLEGAL_RESOURCE   not a valid port,
 *                                    or is not legal width for the port.
 *  \exception  ET_RESOURCE_DEP       another core is actively changing the port.
 * 
 *  \see port_enable
 */
_XCORE_INLINE void port_start_buffered(port_t __p, size_t __transfer_width) _XCORE_NOTHROW
{
  __xcore_port_enable(__p);
  __xcore_port_set_buffered(__p);
  __xcore_port_set_transfer_width(__p, __transfer_width);
}

/** \brief Disable a port
 *
 *  Disables the port so it is no longer ready for use. It must be re-enabled if it is used again.
 *  \param __p  The port to be disabled
 *
 *  \exception  ET_ILLEGAL_RESOURCE   not a valid port.
 *  \exception  ET_RESOURCE_DEP       another core is actively changing the port.
 */
_XCORE_INLINE void port_disable(port_t __p) _XCORE_NOTHROW
{
  __xcore_port_free(__p);
}

/** \brief Change the transfer width of a port.
 *
 *  The default transfer width is the same as the physical port width.
 *
 *  \attention A port must have been set to buffered if the width is different from the
 *             physical port width
 *
 *  \param __p               The port to change the transfer width of
 *  \param __transfer_width  Number of bits to serialise; must be 1, 4, 8, or 32.
 *                           The number of bits must be >= to the physical port
 *                           width.
 *
 *  \exception  ET_ILLEGAL_RESOURCE   not a valid port,
 *                                    or is not legal width for the port,
 *                                    or the port is unbuffered.
 *  \exception  ET_RESOURCE_DEP       another core is actively changing the port.
 */
_XCORE_INLINE void port_set_transfer_width(port_t __p, size_t __transfer_width) _XCORE_NOTHROW
{
  __xcore_port_set_transfer_width(__p, __transfer_width);
}

/** \brief Sets a port to be buffered.
 *
 *  Configures a port into buffered mode where it can automatically serialise or
 *  deserialise data.
 *
 *  \param __p  The port to set as buffered
 *
 *  \exception  ET_ILLEGAL_RESOURCE   not a valid port.
 *  \exception  ET_RESOURCE_DEP       another core is actively changing the port.
 */
_XCORE_INLINE void port_set_buffered(port_t __p) _XCORE_NOTHROW
{
  __xcore_port_set_buffered(__p);
}

/** \brief Sets a port to be unbuffered (default state).
 *
 *  Configures a port into unbuffered mode. Note that before this is called, a
 *  a port needs to have its transfer width equal to the port width and be
 *  configured as a master port.
 *
 *  \param __p  The port to set as unbuffered
 *
 *  \exception  ET_ILLEGAL_RESOURCE   not a valid port.
 *  \exception  ET_RESOURCE_DEP       another core is actively changing the port.
 */
_XCORE_INLINE void port_set_unbuffered(port_t __p) _XCORE_NOTHROW
{
  __xcore_port_set_unbuffered(__p);
}

/** \brief Configure the port with a platform-specific value
 *
 *  Uses the value of \a __word to configure the port. The effect of this
 *  is platform dependent.
 *
 *  \param __p     The port to configure
 *  \param __word  The config word to set on the port
 *
 *  \exception  ET_ILLEGAL_RESOURCE   not a valid port.
 *  \exception  ET_RESOURCE_DEP       another core is actively changing the port
 *                                    or \a __word is not valid
 */
_XCORE_INLINE void port_write_control_word(port_t __p, uint32_t __word) _XCORE_NOTHROW
{
  __xcore_resource_write_control_word(__p, __word);
}

/** \brief Set the clock clocking a port.
 *
 *  Changes the clock used for a port's control functions.
 *  The default clock is XS1_CLKBLK_REF.
 *
 *  \param __p    Port whose clock is being changed.
 *  \param __clk  Clock to attach to the port.
 *
 *  \exception  ET_ILLEGAL_RESOURCE   not a valid port/clock, or clock is running.
 *  \exception  ET_RESOURCE_DEP       another core is actively changing the port.
 */
_XCORE_INLINE void port_set_clock(port_t __p, xclock_t __clk) _XCORE_NOTHROW
{
  __xcore_port_set_clock(__p, __clk);
}

/** \brief Set a port drive out the data value (default state).
 *
 *  \param __p  Port to change the mode of.
 *
 *  \exception  ET_ILLEGAL_RESOURCE   not a valid port.
 *  \exception  ET_RESOURCE_DEP       another core is actively changing the port.
 */
_XCORE_INLINE void port_set_inout_data(port_t __p) _XCORE_NOTHROW
{
  __xcore_port_set_inout_data(__p);
}

/** \brief Set a port to drive out its clocking signal.
 *
 *  Configures the port to drive the clock signal instead of its
 *  own data values. The clock signal that is driven out is configured using the
 *  port_set_clock() function.
 *
 *  \param __p  Port to change the mode of.
 *
 *  \exception  ET_ILLEGAL_RESOURCE   not a valid port.
 *  \exception  ET_RESOURCE_DEP       another core is actively changing the port.
 */
_XCORE_INLINE void port_set_out_clock(port_t __p) _XCORE_NOTHROW
{
  __xcore_port_set_out_clock(__p);
}

/** \brief Set a port to drive out the ready signal of another port.
 *
 *  Configures the port to drive the ready signal of another port
 *  instead of its own data values.
 *
 *  \param __p             Port to change the mode of. This must be a 1-bit
 *                         port or this function will trap.
 *  \param __ready_source  The port whose ready signal will be output.
 *
 *  \exception  ET_ILLEGAL_RESOURCE   \li not a valid port \li \a p not a one bit port.
 *  \exception  ET_RESOURCE_DEP       another core is actively changing the port.
 */
_XCORE_INLINE void port_set_out_ready(port_t __p, port_t __ready_source) _XCORE_NOTHROW
{
  __xcore_port_set_out_ready(__p, __ready_source);
}

/** \brief Set the port to invert its data.
 *
 *  Configures a port to invert the data on the pin. This can be
 *  reverted by calling port_set_no_invert().
 *
 *  \param __p  Port to set its data to be inverted. This must be a 1-bit port
 *              or a trap will be raised.
 *
 *  \exception  ET_ILLEGAL_RESOURCE  \li not a valid port \li p not a one bit port.
 *  \exception  ET_RESOURCE_DEP      another core is actively changing the port.
 */
_XCORE_INLINE void port_set_invert(port_t __p) _XCORE_NOTHROW
{
  __xcore_port_set_invert(__p);
}

/** \brief Set the port to not invert its data (default state).
 *
 *  Configures a port to not invert the data on the pin.
 *
 *  \param __p  Port to set the data to not be inverted.
 *
 *  \exception  ET_ILLEGAL_RESOURCE   not a valid port.
 *  \exception  ET_RESOURCE_DEP       another core is actively changing the port.
 */
_XCORE_INLINE void port_set_no_invert(port_t __p) _XCORE_NOTHROW
{
  __xcore_port_set_no_invert(__p);
}

/** \brief Set the port to sample on the falling edge.
 *
 *  The default is for a port to sample data on the rising edge of the clock.
 *  This function changes the port to sample on the falling edge instead.
 *  This change can be reverted by calling port_set_sample_rising_edge().
 *
 *  \param __p  Port to change its sampling edge.
 *
 *  \exception  ET_ILLEGAL_RESOURCE   not a valid port.
 *  \exception  ET_RESOURCE_DEP       another core is actively changing the port.
 */
_XCORE_INLINE void port_set_sample_falling_edge(port_t __p) _XCORE_NOTHROW
{
  __xcore_port_set_sample_falling_edge(__p);
}

/** \brief Set the port to sample on the rising edge (default state).
 *
 *  Restores a port to sampling data on the rising edge of the clock.
 *
 *  \param __p  Port to change its sampling edge.
 *
 *  \exception  ET_ILLEGAL_RESOURCE   not a valid port.
 *  \exception  ET_RESOURCE_DEP       another core is actively changing the port.
 */
_XCORE_INLINE void port_set_sample_rising_edge(port_t __p) _XCORE_NOTHROW
{
  __xcore_port_set_sample_rising_edge(__p);
}

/** \brief Set the port to master mode (default state).
 *
 *  Configures a port to be a master. This is only relevant when
 *  using ready signals (port_set_ready_strobed() / port_set_ready_handshake()).
 *
 *  \param __p  Port to set as a master
 *
 *  \exception  ET_ILLEGAL_RESOURCE   not a valid port.
 *  \exception  ET_RESOURCE_DEP       another core is actively changing the port.
 *
 *  \note It is highly recommended to use the \c port_protocol_* functions to put a
 *        port into its desired mode as the order of operations is critical.
 *  \see xcore/port_protocol.h
 */
_XCORE_INLINE void port_set_master(port_t __p) _XCORE_NOTHROW
{
  __xcore_port_set_master(__p);
}

/** \brief Set the port to slave mode.
 *
 *  Configures a port to be a master. This is only relevant when
 *  using a ready strobe (port_set_ready_strobed())
 *
 *  \attention the port must be set to use a ready strobe, otherwise this function
 *             will raise an exception.
 *
 *  \param __p  Port to set as a slave
 *
 *  \exception  ET_ILLEGAL_RESOURCE   not a valid port.
 *  \exception  ET_RESOURCE_DEP       another core is actively changing the port.
 * 
 *  \note It is highly recommended to use the \c port_protocol_* functions to put a
 *        port into its desired mode as the order of operations is critical.
 *  \see xcore/port_protocol.h
 */
_XCORE_INLINE void port_set_slave(port_t __p) _XCORE_NOTHROW
{
  __xcore_port_set_slave(__p);
}

/** \brief Set the port to use no ready signals (default state).
 *
 *  Changes a port to not use ready signals. A port can be configured
 *  to use strobes or handshaking signals using port_set_ready_strobed() or
 *  port_set_ready_handshake().
 *
 *  \attention the port must be a \c master port otherwise this function will raise
 *             an exception.
 *
 *  \param __p  Port to change to not use ready signals
 *
 *  \exception  ET_ILLEGAL_RESOURCE   not a valid port.
 *  \exception  ET_RESOURCE_DEP       another core is actively changing the port.
 * 
 *  \note It is highly recommended to use the \c port_protocol_* functions to put a
 *        port into its desired mode as the order of operations is critical.
 *  \see xcore/port_protocol.h
 */
_XCORE_INLINE void port_set_no_ready(port_t __p) _XCORE_NOTHROW
{
  __xcore_port_set_no_ready(__p);
}

/** \brief Set the port to use a single strobe.
 *
 *  Changes a port to not use ready signals. A port can be configured
 *  to use strobes or handshaking signals using port_set_ready_strobed() or
 *  port_set_ready_handshake().
 *
 *  \attention the port must be a buffered port otherwise this function will raise
 *             an exception.
 *
 *  \param __p  Port to change to not use ready signals
 *
 *  \exception  ET_ILLEGAL_RESOURCE   not a valid port.
 *  \exception  ET_RESOURCE_DEP       another core is actively changing the port.
 *
 *  \note It is highly recommended to use the \c port_protocol_* functions to put a
 *        port into its desired mode as the order of operations is critical.
 *  \see xcore/port_protocol.h
 */
_XCORE_INLINE void port_set_ready_strobed(port_t __p) _XCORE_NOTHROW
{
  __xcore_port_set_ready_strobed(__p);
}

/** \brief Set the port to be fully handshaken.
 *
 *  Changes a port to use both a ready input and drive a ready
 *  output in order to control when data is sampled or written.
 *
 *  \attention the port must be a master buffered port otherwise this function will
 *             raise an exception.
 *
 *  \param __p  Port to change to not use ready signals
 *
 *  \exception  ET_ILLEGAL_RESOURCE   not a valid port.
 *  \exception  ET_RESOURCE_DEP       another core is actively changing the port.
 *
 *  \note It is highly recommended to use the \c port_protocol_* functions to put a
 *        port into its desired mode as the order of operations is critical.
 *  \see xcore/port_protocol.h
 */
_XCORE_INLINE void port_set_ready_handshake(port_t __p) _XCORE_NOTHROW
{
  __xcore_port_set_ready_handshake(__p);
}

/** \brief Gets the timestamp of the last input or output operation on a port.
 *
 *  \note The returned timestamp will be a valid timestamp no less than
 *        \ref PORT_TIMESTAMP_MIN and no greater than \ref PORT_TIMESTAMP_MAX.
 *
 *  \param __p  The port to get the timestamp from  
 *  \return     The timestamp of the last operation
 *
 *  \exception  ET_ILLEGAL_RESOURCE   not a valid port.
 *  \exception  ET_RESOURCE_DEP       another core is actively changing the port.
 */
_XCORE_INLINE port_timestamp_t port_get_trigger_time(port_t __p) _XCORE_NOTHROW
{
  return __xcore_port_get_trigger_time(__p);
}

/** \brief Set the timestamp at which the port will input/output data.
 *
 *  Sets the time condition for the next input or output on
 *  a port.
 *  If the port is unbuffered or the buffer is empty/full a call to port_in()
 *  or port_out() will pause until the specified time.
 *  The trigger is cleared by a input/output or by calling port_clear_trigger_time().
 *  The given timestamp must be a valid port timestamp no less than
 *  \ref PORT_TIMESTAMP_MIN and no greater than \ref PORT_TIMESTAMP_MAX.
 *
 *  \param __p  The port to set the condition on
 *  \param __t  The port timestamp to match
 *
 *  \exception  ET_ILLEGAL_RESOURCE   not a valid port.
 *  \exception  ET_RESOURCE_DEP       another core is actively using the port.
 */
_XCORE_INLINE void port_set_trigger_time(port_t __p, port_timestamp_t __t) _XCORE_NOTHROW
{
  __xcore_port_set_trigger_time(__p, __t);
}

/** \brief Clear the timestamp trigger on a port.
 *
 *  Clears any trigger_time condition on the port so the next input or
 *  output will happen unconditionally in respect to the timestamp.
 *  This function does not clear the trigger_in condition on the port.
 *
 *  \param __p  The port to clear the trigger_time on
 *
 *  \exception  ET_ILLEGAL_RESOURCE   not a valid port.
 *  \exception  ET_RESOURCE_DEP       another core is actively changing the port.
 */
_XCORE_INLINE void port_clear_trigger_time(port_t __p) _XCORE_NOTHROW
{
  __xcore_port_clear_trigger_time(__p);
}

/** \brief Setup an event to trigger on a port when its input value matches.
 *
 *  On a unbuffered port the trigger will apply to all future inputs
 *  until the trigger is set again.
 *  On an buffered port the trigger will only hold for the next input
 *  after which the trigger_in_equal will be cleared.
 *
 *  \param __p  The port to set the trigger on
 *  \param __v  The value to match
 *
 *  \exception  ET_ILLEGAL_RESOURCE   not a valid port.
 *  \exception  ET_RESOURCE_DEP       another core is actively changing the port.
 */
_XCORE_INLINE void port_set_trigger_in_equal(port_t __p, uint32_t __v) _XCORE_NOTHROW
{
  __xcore_port_set_trigger_in_equal(__p, __v);
}

/** \brief Setup an event to trigger on a port when its input value does not matches.
 *
 *  On a unbuffered port the trigger will apply to all future inputs
 *  until the trigger is set again.
 *  On an buffered port the trigger will only hold for the next input
 *  after which the trigger_in_not_equal will be cleared.
 *
 *  \param __p    The port to set the trigger on
 *  \param __v    The value to not match
 *
 *  \exception  ET_ILLEGAL_RESOURCE   not a valid port.
 *  \exception  ET_RESOURCE_DEP       another core is actively changing the port.
 */
_XCORE_INLINE void port_set_trigger_in_not_equal(port_t __p, uint32_t __v) _XCORE_NOTHROW
{
  __xcore_port_set_trigger_in_not_equal(__p, __v);
}

/** \brief Clear the in trigger on a port.
 *
 *  Clears any trigger_in condition on the port so the next input
 *  will happen unconditionally in respect to the input value.
 *  This function does not clear the trigger_time condition on the port.
 *
 *  \param __p    The port to clear the trigger_in on
 *
 *  \exception  ET_ILLEGAL_RESOURCE   not a valid port.
 *  \exception  ET_RESOURCE_DEP       another core is actively changing the port.
 */
_XCORE_INLINE void port_clear_trigger_in(port_t __p) _XCORE_NOTHROW
{
  __xcore_port_clear_trigger_in(__p);
}

/** \brief Peek at the value on a port.
 *
 *  Peeking a port returns the current value on the pins of a port, regardless
 *  of whether the port is an output or input and without affecting its direciton.
 *  Peek will not pause, regardless of any triggers that have been set.
 *
 *  \param __p     Port to be peeked
 *  \return      The current value on the pins
 *
 *  \exception  ET_ILLEGAL_RESOURCE   not a valid port.
 */
_XCORE_INLINE uint32_t port_peek(port_t __p) _XCORE_NOTHROW
{
  return __xcore_port_peek(__p);
}

/** \brief Outputs a value onto a port.
 *
 *  In the case of an unbuffered port, the value will be driven on the pins on
 *  the next clock cycle. In the case of a buffered port, the data will be
 *  stored in the buffer, and be serialised onto the output pins.
 *
 *  If there is a time trigger setup and the port is unbuffered or the buffer is full
 *  the call will pause until the specified time.
 *
 *  \param __p     Port to output to
 *  \param __data  Value to output
 *
 *  \exception  ET_ILLEGAL_RESOURCE   not a valid port.
 *  \exception  ET_RESOURCE_DEP       another core is actively changing the port.
 */
_XCORE_INLINE void port_out(port_t __p, uint32_t __data) _XCORE_NOTHROW
{
  __xcore_port_out(__p, __data);
}

/** \brief Input a value from a port.
 *
 *  For unbuffered port with no trigger, the data will be whatever is on the input pins.
 *  For unbuffered port with a trigger, the data will be the value read when the
 *  trigger fired. The call will pause if the trigger has not yet fired.
 *  For buffered port, this function will pause until the buffer is filled up with
 *  deserialised data.
 *
 *  \param __p  Port to input from
 *  \return     The inputted data
 *
 *  \exception  ET_ILLEGAL_RESOURCE   not a valid port.
 *  \exception  ET_RESOURCE_DEP       another core is actively changing the port.
 */
_XCORE_INLINE uint32_t port_in(port_t __p) _XCORE_NOTHROW
{
  return __xcore_port_in(__p);
}

/** \brief Outputs a value onto a port and shift the output data.
 *
 *  In the case of an unbuffered port, the value will be driven on the pins on
 *  the next clock cycle. In the case of a buffered port, the data will be stored
 *  in the buffer, and be serialised onto the output pins.
 *
 *  If there is a time trigger setup and the port is unbuffered or the buffer is full
 *  the call will pause until the specified time.
 *
 *  \param __p     Port to output to
 *  \param __data  \a data is shifted right by the transfer width of the port,
 *                 with the bits shifting out onto the port.
 *  \return        The remaining shifted bits of \a data
 *
 *  \exception  ET_ILLEGAL_RESOURCE   not a valid port.
 *  \exception  ET_RESOURCE_DEP       another core is actively changing the port.
 */
_XCORE_INLINE uint32_t port_out_shift_right(port_t __p, uint32_t __data) _XCORE_NOTHROW
{
  return __xcore_port_out_shift_right(__p, __data);
}

/** \brief Input a value from a port and shift the data.
 *
 *  For unbuffered port with no trigger, the data will be whatever is on the input pins.
 *  For unbuffered port with a trigger, the data will be the value read when the
 *  trigger fired. The call will pause if the trigger has not yet fired.
 *  For buffered port, this function will pause until the buffer is filled up with
 *  deserialised data.
 *
 *  \param __p     Port to input from
 *  \param __data  Initial value to shift input data into
 *  \return        Result of shifting \a data right by the port width and storing the
 *                 input data in the most significant bits
 *
 *  \exception  ET_ILLEGAL_RESOURCE   not a valid port.
 *  \exception  ET_RESOURCE_DEP       another core is actively changing the port.
 */
_XCORE_INLINE uint32_t port_in_shift_right(port_t __p, uint32_t __data) _XCORE_NOTHROW
{
  return __xcore_port_in_shift_right(__p, __data);
}

/** \brief Outputs a value onto a port at a specified port timestamp.
 *
 *  In the case of an unbuffered port, the value will be driven on the pins when
 *  on the clock cycle that moves the port timestamp to the specified time. In
 *  the case of a buffered port, the data will be stored in the buffer, and
 *  be serialised onto the output pins at the point that the time is reached.
 *  The given timestamp must be a valid port timestamp no less than
 *  \ref PORT_TIMESTAMP_MIN and no greater than \ref PORT_TIMESTAMP_MAX.
 *
 *  \param __p     Port to output to
 *  \param __t     The timestamp to do the output on
 *  \param __data  Value to output
 *
 *  \exception  ET_ILLEGAL_RESOURCE   not a valid port.
 *  \exception  ET_RESOURCE_DEP       another core is actively changing the port.
 */
_XCORE_INLINE void port_out_at_time(port_t __p, port_timestamp_t __t, uint32_t __data) _XCORE_NOTHROW
{
  __xcore_port_set_trigger_time(__p, __t);
  __xcore_port_out(__p, __data);
}

/** \brief Input data from a port when its counter is at a specific time.
 *
 *  In the case of an unbuffered port, the data will be inputted when the
 *  counter reaches time \a t. In the case of a buffered port, an input
 *  will wait until the given time and then will start capturing data,
 *  returning a value when the buffer is full.
 *  The given timestamp must be a valid port timestamp no less than
 *  \ref PORT_TIMESTAMP_MIN and no greater than \ref PORT_TIMESTAMP_MAX.
 *
 *  \param __p  Port to input from
 *  \param __t  The timestamp to do input on
 *  \return     The inputted data
 *
 *  \exception  ET_ILLEGAL_RESOURCE   not a valid port.
 *  \exception  ET_RESOURCE_DEP       another core is actively changing the port.
 */
_XCORE_INLINE uint32_t port_in_at_time(port_t __p, port_timestamp_t __t) _XCORE_NOTHROW
{
  __xcore_port_set_trigger_time(__p, __t);
  return __xcore_port_in(__p);
}

/** \brief Outputs a value onto a port at a specified time and shifts the output data.
 *
 *  In the case of an unbuffered port, the value will be driven on the pins when
 *  on the clock cycle that moves the port counter to the specified time. In
 *  the case of a buffered port, the data will be stored in the buffer, and
 *  be serialised onto the output pins at the point that the time is reached.
 *  The given timestamp must be a valid port timestamp no less than
 *  \ref PORT_TIMESTAMP_MIN and no greater than \ref PORT_TIMESTAMP_MAX.
 *
 *  \param __p     Port to output to
 *  \param __t     The timestamp of the output
 *  \param __data  data is shifted right by the transfer width of the port,
 *                 with the bits shifting out onto the port.
 *  \return        The remaining shifted bits
 *
 *  \exception  ET_ILLEGAL_RESOURCE   not a valid port.
 *  \exception  ET_RESOURCE_DEP       another core is actively changing the port.
 */
_XCORE_INLINE uint32_t port_out_shift_right_at_time(port_t __p, port_timestamp_t __t, uint32_t __data) _XCORE_NOTHROW
{
  __xcore_port_set_trigger_time(__p, __t);
  return __xcore_port_out_shift_right(__p, __data);
}

/** \brief Input data from a port at a specific time and shift the data.
 *
 *  In the case of an unbuffered port, the data will be inputted when the
 *  counter reaches time \a t. In the case of a buffered port, an input
 *  will wait until the given time and then will start capturing data,
 *  returning a value when the buffer is full.
 *  The given timestamp must be a valid port timestamp no less than
 *  \ref PORT_TIMESTAMP_MIN and no greater than \ref PORT_TIMESTAMP_MAX.
 *
 *  \param __p     Port to input from
 *  \param __t     The timestamp to do input on
 *  \param __data  Initial value to shift input data into
 *  \return        Result of shifting \a data right by the port width and storing the
 *                 input data in the most significant bits
 *
 *  \exception  ET_ILLEGAL_RESOURCE   not a valid port.
 *  \exception  ET_RESOURCE_DEP       another core is actively changing the port.
 */
_XCORE_INLINE uint32_t port_in_shift_right_at_time(port_t __p, port_timestamp_t __t, uint32_t __data) _XCORE_NOTHROW
{
  __xcore_port_set_trigger_time(__p, __t);
  return __xcore_port_in_shift_right(__p, __data);
}


/** \brief Input data from a port when its pins match a specific value.
 *
 *  In the case of an unbuffered port, the data inputted
 *  be identical to the value. In the case of a buffered port, an input
 *  will wait until the value appears on the pins and then return that
 *  value and some previous values that have been deserialised.
 *
 *  \param __p      Port to input from
 *  \param __pt     If port is buffered or unbuffered.
 *  \param __value  The value to match against the pins
 *  \return         The inputted data
 *
 *  \exception  ET_ILLEGAL_RESOURCE   not a valid port.
 *  \exception  ET_RESOURCE_DEP       another core is actively changing the port.
 */
_XCORE_INLINE uint32_t port_in_when_pinseq(port_t __p, port_type_t __pt, uint32_t __value) _XCORE_NOTHROW
{
  __xcore_port_set_trigger_in_equal(__p, __value);
  uint32_t __data = __xcore_port_in(__p);
  if (__pt==PORT_UNBUFFERED)
  {
    __xcore_port_clear_trigger_in(__p);
  }
  return __data;
}

/** \brief Input data from a port when its pins do not match a specific value.
 *
 *  In the case of an unbuffered port, the inputted data
 *  will be the non-matching pin values. In the case of a buffered port,
 *  this function will wait until a non matching value appears on the pins, and
 *  then return that value and previous values that have been deserialised.
 *
 *  \param __p      Port to input from
 *  \param __pt     If port is buffered or unbuffered.
 *  \param __value  The value to match against the pins
 *  \return         The inputted data
 *
 *  \exception  ET_ILLEGAL_RESOURCE   not a valid port.
 *  \exception  ET_RESOURCE_DEP       another core is actively changing the port.
 */
_XCORE_INLINE uint32_t port_in_when_pinsneq(port_t __p, port_type_t __pt, uint32_t __value) _XCORE_NOTHROW
{
  __xcore_port_set_trigger_in_not_equal(__p, __value);
  uint32_t __data = __xcore_port_in(__p);
  if (__pt==PORT_UNBUFFERED)
  {
    __xcore_port_clear_trigger_in(__p);
  }
  return __data;
}


/** \brief Input data from a port when its pins match a specific value and shift the data.
 *
 *  In the case of an unbuffered port, the data inputted
 *  be identical to the value. In the case of a buffered port, an input
 *  will wait until the value appears on the pins and then return that
 *  value and some previous values that have been deserialised.
 *
 *  \param __p      Port to input from
 *  \param __pt     If port is buffered or unbuffered.
 *  \param __value  The value to match against the pins
 *  \param __data   Initial value to shift input data into
 *  \return         Result of shifting \a data right by the port width and storing the
 *                  input data in the most significant bits
 *
 *  \exception  ET_ILLEGAL_RESOURCE   not a valid port.
 *  \exception  ET_RESOURCE_DEP       another core is actively changing the port.
 */
_XCORE_INLINE uint32_t port_in_shift_right_when_pinseq(port_t __p, port_type_t __pt, uint32_t __value, uint32_t __data) _XCORE_NOTHROW
{
  __xcore_port_set_trigger_in_equal(__p, __value);
  uint32_t __new_data = __xcore_port_in_shift_right(__p, __data);
  if (__pt==PORT_UNBUFFERED)
  {
    __xcore_port_clear_trigger_in(__p);
  }
  return __new_data;
}

/** \brief Input data from a port when its pins do not match a specific value  and shift the data.
 *
 *  In the case of an unbuffered port, the inputted data
 *  will be the non-matching pin values. In the case of a buffered port,
 *  this macro will wait until a non matching value appears on the pins, and
 *  then return that value and previous values that have been deserialised.
 *
 *  \param __p      Port to input from
 *  \param __pt     If port is buffered or unbuffered.
 *  \param __value  The value to match against the pins
 *  \param __data   Initial value to shift input data into
 *  \return         Result of shifting \a data right by the port width and storing the
 *                  input data in the most significant bits
 *
 *  \exception  ET_ILLEGAL_RESOURCE   not a valid port.
 *  \exception  ET_RESOURCE_DEP       another core is actively changing the port.
 */
_XCORE_INLINE uint32_t port_in_shift_right_when_pinsneq(port_t __p, port_type_t __pt, uint32_t __value, uint32_t __data) _XCORE_NOTHROW
{
  __xcore_port_set_trigger_in_not_equal(__p, __value);
  uint32_t __new_data = __xcore_port_in_shift_right(__p, __data);
  if (__pt==PORT_UNBUFFERED)
  {
    __xcore_port_clear_trigger_in(__p);
  }
  return __new_data;
}

/** \brief Clears the buffer used by a port.
 *
 *  Any data sampled by the port which has not been input by the processor is
 *  discarded. Any data output by the processor which has not been driven by the
 *  port is discarded. If the port is in the process of serialising output, it
 *  is interrupted immediately. If a pending output would have caused a change
 *  in direction of the port then that change of direction does not take place.
 *  If the port is driving a value on its pins when this function is called then
 *  it continues to drive the value until an output statement changes the value
 *  driven.
 *
 *  \param __p  The port whose buffer is to be cleared
 *
 *  \exception  ET_ILLEGAL_RESOURCE   not a valid port.
 *  \exception  ET_RESOURCE_DEP       another core is actively changing the port.
 */
_XCORE_INLINE void port_clear_buffer(port_t __p) _XCORE_NOTHROW
{
  __xcore_port_clear_buffer(__p);
}

/** \brief Ends the current input on a buffered port.
 *
 *  The number of bits sampled by the port but not yet input by the processor is
 *  returned. This count includes both data in the transfer register and data in
 *  the shift register used for deserialisation.
 *  Subsequent inputs on the port return transfer-width bits of data
 *  until there is less than one transfer-width bits of data remaining.
 *  Any remaining data can be read with one further input, which
 *  returns transfer-width bits of data with the remaining buffered data
 *  in the most significant bits of this value.
 *
 *  \param __p  The port to end the current input on
 *  \return     The number of bits of data remaining
 *
 *  \exception  ET_ILLEGAL_RESOURCE   not a valid port.
 *  \exception  ET_RESOURCE_DEP       another core is actively changing the port.
 */
_XCORE_INLINE size_t port_endin(port_t __p) _XCORE_NOTHROW
{
  return __xcore_port_endin(__p);
}

/** \brief Force an input on a buffered port.
 *
 *  Perform an input on a buffered port even if the buffer is only partially full.
 *
 *  \param __p          The port to do the input on
 *  \param[out] __data  The inputted data

 *  \return The number of bits input
 *
 *  \exception  ET_ILLEGAL_RESOURCE   not a valid port.
 *  \exception  ET_RESOURCE_DEP       another core is actively changing the port.
 *  \exception  ET_LOAD_STORE         invalid \a data argument.
 */
_XCORE_INLINE size_t port_force_input(port_t __p, uint32_t *__data) _XCORE_NOTHROW
{
  size_t __num = __xcore_port_endin(__p);
  *__data = __xcore_port_in(__p);
  return __num;
}

/** \brief Sets the trigger value for a port with a configured trigger.
 *
 *  Changes only the trigger value of a port which has already been configured
 *  to trigger on a value using port_set_trigger_in_equal() or port_set_trigger_in_not_equal().
 *
 *  \param __p      The port to change the trigger value of
 *  \param __value  The new trigger value to set
 *
 *  \exception  ET_ILLEGAL_RESOURCE   not a valid port.
 *  \exception  ET_RESOURCE_DEP       another core is actively using the port.
 */
_XCORE_INLINE void port_set_trigger_value(port_t __p, uint32_t __value) _XCORE_NOTHROW
{
  __xcore_port_set_trigger_value(__p, __value);
}

/** \brief Synchronise with the port to ensure all data has been output
 *
 *  Block until all data has been shifted out of the port, with the final port-width
 *  bits being held for one clock period.
 *
 *  \param __p  The port to synchronise
 *
 *  \exception  ET_ILLEGAL_RESOURCE   not a valid port.
 *  \exception  ET_RESOURCE_DEP       another core is actively using the port.
 */
_XCORE_INLINE void port_sync(port_t __p) _XCORE_NOTHROW
{
  __xcore_port_sync(__p);
}

/** \brief Set the port shift count for input and output operations
 *
 *  \param __p  The port to set the shift count for
 *  \param __sc Shift count to set
 *
 *  \exception  ET_ILLEGAL_RESOURCE   Not a valid buffered port, or the shift count is
 *                                    not valid for the port width.
 *  \exception  ET_RESOURCE_DEP       another core is actively using the port.
 *
 */
_XCORE_INLINE void port_set_shift_count(port_t __p, uint32_t __sc) _XCORE_NOTHROW
{
  __xcore_port_shift_count_set(__p, __sc);
}

/** \brief Output a part word to a port
 *
 *  Writes a specified number of bits of a given value to the a buffered port
 *
 *  \param __p     The port to output to
 *  \param __d     The word to take the part word from
 *  \param __bitp  The number of bits of __d to output.
 *
 *  \note This is equivalent to using port_set_shift_count() followed by port_out()
 *
 *  \exception  ET_ILLEGAL_RESOURCE   Not a valid buffered port, the port is busy, or
 *                                    an invalid part-word width has been used.
 *  \exception  ET_RESOURCE_DEP       another core is actively using the port.
 */
_XCORE_INLINE void port_out_part_word(port_t __p, uint32_t __d, uint32_t __bitp) _XCORE_NOTHROW
{
  __xcore_port_output_partial_word(__p, __d, __bitp);
}

/** \brief Compare two port timestamps
 *
 *  Convenience function which checks if a port timestamp \a a is later than another port timestamp \b .
 *  Returns a nonzero value if \a a occurs after \a b, or zero otherwise. Returns 0 if \a a and \a b are equal.
 * 
 *  \param __a Port timestamp to compare to \a __b
 *  \param __b Reference port timestamp
 *
 *  \note Port timestamps have a limited range and can overflow and wrap to 0 (hence they cannot be compared trivially).
 *        A timestamp is considered 'after' another one if it is ahead by no more than half the range of a port timer.
 *  \note Equivalent to XC's \a porttimeafter from \a xs1.h
 *
 */

_XCORE_INLINE int port_time_after(port_timestamp_t __a, port_timestamp_t __b) _XCORE_NOTHROW
{
  return ((int16_t)((__b) - (__a)) < 0);
}

#ifdef __cplusplus
}
#endif

