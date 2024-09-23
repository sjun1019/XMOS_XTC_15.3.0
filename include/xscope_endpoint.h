#ifndef _XSCOPE_ENDPOINT_H_
#define _XSCOPE_ENDPOINT_H_

/**
 * \file xscope_endpoint.h
 * \brief xSCOPE host API
 *
 * This file contains functions to communicate with an xSCOPE server.
 */

#ifdef _WIN32
  #define XSCOPE_EP_DLL_EXPORT __declspec(dllexport)
#else
  #define XSCOPE_EP_DLL_EXPORT __attribute__((visibility("default")))
#endif

/// Status code: Function was successful
#define XSCOPE_EP_SUCCESS 0
/// Status code: Function failed
#define XSCOPE_EP_FAILURE 1

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Function pointer which will be called when the target registers new probes.
 * 
 * Probes registered using an xSCOPE config file will trigger this callback upon initial connection.
 * Probes registered via calls to \rsti :c:func:`xscope_register` \endrst will trigger this callback when they are called.
 * 
 * Most of these parameters are configured directly by the target code/config file.
 * The RGB color is currently chosen arbitrarily by the server.
 * 
 * @param id  The unique ID of the probe, which has been allocated by the server.
 * @param type  The type of the probe. See \rsti :c:enum:`xscope_EventType` \endrst
 * @param r  Red color value from 0-255 to visually represent the probe
 * @param g  Green color value from 0-255 to visually represent the probe
 * @param b  Blue color value from 0-255 to visually represent the probe
 * @param name  String representing the name of the probe
 * @param unit  String representing the unit of time being used (e.g. 'ps')
 * @param data_type  Type of the data (signed, unsigned or float). See \rsti :c:enum:`xscope_UserDataType` \endrst
 * @param data_name  String representing the unit of measurement of the probe (e.g. 'mV')
 */
typedef void (*xscope_ep_register_fptr)(unsigned int id, 
                                        unsigned int type,
                                        unsigned int r,
                                        unsigned int g,
                                        unsigned int b,
                                        unsigned char *name,
                                        unsigned char *unit,
                                        unsigned int data_type,
                                        unsigned char *data_name);


/**
 * Function pointer which will be called when a record for a probe is received from the target.
 * 
 * @param id  ID value which has previously been registered with a \ref xscope_ep_register_fptr call
 * @param timestmp  Timestamp of the received record, in the units given in the xscope_ep_register_fptr call
 * @param length  0 if the value received is in dataval, otherwise it is the length of the data in databytes
 * @param dataval  The value received for the record. Only valid if the length is zero.
 *                 The value should be cast based on the data_type argument provided by the \ref xscope_ep_register_fptr call
 * @param databytes  The data buffer received for the record. Only valid if length is nonzero.
 *                   The target can send this kind of message using \rsti :c:func:`xscope_bytes` \endrst
 */
typedef void (*xscope_ep_record_fptr)(unsigned int id,
                                      unsigned long long timestamp,
                                      unsigned int length,
                                      unsigned long long dataval,
                                      unsigned char *databytes);

/**
 * Function pointer which will be called with stats when requested using \ref xscope_ep_request_stats
 * 
 * @warning The server does not implement this request.
 * @param id  Not implemented: always zero
 * @param average  Not implemented: value of 'data' from the server. Always 0xdeadbeef.
 */
typedef void (*xscope_ep_stats_fptr)(int id, unsigned long long average);

/**
 * Function pointer which will be called when the target executes a write syscall (such as a print)
 * 
 * @warning This function gets called for all write syscalls, not just prints to stdout.
 * 
 * @param timestamp  The timestamp of the print record that has been received
 * @param length  The number of characters in the data buffer
 * @param data  Data buffer containing the data the target is writing.
 */
typedef void (*xscope_ep_print_fptr)(unsigned long long timestamp, unsigned int length, unsigned char *data);


/**
 * Function pointer which will be called when \ref xscope_ep_disconnect is called
 * 
 * @warning This does not automatically get called by anything internally.
 *          Only \ref xscope_ep_disconnect calls this when it is called manually.
 */
