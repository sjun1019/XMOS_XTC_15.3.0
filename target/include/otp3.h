/**
 *  \file otp.h
 *
 * This API provides functions to program and read the OTP (One Time
 * Programmable) memory of an xCORE. Functions are provided to program and read
 * the OTP memory using both single-ended and differential modes. Functions are
 * also provided that allow automatic repair of failed programming locations
 * using the OTP redundancy registers.  Access is also provided to program and
 * read the OTP special registers.
 */

#ifndef _OTP3_H_
#define _OTP3_H_

#include <xccompat.h>

#ifdef __otp_conf_h_exists__
#include "otp_conf.h"
#endif

////////////////////////////////////////////////////////////////////////////////

/**
 * OTP Statistics gathering is disabled by default.  #define OTP_STATS 1 in
 * user defined otp_conf.h to enable
 */
#ifndef OTP_STATS
#define OTP_STATS 0
#endif //OTP_STATS

/**
 * Maximum size of OTP memory per tile in words.
 * The 8kB OTP is split into two halves, each allocated to a tile.
 */
#define OTP_SIZE (0x800 / 2)

/**
 * Standard initializer for an otp_ports_t structure. Use as follows:
 * on tile[0]: OTPPorts otp_ports = OTP_PORTS_INITIALIZER;
 */
#define OTP_PORTS_INITIALIZER \
{ \
  0, \
  XS1_PORT_32B \
}

////////////////////////////////////////////////////////////////////////////////

/**
 * Structure contains ports used to access OTP memory.
 */
typedef struct OTPPorts
{
  unsigned otp_shadow_g;
  port otp_data_g;
} OTPPorts;

/**
 * If OTP_STATS is set then the OTP library will record statistics
 * during OTP memory programming. These statistics can then be used to
 * determine the success/failure status from the programming event. Stats can
 * the be retrieved using function otpstats_get_stats.
 */
#if (OTP_STATS)
typedef struct OTPStats
{
  unsigned programmed_words;        ///< Number of words that were successfully programmed
  unsigned programmed_bits;         ///< Number of bits that were successfully programmed using the short programming pulse
  unsigned soaked_bits;             ///< Number of bits that were successfully programmed using the long programming pulse
  unsigned leaky_bits;              ///< Number of bits that are already in the programmed state before programming commenced but should not be programmed
  unsigned failed_to_program_words; ///< Number of words that failed to program
  unsigned redundant_sectors_used;  ///< Number of redundant sectors used
} OTPStats;
#endif //OTP_STATS


////////////////////////////////////////////////////////////////////////////////

/**
 * This function programs a block of OTP memory of defined size at address
 * using default single_ended mode. This function will attempt to repair any
 * failures using the redundancy registers.
 *
 * \param[in] ports : OTPPorts - the OTPPorts structure.
 * \param[in] address : unsigned - the location in OTP memory to write to.
 * \param[in] data : const unsigned[] - an array of data to be written.
 * \param[in] size : unsigned - the amount of data to be written.
 * \returns 1 if successful; 0 if failed.
 */
int otp_program(REFERENCE_PARAM(OTPPorts, ports),
                unsigned address,
                const unsigned data[],
                unsigned size);

/**
 * This function programs a block of OTP memory of defined size at address
 * using enhanced differential mode. This function will attempt to repair any
 * failures using the redundancy registers.
 *
 * \param[in] ports : OTPPorts - the OTPPorts structure.
 * \param[in] address : unsigned - the location in OTP memory to write to.
 * \param[in] data : const unsigned[] - an array of data to be written.
 * \param[in] size : unsigned - the amount of data to be written.
 * \returns 1 if successful; 0 if failed.
 */
int otp_program_differential(REFERENCE_PARAM(OTPPorts, ports),
                              unsigned address,
                              const unsigned data[],
                              unsigned size);

/**
 * This function programs a block of OTP memory of defined size at address
 * using default single_ended mode. No attempt to repair unprogrammed/faulty
 * bits will take place using this function however a failure map is returned
 * providing an indication of OTP memory that was not successfully programmed.
 *
 * \param[in] ports : OTPPorts - the OTPPorts structure.
 * \param[in] address : unsigned - the location in OTP memory to write to.
 * \param[in] data : const unsigned[] - an array of data to be written.
 * \param[in] size : unsigned - the amount of data to be written.
 * \param[out] failmap : unsigned [] - location of bits that failed to program
 * successfully.
 * \returns 1 if successful; 0 if failed.
 */
int otp_program_no_repair(REFERENCE_PARAM(OTPPorts, ports),
                          unsigned address,
                          const unsigned data[],
                          unsigned size,
                          char failmap[]);

