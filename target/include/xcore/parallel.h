// Copyright (c) 2020-2024, XMOS Ltd, All rights reserved
#pragma once

/** \file
 * \brief Helper macros for fork/join style parallel execution
 */

#include <xcore/thread.h>
#include <xcore/_support/xcore_common.h>
#include <xcore/_support/xcore_parallel_impl.h>

#ifdef __cplusplus
extern "C" {
#endif

/** \typedef synchronizer_t
 *  \brief Synchronizer handle which may be used to explicitly synchronise jobs executed by PAR_JOBS.
 *  \warning Synchronizer handles are not portable between threads; they are meaningful only within
 *           the thread to which they are originally passed by a \ref PAR_JOBS expansion.
 *  \note  The synchronizer handles for each thread in a \ref PAR_JOBS are neither guaranteed to
 *         compare equal nor non-equal to other handles, regardless of whether they are
 *         participating in the same \ref PAR_JOBS.
 */
typedef resource_t synchronizer_t;

/** \brief Synchronise all threads in a PAR_JOBS
 *
 * When passed a value of type \ref synchronizer_t, as obtained using \ref PAR_SYNC as a parameter
 * to the \ref PAR_JOBS which started the current thread, blocks until all threads started by that
 * \ref PAR_JOBS call \ref par_jobs_synchronize with their respective \ref synchronizer_t values.
 * If all other threads started by the \ref PAR_JOBS are already blocking in a call to
 * \ref par_jobs_synchronize then causes all of those threads to return from those calls, and 
 * returns immediately.
 *
 * If the synchronizer passed was not obtained by expanding \ref PAR_SYNC in the parameter pack
 * of the \ref PJOB expansion relating to the calling thread, the behaviour is undefined. That is,
 * synchronizers must not be passed between threads, including to child threads.
 *
 * If any thread calls \ref par_jobs_synchronize when any other job in the respective \ref PAR_JOBS
 * has already returned, or if any job returns whilst another job in its respective \ref PAR_JOBS
 * is blocked on a call to \ref par_jobs_synchronize, then the behaviour is undefined. Effectively,
 * each job in a \ref PAR_JOBS expansion must call \ref par_jobs_synchronize the same number of times
 *  
 * \param __s The synchronizer handle for this thread
 */
_XCORE_INLINE void par_jobs_synchronize(const synchronizer_t __s) _XCORE_NOTHROW
{
  if (__s != _XCORE_NULL_SYNCHRONIZER_HANDLE)
  {
    __xcore_thread_group_sync_parent(__s);
  }
  else
  {
    __xcore_thread_group_sync_child();
  }
}

/** \brief Calls a list of <tt>void (void*)</tt> functions with each utilising a different hardware thread
 *
 * Expands to a block which evaluates each of the given function call descriptions such that each
 * runs in a different thread. For each \c PFUNC, a stack will be allocated from the current stack
 * and used to call \c FUNCTION with \c ARGUMENT as its sole argument.
 * The block is blocking until all function calls have returned.
 * \note One function call will be evaluated in the current thread.
 *
 * There must be at least enough unallocated logical cores to execute each PFUNC in its own
 * thread, including the current thread as one available logical core.
 *
 * \param ...  Functions to call, each the result of expanding \c PFUNC
 *
 * Example: \code
 * PAR_FUNCS(PFUNC(my_print_function, "Hello 1"),
 *           PFUNC(my_print_function, "Hello 2"));
 * \endcode
 * \see PFUNC
 * \hideinitializer
 */
#define PAR_FUNCS(...) _XCORE_PAR_FUNCS(_XCORE_UNIQUE_LABEL(_Par), __VA_ARGS__)

/** \brief Specifies a parallelised function call
 *
 * Expands to a function call description which can be used as an argument to PAR_FUNCS.
 *  \param _FUNCTION    a function with signature <tt>void (void *)</tt>
 *  \param _ARGUMENT    the argument to pass to \a _FUNCTION - must be implicitly convertible to <tt>void *</tt>
 * \see PAR_FUNCS
 * \hideinitializer
 */
#define PFUNC(_FUNCTION, _ARGUMENT) (_FUNCTION, _ARGUMENT)

/** \brief Declare a \c void function with arbitrary parameters which can be dispatched in another thread
 *
 * Expands to the declaration of a function named \a _NAME and additional definitions required to
 * execute that function in a different thread.
 * \param _NAME            Name of function to declare
 * \param _ARG_TYPES_PACK  A pack of type specifiers which form the parameter part of the function 
 *                         signature (must not include names)
 *
 * The \a _ARG_TYPES_PACK parameter must be a comma-separated sequence of permitted types; permitted types may be:
 *  \li  A type specifier with optional qualification (e.g. <tt>int</tt>, <tt>const unsigned</tt>, <tt>enum my_enum</tt>).
 *       In the case of an enum, union or struct a declaration list must not be included.
 *  \li An (optionally qualified) pointer to a permitted type.
 *
 * \note These restrictions mean that array types and pointer-to-function types may not appear
 *       in a parameter pack. However, typedefs of these types are allowed.
 *   
 *  All parameter types must be complete at the point where DECLARE_JOB() is expanded, unless the first
 *  parameter type is \c void in which case this must be the only parameter and the parameter pack must
 *  be exactly \c (void) (i.e. an alias of \c void must not be used).
 *
 * Example: \code
 * DECLARE_JOB(thread_sum, (const unsigned *, size_t, unsigned long *));
 * void thread_sum(const unsigned * const ints, const size_t number, unsigned long * const result) {
 *   unsigned long sum = 0;
 *   for (size_t i = 0; i < number; i += 1) { sum += ints[i]; }
 *   *result = sum;
 * }
 * \endcode
 * \see \li PJOB \li PAR_JOBS
 * \hideinitializer
 */
#define DECLARE_JOB(_NAME, _ARG_TYPES_PACK) \
  _XCORE_DECLARE_JOB(_NAME, \
                       _XCORE_PAR_ARG_STRUCT_NAME(_NAME), \
                       _XCORE_PAR_ARG_PACK_PREPARE(_ARG_TYPES_PACK), \
                       _ARG_TYPES_PACK)

/** \brief Calls, in parallel, a list of functions declared using DECLARE_JOB
 *
 * Expands to a block which executes each of the given function call specifications such that each
 * call takes place in a different hardware thread. Each call specification should be the result of
 * expanding PJOB and the \c FUNCTION should be one which was declared using DECLARE_JOB.
 * Each thread's stack will be allocated from the current stack. Each function will execute as if
 * \c FUNCTION \c ARGPACK; had been executed in its assigned thread.
 * The block is blocking until all functions have returned.
 * \note One function call will be evaluated in the current thread.
 *
 * There must be at least enough unallocated logical cores to execute each PJOB in its own
 * thread, including the current thread as one available logical core.
 *
 * \param ...  Function call specifications as expanded from PJOB.
 *
 * Example: \code
 * unsigned long sum1, sum2;
 * PAR_JOBS(PJOB(thread_sum, (ints1, ints1_length, &sum1)),
 *          PJOB(thread_sum, (ints2, ints2_length, &sum2)));
 * \endcode
 * \see \li PJOB \li DECLARE_JOB
 * \hideinitializer
 */
#define PAR_JOBS(...) _XCORE_JPAR_JOBS(_XCORE_UNIQUE_LABEL(_Par), __VA_ARGS__)

/** \brief Specifies a parallelised call of a function declared with DECLARE_JOB
 *
 * Expands to a function call description which can be used as an argument to PAR_JOBS.
 *  \param _FUNCTION  a function which was declared using DECLARE_JOB
 *  \param _ARGPACK   pack of the arguments to pass to \a _FUNCTION - each must be implicitly
 *                    convertible to its respective parameter type.
 *                    E.g. <tt>(5, NULL)</tt> for <tt>void(int, int *)</tt>
 * \see \li PAR_JOBS \li DECLARE_JOB
 * \hideinitializer
 */
#define PJOB(_FUNCTION, _ARGPACK) (_FUNCTION, _ARGPACK)

/** \brief Job parameter macro to access the respective thread's synchronizer
 *
 * When expanded as a parameter to \ref PJOB as part of a \ref PAR_JOBS expansion, expands to a
 * value compatible with synchronizer_t which represents that thread's synchronizer handle. This can
 * be used to synchronise all threads in the \ref PAR_JOBS when used with
 * \ref par_jobs_synchronize .
 *
 * \note As synchronization affects all threads in the enclosing \ref PAR_JOBS, it is never useful
 *       for any \ref PJOB in a \ref PAR_JOBS to include a \ref PAR_SYNC parameter unless \b all
 *       PJOBs in that \ref PAR_JOBS do so.
 *
 * \warning Accessing the synchroniser forces allocation of a hardware synchroniser resource, even
 *          if one would not ordinarily be required. This means that a \ref PAR_JOBS expansion with
 *          only a single \ref PJOB will require a hardware synchroniser if that \ref PJOB is
 *          passed \ref PAR_SYNC. This can potentially lead to synchroniser exhaustion. Note that
 *          \ref par_jobs_synchronize has no observable effect if there is only one thread in the
 *          group.
 */
#define PAR_SYNC _XCORE_JPAR_PAR_SYNC

#ifdef __cplusplus
}
#endif