typedef void (*xscope_ep_exit_fptr)();


/**
 * Register a callback for receiving probe registration information.
 * 
 * @param registration Callback
 * @retval XSCOPE_EP_SUCCESS Success
 * @retval XSCOPE_EP_FAILURE Failure, such as endpoint is already connected.
 */
XSCOPE_EP_DLL_EXPORT int xscope_ep_set_register_cb(xscope_ep_register_fptr registration);

/**
 * Register a callback for receiving probe record data.
 * 
 * @param record Callback
 * @retval XSCOPE_EP_SUCCESS Success
 * @retval XSCOPE_EP_FAILURE Failure, such as endpoint is already connected.
 */
XSCOPE_EP_DLL_EXPORT int xscope_ep_set_record_cb(xscope_ep_record_fptr record);

/**
 * Register a callback for getting statistics.
 * 
 * @warning This system is not implemented.
 * @param stats Callback
 * @retval XSCOPE_EP_SUCCESS Success
 * @retval XSCOPE_EP_FAILURE Failure, such as endpoint is already connected.
 */
XSCOPE_EP_DLL_EXPORT int xscope_ep_set_stats_cb(xscope_ep_stats_fptr stats);

/**
 * Register a callback for receiving data to print to the user.
 * 
 * @warning This callback is not implemented.
 * @param print Callback
 * @retval XSCOPE_EP_SUCCESS Success
 * @retval XSCOPE_EP_FAILURE Failure, such as endpoint is already connected.
 */
XSCOPE_EP_DLL_EXPORT int xscope_ep_set_print_cb(xscope_ep_print_fptr print);

/**
 * Register a callback which will be called when \ref xscope_ep_disconnect is called.
 * 
 * @param exit Callback
 * @retval XSCOPE_EP_SUCCESS Success
 * @retval XSCOPE_EP_FAILURE Failure, such as endpoint is already connected.
 */
XSCOPE_EP_DLL_EXPORT int xscope_ep_set_exit_cb(xscope_ep_exit_fptr exit);

/**
 * Connect to an xSCOPE server which is running and waiting for a client to connect.
 * 
 * @param ipaddr IPv4 address of the xSCOPE server or a host name which will be resolved to one
 * @param port Port of the xSCOPE server
 * @retval XSCOPE_EP_SUCCESS Success
 * @retval XSCOPE_EP_FAILURE Failure, such as endpoint is already connected, or failed to connect.
 */
XSCOPE_EP_DLL_EXPORT int xscope_ep_connect(const char *ipaddr, const char *port);

/**
 * Disconnect from the connected xSCOPE server, and clean up.
 * 
 * @retval XSCOPE_EP_SUCCESS Success
 * @retval XSCOPE_EP_FAILURE Failure
 */
XSCOPE_EP_DLL_EXPORT int xscope_ep_disconnect(void);

// Endpoint request functions

/**
 * No-op, unimplemented.
 * 
 * This is not required. The \ref xscope_ep_register_fptr callback will be called as probe registrations
 * are made, regardless of whether this function is called.
 * 
 * @warning  This is not implemented
 * 
 * @return Status value 
 */
XSCOPE_EP_DLL_EXPORT int xscope_ep_request_registered(void);

/**
 * Request stats from the xSCOPE server, and trigger any registered \ref xscope_ep_stats_fptr callback.
 * 
 * @warning  This is not implemented
 * 
 * @return Status value
 */
XSCOPE_EP_DLL_EXPORT int xscope_ep_request_stats(void);

/**
 * @brief Send data to the target.
 * 
 * This will be received by \rsti :c:func:`xscope_data_from_host` \endrst on the target.
 * 
 * @param length Length of the data buffer, in bytes. Must be 256 bytes or fewer
 * @param data The data buffer to send to the target
 * 
 * @retval XSCOPE_EP_SUCCESS Success
 * @retval XSCOPE_EP_FAILURE Failure, such as data too long, or endpoint not connected.
 */
XSCOPE_EP_DLL_EXPORT int xscope_ep_request_upload(unsigned int length, const unsigned char *data);

#ifdef __cplusplus
}
#endif

#endif
