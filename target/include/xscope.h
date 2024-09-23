#ifndef XSCOPE_H_
#define XSCOPE_H_

/**
 * \file xscope.h
 * \brief xSCOPE interface
 *
 * This file contains functions to access xSCOPE.
 * Example:
\code
#include <platform.h>
#include <xscope.h>
#include <xccompat.h>

void xscope_user_init(void) {
  xscope_register(1, XSCOPE_CONTINUOUS, "Continuous Value 1", XSCOPE_UINT, "Value");
}

int main (void) {
  par {
    on tile[0]: {
      for (int i = 0; i < 100; i++) {
        xscope_int(0, i*i);
      }
    }
  }
  return 0;
}
\endcode
*/

/**
 * User-implemented function to allow xSCOPE event registration. 
 * This allows the code on the device to synchronize with the host.
 * This may be defined anywhere in the application code and (if present) will be called before main().
 * It can be used to register probes which can then be used later in the application with the other functions
 * defined in this header.
 * 
 * \note A weak stub version of this function is defined, but it is intended to be overridden by a user implementation.
 */
void xscope_user_init();

/** Kind of event that an xSCOPE probe can receive. */
typedef enum {
  XSCOPE_STARTSTOP=1, /**< Start/Stop - Event gets a start and stop value representing a block of execution */
  XSCOPE_CONTINUOUS, /**<  Continuous - Only gets an event start, single timestamped "ping" */
  XSCOPE_DISCRETE, /**<  Discrete - Event generates a discrete block following on from the previous event */
  XSCOPE_STATEMACHINE, /**<  State Machine - Create a new event state for every new data value */
  XSCOPE_HISTOGRAM, /** Histogram - Unimplemented. */
} xscope_EventType;

/** C data types an xSCOPE probe can receive. */
typedef enum {
  XSCOPE_NONE=0, /**< No user data */
  XSCOPE_UINT, /**< Unsigned int user data */
  XSCOPE_INT, /**< Signed int user data */
  XSCOPE_FLOAT, /**< Floating point user data */
} xscope_UserDataType;

/** Methods of I/O redirection over the xSCOPE connection. This applies to all write syscalls. */
typedef enum {
  XSCOPE_IO_NONE=0, /**< Output is not redirected */
  XSCOPE_IO_BASIC, /**< Output is redirected over xSCOPE */
  XSCOPE_IO_TIMED, /**< Output is redirected over xSCOPE, with prepended timestamp */
} xscope_IORedirectionMode;

/**
 * Registers the trace probes with the host system.
 * First parameter is the number of probes that will be registered. Further parameters are in groups of four.
 * -# event type (\link #xscope_EventType \endlink)
 * -# probe name
 * -# user data type (\link #xscope_UserDataType \endlink)
 * -# user data name.
 * 
 * This must be called from inside the xscope_user_init() function.
 *
 * Examples:
 * \code
 *    xscope_register(1, XSCOPE_DISCRETE, "A probe", XSCOPE_UINT, "value"); ``
 *    xscope_register(2, XSCOPE_CONTINUOUS, "Probe", XSCOPE_FLOAT, "Level",
 *                       XSCOPE_STATEMACHINE, "State machine", XSCOPE_NONE, "no name");
 * \endcode
 * \param num_probes Number of probes that will be specified.
 * 
 * Note that the 'id' of each of these probes is implicit, starting from 0 and incrementing
 * by 1 for each additional probe registered. In the above example:
 * 
 * - "A probe" would have id 0.
 * - "Probe" would have id 1.
 * - "State machine" would have id 2.
 * 
 * If an  \rsti :ref:`config_xscope_format` \endrst file is specified on the compile line, that takes priority, so this call
 * will be ignored.
 * 
 * For these reasons, it is recommended to not use this function, and instead use the \rsti :ref:`config_xscope_format` \endrst
 * on the command line to allocate probes. xSCOPE config file probe IDs are
 * available as preprocessor constants in the compiled program.
 */
void xscope_register(int num_probes, ...);

/**
 * Enable the xSCOPE event capture on the local xCORE tile. This is not necessary if an \rsti :ref:`config_xscope_format` \endrst is used to enable xSCOPE.
 */
void xscope_enable();

/**
 * Disable the xSCOPE event capture on the local xCORE tile.
 */
void xscope_disable();