/**
 * This function programs a block of OTP memory of defined size at address
 * using enhanced differential mode. No attempt to repair unprogrammed/faulty
 * bits will take place using this function however a failure map is returned
 * providing an indication of OTP memory that was not successfully programmed.
 *
 * \param[in] ports : OTPPorts - the OTPPorts structure.
 * \param[in] address : unsigned - the location in OTP memory to write to.
 * \param[in] data : const unsigned[] - an array of data to be written.
 * \param[in] size : unsigned - the amount of data to be written.
 * \param[out] failmap : unsigned [] - location of bits that failed to program
 * successfully.
 * \returns 1 if successful; 0 if failed.
 */
int otp_program_differential_no_repair(REFERENCE_PARAM(OTPPorts, ports),
                                        unsigned address,
                                        const unsigned data[],
                                        unsigned size,
                                        char failmap[]);

/**
 * otp_program_special_register programs a word to the OTP special register.
 *
 * \param[in] ports : OTPPorts - the OTPPorts structure.
 * \param[in] data : unsigned - the data to be written.
 * \returns 1 if successful; 0 if failed.
 */
int otp_program_special_register(REFERENCE_PARAM(OTPPorts, ports), unsigned data);

/**
 * otp_program_secure_config_register programs a word to the OTP secure config 
 * register.
 *
 * \param[in] ports : OTPPorts - the OTPPorts structure.
 * \param[in] data : unsigned - the data to be written.
 * \returns 1 if successful; 0 if failed.
 */
int otp_program_secure_config_register(REFERENCE_PARAM(OTPPorts, ports), unsigned data);

//Program a single word without repairing any errors
int otpprogram_writeword_norepair(REFERENCE_PARAM(OTPPorts, ports),
                                  unsigned address,
                                  unsigned data);

/**
 * otp_read reads a block of OTP data of defined size from address using
 * default single_ended mode.
 *
 * \param[in] ports : OTPPorts - the OTPPorts structure.
 * \param[in] address : unsigned - the location in OTP memory to read from.
 * \param[out] data : unsigned[] - an array of words that the data being
 *  read can be stored in.
 * \param[in] size : unsigned - the amount of data to be read.
 */
void otp_read(REFERENCE_PARAM(OTPPorts, ports),
              unsigned address,
              unsigned data[],
              unsigned size);

/**
 * otp_read_differential reads a block of OTP data of defined size from address
 * using enhanced differential mode.
 *
 * \param[in] ports : OTPPorts - the OTPPorts structure.
 * \param[in] address : unsigned - the location in OTP memory to read from.
 * \param[out] data : unsigned[] - an array of words that the data being read
 *  can be stored in.
 * \param[in] size : unsigned - the amount of data to be read.
 */
void otp_read_differential(REFERENCE_PARAM(OTPPorts, ports),
                            unsigned address,
                            unsigned data[],
                            unsigned size);

/**
 * otp_read_special_register reads a word from the OTP special register.
 *
 * \param[in] ports : OTPPorts - the OTPPorts structure.
 * \param[out] data : unsigned - the word read from the special register.
 */
void otp_read_special_register(REFERENCE_PARAM(OTPPorts, ports), REFERENCE_PARAM(unsigned, data));

/**
 * otp_read_secure_config_register reads a word from the OTP secure config 
 * register.
 *
 * \param[in] ports : OTPPorts - the OTPPorts structure.
 * \param[out] data : unsigned - the word read from the special register.
 */
void otp_read_secure_config_register(REFERENCE_PARAM(OTPPorts, ports), REFERENCE_PARAM(unsigned, data));

////////////////////////////////////////////////////////////////////////////////

#if (OTP_STATS)

/**
 * This function retrieves the statistics from the last programming event.
 * Requires the #define OTP_STATS to be set to 1 in user defined otp_conf.h
 * to activate statistic gathering.
 *
 * \param[out] stats : OTPStats - structure that will contain the statitic
 * information
 */
void otpstats_get_stats(REFERENCE_PARAM(OTPStats, stats));

/**
 * This function allows two OTPStats structs to be merged together.
 * Requires the #define OTP_STATS to be set to 1 in user defined otp_conf.h
 * to activate statistic gathering.
 *
 * \param[in, out] a : OTPStats - structure contains the original stats and
 *  param b will be merged with them
 * \param[in] b : OTPStats - structure contains the stats that are to be merged
 * with param a
 */
void otpstats_merge_stats(REFERENCE_PARAM(OTPStats, a),
                          REFERENCE_PARAM(const OTPStats, b));

/**
 * This function outputs the statistics from the given OTPStats struct.
 * Requires the #define OTP_STATS to be set to 1 in user defined otp_conf.h
 * to activate statistic gathering.
 *
 * \param[in] stats : OTPStats - structure containing the stats to be printed.
 *
 */
void otpstats_print_stats(REFERENCE_PARAM(const OTPStats, stats));

#endif //OTP_STATS

////////////////////////////////////////////////////////////////////////////////

#endif //_OTP3_H_

////////////////////////////////////////////////////////////////////////////////
