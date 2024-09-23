/*
 * Copyright (C) XMOS Limited 2008-2019
 *
 * The copyrights, all other intellectual and industrial property rights are
 * retained by XMOS and/or its licensors.
 *
 * The code is provided "AS IS" without a warranty of any kind. XMOS and its
 * licensors disclaim all other warranties, express or implied, including any
 * implied warranty of merchantability/satisfactory quality, fitness for a
 * particular purpose, or non-infringement except to the extent that these
 * disclaimers are held to be legally invalid under applicable law.
 *
 * Version: 15.3.0
 */

#ifndef _xs1_registers_h_
#define _xs1_registers_h_

#if defined(__XS3A__)
  #include <xs3a_registers.h>
#elif defined(__XS2A__)
  #include <xs2a_registers.h>
#elif defined(__XS1B__)
  #include <xs1b_registers.h>
#else
  #error "Unknown architecture"
#endif

#endif /* _xs1_registers_h_ */