/**
 * Configures xSCOPE I/O redirection.
 * This is not necessary if an \rsti :ref:`config_xscope_format` \endrst is used to enable xSCOPE io.
 * \param mode I/O redirection mode. See \ref xscope_IORedirectionMode
 */
void xscope_config_io(unsigned int mode);

/**
 * Generate an xSCOPE ping system timestamp event
 */
void xscope_ping();

/**
 * \defgroup xscope_trace_functions Functions for sending data to the host
 * @{
 */

/**
 * Send a trace event for the specified xSCOPE probe of type char.
 * \param id xSCOPE probe id.
 * \param data User data value (char).
 */
void xscope_char(unsigned char id, unsigned char data);
#ifndef XSCOPE_IMPL
#define xscope_char(id, data) do { if ((id) != -1) xscope_char(id, data); } while (0)
#endif
/**
 * Send a trace event for the specified xSCOPE probe of type short.
 * \param id xSCOPE probe id.
 * \param data User data value (short).
 */
void xscope_short(unsigned char id, unsigned short data);
#ifndef XSCOPE_IMPL
#define xscope_short(id, data) do { if ((id) != -1) xscope_short(id, data); } while (0)
#endif

/**
 * Send a trace event for the specified xSCOPE probe of type int.
 * \param id xSCOPE probe id.
 * \param data User data value (int).
 */
void xscope_int(unsigned char id, unsigned int data);
#ifndef XSCOPE_IMPL
#define xscope_int(id, data) do { if ((id) != -1) xscope_int(id, data); } while (0)
#endif

/**
 * Send a trace event for the specified xSCOPE probe of type long long.
 * \param id xSCOPE probe id.
 * \param data User data value (long long).
 */
void xscope_longlong(unsigned char id, unsigned long long data);
#ifndef XSCOPE_IMPL
#define xscope_longlong(id, data) do { if ((id) != -1) xscope_longlong(id, data); } while (0)
#endif

/**
 * Send a trace event for the specified xSCOPE probe of type float.
 * \param id xSCOPE probe id.
 * \param data User data value (float).
 */
void xscope_float(unsigned char id, float data);
#ifndef XSCOPE_IMPL
#define xscope_float(id, data) do { if ((id) != -1) xscope_float(id, data); } while (0)
#endif

/**
 * Send a trace event for the specified xSCOPE probe of type double.
 * \param id xSCOPE probe id.
 * \param data User data value (double).
 */
void xscope_double(unsigned char id, double data);
#ifndef XSCOPE_IMPL
#define xscope_double(id, data) do { if ((id) != -1) xscope_double(id, data); } while (0)
#endif

/**
 * Send a trace event for the specified xSCOPE probe with a byte array.
 * \param id xSCOPE probe id.
 * \param size User data size.
 * \param data User data bytes (char[]).
 */
void xscope_bytes(unsigned char id, unsigned int size, const unsigned char data[]);
#ifndef XSCOPE_IMPL
#define xscope_bytes(id, size, data) do { if ((id) != -1) xscope_bytes(id, size, data); } while (0)
#endif

/**
 * Start a trace block for the specified xSCOPE probe.
 * For use with XSCOPE_STARTSTOP probes.
 * \param id xSCOPE probe id.
 */
void xscope_start(unsigned char id);
#ifndef XSCOPE_IMPL
#define xscope_start(id) do { if ((id) != -1) xscope_start(id); } while (0)
#endif

/**
 * Stop a trace block for the specified xSCOPE probe.
 * For use with XSCOPE_STARTSTOP probes.
 * \param id xSCOPE probe id.
 */
void xscope_stop(unsigned char id);
#ifndef XSCOPE_IMPL
#define xscope_stop(id) do { if ((id) != -1) xscope_stop(id); } while (0)
#endif

/**
 * Start a trace block for the specified xSCOPE probe and capture a value of type int.
 * For use with XSCOPE_STARTSTOP probes.
 * \param id xSCOPE probe id.
 * \param data User data value (int).
 */
void xscope_start_int(unsigned char id, unsigned int data);
#ifndef XSCOPE_IMPL
#define xscope_start_int(id, data) do { if ((id) != -1) xscope_start_int(id, data); } while (0)
#endif

/**
 * Stop a trace block for the specified xSCOPE probe and capture a value of type int.
 * For use with XSCOPE_STARTSTOP probes.
 * \param id xSCOPE probe id.
 * \param data User data value (int).
 */
