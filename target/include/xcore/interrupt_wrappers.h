// Copyright (c) 2016-2024, XMOS Ltd, All rights reserved
#pragma once

/** \file
 *  \brief Helpers for implementing interrupt-enabled functions.
 */

#include <xs1.h>
#include <xcore/_support/xcore_common.h>
#include <xcore/_support/xcore_interrupt_wrappers_impl.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef LIBXCORE_KSTACK_WORDS
/** \brief Specify the minimum kernel stack size the interrupt permitting function should create.
 *
 *  The user may specify a minimum kstack size by setting the \ref LIBXCORE_KSTACK_WORDS
 *  define in their Makefile.
 *  This should be done when the kstack is being used by more than interrupt_callback_t functions.
 *
 *  \showinitializer
 */ 
#define LIBXCORE_KSTACK_WORDS 0
#endif

/** \brief Define a function that allows interrupts to occur within its scope
 *
 *  This macro will define two functions for you:
 *    - An ordinary function that may be called directly
 *      Its signature will be <tt>\a ret \a root_function ( \a ... )</tt>
 *    - A function that will also reserve space for and set up a stack
 *      for handling interrupts.
 *      The function name is accessed using the INTERRUPT_PERMITTED() macro
 *
 *  You would normally use this macro on the definition of the root function
 *  which will be called in a par statement.
 *  The interrupt stack (kernel stack) is created on the core's stack with the
 *  ksp and sp being modified as necessary. When the functions exits, neither the
 *  kernel stack nor ksp is valid.
 *
 *  The kernel stack allocated has enough space for the interrupt_callback_t
 *  function (+callees) in the given \a group. The use of the \a group identifier
 *  allows a kernel stack to be no larger than that required by its greediest member.
 *
 *  \warning The kernel stack is not re-entrant so kernel mode must not be masked
 *  from within an interrupt_callback_t
 *
 *  The user may specify a larger kernel stack by defining LIBXCORE_KSTACK_WORDS.
 *
 *  Example usage: \code
 *    DEFINE_INTERRUPT_PERMITTED(groupA, int, rootfunc, chanend c, int i)
 *    {
 *      // This is the body of 'int rootfunc(chanend c, int i)'
 *    }
 *  \endcode
 *
 *  \param __group          this is the group of interrupt_callback_t function
 *                          that may be safely enabled - see DEFINE_INTERRUPT_CALLBACK()
 *  \param __ret            the return type of the ordinary function; the return type must be a
 *                          non-union, non-aggregate type not larger than \c int
 *  \param __root_function  the name of the ordinary function
 *  \param ...              the arguments of the ordinary function; there may be no more than
 *                          4 parameters, and they must each have non-union, non-aggregate
 *                          type not larger than \c int
 *  \hideinitializer
 */
#define DEFINE_INTERRUPT_PERMITTED(__group, __ret, __root_function, ...) \
        _XCORE_DEFINE_INTERRUPT_PERMITTED(__group, __ret, __root_function, __VA_ARGS__)

/** \brief Declare an interrupt permitting function
 *
 *  Use this macro when you require a declaration of your interrupt permitting function types
 *
 *  Example usage: \code
 *    // In another file:
 *    //   DEFINE_INTERRUPT_PERMITTED(groupA, int, rootfunc, chanend c, int i)
 *    //   DEFINE_INTERRUPT_PERMITTED(groupB, void, anotherfunc, void)
 *    DECLARE_INTERRUPT_PERMITTED(int, rootfunc, chanend c, int i);
 *    DECLARE_INTERRUPT_PERMITTED(void, anotherfunc, void);
 *    ...
 *      par {
 *          int ret = INTERRUPT_PERMITTED(rootfunc)(c,i);  // kstack for groupA.
 *          INTERRUPT_PERMITTED(anotherfunc)();  // kstack for groupB.
 *  \endcode
 *
 *  \param __ret            the return type of the ordinary function
 *  \param __root_function  the name of the ordinary function; the return type must be a non-union,
 *                          non-aggregate type not larger than \c int
 *  \param ...              the arguments of the ordinary function; there may be no more than
 *                          4 parameters, and they must each have non-union, non-aggregate
 *                          type not larger than \c int
 *  \hideinitializer
 */
#define DECLARE_INTERRUPT_PERMITTED(__ret, __root_function, ...) \
        _XCORE_DECLARE_INTERRUPT_PERMITTED(__ret, __root_function, __VA_ARGS__)

/** \brief The name of the defined interrupt permitting function
 *
 *  Use this macro for retriving the name of the declared interrupt function.
 *  This is the name used to invoke the function.
 *
 *  \return the name of the defined interrupt permitting function
*   \hideinitializer
 */
#define INTERRUPT_PERMITTED(__root_function) _XCORE_INTERRUPT_PERMITTED(__root_function)

/** \brief Define an interrupt handling function
 *
 *  This macro will define two functions for you:
 *    - An ordinary function that may be called directly
 *      Its signature will be <tt>void \a intrpt ( void* \a data )</tt>
 *    - An interrupt_callback_t function for passing to a res_setup_interrupt_callback function.
 *      The interrupt_callback_t function name is accessed using the INTERRUPT_CALLBACK() macro
 *
 *  \warning The kernel stack is not re-entrant so kernel mode must not be masked
 *  from within an interrupt_callback_t
 *
 *  Example usage: \code
 *    DEFINE_INTERRUPT_CALLBACK(groupA, myfunc, arg)
 *    {
 *      // This is the body of 'void myfunc(void* arg)'
 *    }
 *  \endcode
 *
 *  \param __group   the group of interrupt_callback_t function we belong to
 *                   see DEFINE_INTERRUPT_PERMITTED()
 *  \param __intrpt  this is the name of the ordinary function
 *  \param __data    the name to use for the void* argument
 *  \hideinitializer
 */
#define DEFINE_INTERRUPT_CALLBACK(__group, __intrpt, __data) \
        _XCORE_DEFINE_INTERRUPT_CALLBACK(__group, __intrpt, __data)

/** \brief Declare an interrupt handling function
 *
 *  Use this macro when you require a declaration of your interrupt function types
 *
 *  Example usage: \code
 *    DECLARE_INTERRUPT_CALLBACK(myfunc, arg);
 *    chanend_setup_interrupt_callback(c, 0 , INTERRUPT_CALLBACK(myfunc));
 *  \endcode
 *
 *  \param __intrpt  this is the name of the ordinary function
 *  \param __data    the name to use for the void* argument
 *  \hideinitializer
 */
#define DECLARE_INTERRUPT_CALLBACK(__intrpt, __data) \
        _XCORE_DECLARE_INTERRUPT_CALLBACK(__intrpt, __data)

/** \brief The name of the defined \c interrupt_callback_t function
 *
 *  Use this macro for retriving the name of the declared interrupt callback function.
 *  This is the name that is passed to \c *_setup_interrupt_callback() for registration.
 *
 *  \return the name of the defined interrupt_callback_t function
 *  \hideinitializer
 */
#define INTERRUPT_CALLBACK(__intrpt) _XCORE_INTERRUPT_CALLBACK(__intrpt)

#ifdef __cplusplus
}
#endif