void xscope_stop_int(unsigned char id, unsigned int data);
#ifndef XSCOPE_IMPL
#define xscope_stop_int(id, data) do { if ((id) != -1) xscope_stop_int(id, data); } while (0)
#endif

/**
 * Send a trace event for the specified xSCOPE probe of type char with logical core info.
 * \param id xSCOPE probe id.
 * \param data User data value (char).
 */
void xscope_core_char(unsigned char id, unsigned char data);
#ifndef XSCOPE_IMPL
#define xscope_core_char(id, data) do { if ((id) != -1) xscope_core_char(id, data); } while (0)
#endif

/**
 * Send a trace event for the specified xSCOPE probe of type short with logical core info.
 * \param id xSCOPE probe id.
 * \param data User data value (short).
 */
void xscope_core_short(unsigned char id, unsigned short data);
#ifndef XSCOPE_IMPL
#define xscope_core_short(id, data) do { if ((id) != -1) xscope_core_short(id, data); } while (0)
#endif

/**
 * Send a trace event for the specified xSCOPE probe of type int with logical core info.
 * \param id xSCOPE probe id.
 * \param data User data value (int).
 */
void xscope_core_int(unsigned char id, unsigned int data);
#ifndef XSCOPE_IMPL
#define xscope_core_int(id, data) do { if ((id) != -1) xscope_core_int(id, data); } while (0)
#endif

/**
 * Send a trace event for the specified xSCOPE probe of type long long with logical core info.
 * \param id xSCOPE probe id.
 * \param data User data value (long long).
 */
void xscope_core_longlong(unsigned char id, unsigned long long data);
#ifndef XSCOPE_IMPL
#define xscope_core_longlong(id, data) do { if ((id) != -1) xscope_core_longlong(id, data); } while (0)
#endif

/**
 * Send a trace event for the specified xSCOPE probe of type float with logical core info.
 * \param id xSCOPE probe id.
 * \param data User data value (float).
 */
void xscope_core_float(unsigned char id, float data);
#ifndef XSCOPE_IMPL
#define xscope_core_float(id, data) do { if ((id) != -1) xscope_core_float(id, data); } while (0)
#endif

/**
 * Send a trace event for the specified xSCOPE probe of type double with logical core info.
 * \param id xSCOPE probe id.
 * \param data User data value (double).
 */
void xscope_core_double(unsigned char id, double data);
#ifndef XSCOPE_IMPL
#define xscope_core_double(id, data) do { if ((id) != -1) xscope_core_double(id, data); } while (0)
#endif

/**
 * Send a trace event for the specified xSCOPE probe with a byte array with logical core info.
 * \param id xSCOPE probe id.
 * \param size User data size.
 * \param data User data bytes (char[]).
 */
void xscope_core_bytes(unsigned char id, unsigned int size, const unsigned char data[]);
#ifndef XSCOPE_IMPL
#define xscope_core_bytes(id, size, data) do { if ((id) != -1) xscope_core_bytes(id, size, data); } while (0)
#endif

/**
 * Start a trace block for the specified xSCOPE probe with logical core info.
 * For use with XSCOPE_STARTSTOP probes.
 * \param id xSCOPE probe id.
 */
void xscope_core_start(unsigned char id);
#ifndef XSCOPE_IMPL
#define xscope_core_start(id) do { if ((id) != -1) xscope_core_start(id); } while (0)
#endif

/**
 * Stop a trace block for the specified xSCOPE probe with logical core info.
 * For use with XSCOPE_STARTSTOP probes.
 * \param id xSCOPE probe id.
 */
void xscope_core_stop(unsigned char id);
#ifndef XSCOPE_IMPL
#define xscope_core_stop(id) do { if ((id) != -1) xscope_core_stop(id); } while (0)
#endif


/**
 * Start a trace block for the specified xSCOPE probe with logical core info and capture a value of type int
 * For use with XSCOPE_STARTSTOP probes.
 * \param id xSCOPE probe id.
 * \param data User data value (int).
 */
void xscope_core_start_int(unsigned char id, unsigned int data);
#ifndef XSCOPE_IMPL
#define xscope_core_start_int(id, data) do { if ((id) != -1) xscope_core_start_int(id, data); } while (0)
#endif


/**
 * Stop a trace block for the specified xSCOPE probe with logical core info and capture a value of type int
 * For use with XSCOPE_STARTSTOP probes.
 * \param id xSCOPE probe id.
 * \param data User data value (int).
 */
void xscope_core_stop_int(unsigned char id, unsigned int data);
#ifndef XSCOPE_IMPL
#define xscope_core_stop_int(id, data) do { if ((id) != -1) xscope_core_stop_int(id, data); } while (0)
#endif

///@}

/**
 * Put xSCOPE into a lossless mode where timing is no longer guaranteed.
 * If the logical core tries to send an xSCOPE packet which the xTAG does not have buffers for, then the
 * logical core will wait until the xTAG does have buffers, stalling for a non-deterministic amount of time.
 * \sa xscope_mode_lossy
 */
void xscope_mode_lossless();

/**
 * Put xSCOPE into a lossy mode where timing is not impacted, but data is lossy.
 * This is the default xSCOPE mode.
 * If the logical core tries to send an xSCOPE packet which the xTAG does not have buffers for, then the entire
 * packet will be dropped.
 * \sa xscope_mode_lossless
 */
void xscope_mode_lossy();

/**
 * Receive data from the host over xSCOPE.
 * This function receives an xSCOPE packet from the host and puts the data contained within it
 * into the supplied buffer. See \rsti :ref:`working_with_targets_example_host_communication` \endrst for an example.
 * \param c The xSCOPE chanend which has been configured with xscope_connect_data_from_host
 * \param buf The user-supplied buffer to fill with data from the host
 * \param n A pointer(c) or reference(xc) to an int which will be filled with the number of bytes put into the buffer
 * \sa xscope_connect_data_from_host
 */
#ifdef __XC__
#pragma select handler
void xscope_data_from_host(chanend c, char buf[256], int &n);
#else
void xscope_data_from_host(unsigned int c, char buf[256], int *n);
#endif

/**
 * Connect to the xSCOPE chanend to receive data packets from the host.
 * Note that it is only possible to use this function once in the system, as there is
 * only a single xSCOPE chanend available to receive from the host. i.e. it can only be
 * called on a single tile. See \rsti :ref:`working_with_targets_example_host_communication` \endrst for an example.
 * \param from_host The chanend to receive on, obtained using the xscope_host_data() call from a multi-tile main.
 * \sa xscope_data_from_host
 */
#ifdef __XC__
void xscope_connect_data_from_host(chanend from_host);
#else
void xscope_connect_data_from_host(unsigned int from_host);
#endif

/* Probe enabled macro */
#define XSCOPE_PROBE_ENABLED(x) ((x) != -1)

/* Backwards compatibility */

#ifdef __XC__
#define xscope_probe(id) _Pragma("warning \"xscope_probe is deprecated, use xscope_char instead\"") xscope_char(id, 0)
#else
__attribute__((deprecated)) static inline void xscope_probe(unsigned char id)
{
  xscope_char(id, 0);
}
#endif

#ifdef __XC__
#define xscope_probe_data(id, data) _Pragma("warning \"xscope_probe_data is deprecated, use xscope_int instead\"") xscope_int(id, data)
#else
__attribute__((deprecated)) static inline void xscope_probe_data(unsigned char id, unsigned int data)
{
  xscope_int(id, data);
}
#endif

#ifdef __XC__
#define xscope_probe_data_pred(id, data) _Pragma("warning \"xscope_probe_data_pred is deprecated, use xscope_int instead\"") xscope_int(id, data)
#else
__attribute__((deprecated)) static inline void xscope_probe_data_pred(unsigned char id, unsigned int data)
{
  xscope_int(id, data);
}
#endif

#ifdef __XC__
#define xscope_probe_cpu(id) _Pragma("warning \"xscope_probe_cpu is deprecated, use xscope_core_char instead\"") xscope_core_char(id, 0)
#else
__attribute__((deprecated)) static inline void xscope_probe_cpu(unsigned char id)
{
  xscope_core_char(id, 0);
}
#endif

#ifdef __XC__
#define xscope_probe_cpu_data(id, data) _Pragma("warning \"xscope_probe_cpu_data is deprecated, use xscope_core_int instead\"") xscope_core_int(id, data)
#else
__attribute__((deprecated)) static inline void xscope_probe_cpu_data(unsigned char id, unsigned int data)
{
  xscope_core_int(id, data);
}
#endif

/* This section includes the autogenerated probe definitions
   from .xscope  files */
#ifdef _XSCOPE_PROBES_INCLUDE_FILE
#include _XSCOPE_PROBES_INCLUDE_FILE
#endif

#endif /* XSCOPE_H_ */
