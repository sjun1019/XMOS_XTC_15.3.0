// Copyright (c) 2019-2024, XMOS Ltd, All rights reserved
#pragma once

/** \file
 *  \brief Hardware-assisted threading support
 */

#include <stddef.h>
#include <xcore/_support/xcore_macros.h>
#include <xcore/_support/xcore_thread_impl.h>
#include <xcore/_support/xcore_resource_impl.h>

#ifdef __cplusplus
extern "C" {
#endif

/** \brief Handle for a single joinable thread. */
typedef resource_t xthread_t;

/** \brief Handle for a group of threads which are jointly joinable. */
typedef resource_t threadgroup_t;

/** \brief Callback type which can be executed in another thread. */
typedef void (*thread_function_t)(void *);

/** \brief Thread mode bits enumeration
 *
 *  Mode bits which may be passed to local_thread_mode_set_bits() and
 *  local_thread_mode_clear_bits() or to interpret the result of calling
 *  local_thread_mode_get_bits().
 *
 *  \note The effect of setting/clearing these bits is platform-dependent;
 *        setting/clearing a bit is not guaranteed to have any effect.
 */
typedef enum
{
  thread_mode_fast = __xcore_thread_mode_mask_fast, /**< Fast mode bit */
  thread_mode_high_priority = __xcore_thread_mode_mask_high_priority, /**< High priority mode bit */
} thread_mode_t;

/** \brief Allocates a hardware thread
 *
 *  Attempts to allocate a thread group from the pool on the current tile.
 *  \note The thread group should be freed using thread_group_free() when it is no longer
 *        required. (Or freed as a consequence of calling thread_group_wait_and_free().)
 * 
 *  \return A thread group handle, or 0 if none were available.
 */
_XCORE_INLINE threadgroup_t thread_group_alloc(void) _XCORE_NOTHROW
{
  return __xcore_allocate_thread_group();
}

/** \brief Add a new task to a thread group.
 *
 *  Adds a thead function invocation to a thread group allocated using thread_group_alloc().
 *  This configures a hardware thread to execute \a func with \a argument as its sole parameter
 *  and with its stack pointer initialised to \a stack_base.
 *  \a stack_base must be word aligned and point to the last word of a block of memory sufficient 
 *  to satisfy <tt>func</tt>'s stack requirements.
 *  That is, for a stack requirement of \c s words, <tt>[stack_base-s*word_size, stack_base]</tt> 
 *  will be  used as the thread's stack and will be clobbered.
 *  
 *  \param __group           Thread group handle as returned by thread_group_alloc().
 *  \param __func            Function to call in separate thread with signature <tt>void(void*)</tt>.
 *  \param[in] __argument    Parameter to pass to \a func.
 *  \param[in] __stack_base  Word aligned pointer to the last word of the region to use as a stack 
 *                           when calling \a func. Note that this can be calculated with stack_base().
 *
 * \note Execution of \a func will not begin until the group is started using thread_group_start().
 * \see thread_group_add()
 */
_XCORE_INLINE void thread_group_add(const threadgroup_t __group, 
                             const thread_function_t __func, 
                             void * const __argument, 
                             void * const __stack_base) _XCORE_NOTHROW
{
  const __xcore_thread_t __xthread = __xcore_create_synchronised_thread(__group);
  __xcore_set_thread_worker(__xthread, __func);
  __xcore_set_thread_stack(__xthread, __stack_base);
  __xcore_set_thread_parameter0(__xthread, __argument);
  __xcore_set_thread_terminator(__xthread, __xcore_synchronised_thread_end);
}

/** \brief Attempts to add a new task to a thread group.
 *
 *  As thread_group_add() except returns false if no threads are available. If false is returned,
 *  the thread group has not been modified and no new resources have been allocated. If the
 *  result is nonzero then the task was successfully added to the group and will be launched when the
 *  group is started.
 *
 *  \param __group           Thread group handle as returned by thread_group_alloc().
 *  \param __func            Function to call in separate thread with signature <tt>void(void*)</tt>.
 *  \param[in] __argument    Parameter to pass to \a func.
 *  \param[in] __stack_base  Word aligned pointer to the last word of the region to use as a stack
 *                           when calling \a func. Note that this can be calculated with stack_base().
 */
_XCORE_INLINE __xcore_bool_t thread_group_try_add(const threadgroup_t __group,
                                                  const thread_function_t __func,
                                                  void * const __argument,
                                                  void * const __stack_base) _XCORE_NOTHROW
{
  const __xcore_thread_t __xthread = __xcore_create_synchronised_thread(__group);
  const __xcore_bool_t __alloc_ok = __xthread != 0;
  if (__builtin_expect(__alloc_ok != 0, 1))
  {
    __xcore_set_thread_worker(__xthread, __func);
    __xcore_set_thread_stack(__xthread, __stack_base);
    __xcore_set_thread_parameter0(__xthread, __argument);
    __xcore_set_thread_terminator(__xthread, __xcore_synchronised_thread_end);
  }
  return __alloc_ok;
}

/** \brief Starts all threads in a group running.
 *
 *  Starts execution of the thread functions for each thread in the group (as added using thread_group_add()).
 *  This function will return immediately regardless of the state of the threads.
 *  \note Use thread_group_wait() or thread_group_wait_and_free() to wait for the thread group to finish.
 *
 *  \param __group  The thread group to start.
 */
_XCORE_INLINE void thread_group_start(const threadgroup_t __group) _XCORE_NOTHROW
{
  __xcore_thread_group_start(__group);
}

/** \brief Frees a thread group.
 * 
 *  Returns the thread group to the pool so that it may be allocated again. The group handle
 *  is invalid once passed to this function so must not be re-used.
 *  \attention This function must not be called on a thread group which has been started
 *             but not waited upon, even if its constituent threads have finished executing.
 *
 *  \param __group  The group to free. 
 */
_XCORE_INLINE void thread_group_free(const threadgroup_t __group) _XCORE_NOTHROW
{
  __xcore_resource_free(__group);
}

/** \brief Wait for all threads in a thread group to finish.
 *
 *  The group must have been started using thread_group_start().
 *  Calls to this function will block until all threads in the group have finished executing.
 *  Upon return of this function, the \a group remains valid but will no longer have any threads
 *  associated with it. The group may be re-used but threads must be re-added to it.
 *
 *  \note Since the group remains valid, it should be freed with thread_group_free() if no longer required.
 *
 *  \param __group  The group to wait for completion.
 */
_XCORE_INLINE void thread_group_wait(const threadgroup_t __group) _XCORE_NOTHROW
{
  __xcore_thread_group_join(__group);
}

/** \brief Waits for a thread group to finish then frees it.
 *  
 *  Helper to both wait for and free a thread group. The group must have been started using
 *  thread_group_start() and the group will be invalid once this function returns.
 *
 * \b Calls \li thread_group_wait() \li thread_group_free()
 */
_XCORE_INLINE void thread_group_wait_and_free(const threadgroup_t __group) _XCORE_NOTHROW
{
  thread_group_wait(__group);
  thread_group_free(__group);
}

/** \brief Runs a function in another thread and returns a waitable handle.
 *
 *  Starts executing \a func in a separate hardware thread with \a argument as its sole parameter
 *  and with its stack pointer initialised to \a stack_base.
 *  \a stack_base must be word aligned and point to the last word of a block of memory sufficient
 *  to satisfy <tt>func</tt>'s stack requirements.
 *  That is, for a stack requirement of \c s words, <tt>[stack_base-s*word_size, stack_base]</tt> 
 *  will be  used as the thread's stack and will be clobbered.
 *
 *  \note The thread will begin execution immediately and this function will return.
 *        The thread will not be returned to the pool upon completion - it is necessary to call 
 *        xthread_wait_and_free() to free the thread so that it may be reused.
 *
 *  \param __func            Function to call in separate thread with signature <tt>void(void*)</tt>.
 *  \param[in] __argument    Parameter to pass to \a func.
 *  \param[in] __stack_base  Word aligned pointer to the last word of the region to use as a stack 
 *                           when calling \a func. Note that this can be calculated with stack_base().
 *  \return A waitable handle for the hardware thread, or zero if the thread resource could not be
 *          allocated.
 */
_XCORE_INLINE xthread_t xthread_alloc_and_start(const thread_function_t __func, 
                                         void * const __argument,
                                         void * const __stack_base) _XCORE_NOTHROW
{
  const __xcore_thread_t __group = thread_group_alloc();
  const __xcore_bool_t __galloc_ok = __group != 0;
  if (__builtin_expect(__galloc_ok != 0, 1))
  {
    const __xcore_bool_t __talloc_ok = thread_group_try_add(__group, __func, __argument, __stack_base);
    if (__builtin_expect(!__talloc_ok, 0))
    {
      thread_group_free(__group);
      return 0;
    }

    thread_group_start(__group);
  }
  return __group;
}

/** \brief Wait for a thread to finish and then free it.
 * 
 *  \a thread must have been allocated and started using xthread_alloc_and_start().
 *  Calls to this function will block until the function called by \a thread has returned
 *  and upon completion it will free the associated hardware thread. \a thread must not be re-used
 *  after it has been freed.
 *
 *  \param __xthread  The thread to wait on, as returned by xthread_alloc_and_start().
 */
_XCORE_INLINE void xthread_wait_and_free(const xthread_t __xthread) _XCORE_NOTHROW
{
  thread_group_wait_and_free(__xthread);
}


/** \brief Runs a function in another hardware thread.
 *
 *  Starts executing \a func in a separate hardware thread with \a argument as its sole parameter
 *  and with its stack pointer initialised to \a stack_base.
 *  \a stack_base must be word aligned and point to the last word of a block of memory sufficient
 *  to satisfy <tt>func</tt>'s stack requirements.
 *  That is, for a stack requirement of \c s words, <tt>[stack_base-s*word_size, stack_base]</tt> 
 *  will be  used as the thread's stack and will be clobbered.
 *
 *  \note The associated hardware thread will be freed once \a func returns, but it is not trivially
 *        possible to determine wether or not this has happened from the calling thread.
 *        If it necessary to wait for the completion of \a func then xthread_alloc_and_start() and
 *        xthread_wait_and_free() should be used instead.
 *
 *  \param __func            Function to call in separate thread with signature <tt>void(void*)</tt>.
 *  \param[in] __argument    Parameter to pass to \a func.
 *  \param[in] __stack_base  Word aligned pointer to the last word of the region to use as a stack 
 *                           when calling \a func. Note that this can be calculated with stack_base().
 */
_XCORE_INLINE void run_async(const thread_function_t __func,
                      void * const __argument,
                      void * const __stack_base) _XCORE_NOTHROW
{
  const __xcore_thread_t __xthread = __xcore_allocate_unsynchronised_thread();
  __xcore_set_thread_worker(__xthread, __func);
  __xcore_set_thread_stack(__xthread, __stack_base);
  __xcore_set_thread_parameter0(__xthread, __argument);
  __xcore_set_thread_terminator(__xthread, __xcore_unsynchronised_thread_end);
  __xcore_unsynronised_thread_start(__xthread);
}


/** \brief Returns a stack pointer suitable for use as a \c stack_base argument 
 *         given a base address and a size.
 * 
 *   Given a base pointer (e.g. as returned by malloc or found by taking the address of an object)
 *   and the size of the intended stack in words, returns a stack base pointer to the
 *   last word of the stack - which is suitable for passing to the \c stack_base arguments of
 *   xthread_alloc_and_start(), run_async() and thread_group_add(). The given base address
 *   must be suitably aligned as the resulting stack pointer is required to be aligned also.
 *   The alignment requirement is a multiple of the word size and is target dependent.
 *   The resulting pointer will be a valid stack pointer for a stack \p words words in size.
 *   If used as a stack pointer for a function with a stack requirement no greater than \p words
 *   words then the memory region used as a stack by that function will not be beyond
 *   <tt>[mem_base, (char *)mem_base + words*WORD_SIZE)</tt> in either direction.
 *
 *   \param[in] __mem_base  The base (lowest) address of the object/region to use as a stack.
 *                          Must be word aligned.
 *   \param __words         Size of the stack the returned pointed will return to in words
 *   \return                The stack pointer.
 */
_XCORE_INLINE void *stack_base(void * const __mem_base, size_t const __words) _XCORE_NOTHROW
{
  return (void *)((char *)__mem_base + (sizeof(int)*__words) - _XCORE_STACK_ALIGN_REQUIREMENT);
}

/** \brief Set mode bits for the current thread
 *
 * Sets platform-dependent mode bits for the calling thread given a mask of bits to set.
 * The mode bits passed must be a value from thread_mode_t or the bitwise disjunuction of
 * two or more such values.
 * The actual effect of setting mode bits varies by target; no bit is guaranteed to have any
 * particular effect or any effect whatsoever.
 * This function can only set bits - using a mode value with a bit unset will not clear that bit
 * in the local thread mode. Instead, local_thread_mode_clear_bits() must be used.
 *
 * \param __mode  Mask of bits to set
 */
_XCORE_INLINE void local_thread_mode_set_bits(const thread_mode_t __mode) _XCORE_NOTHROW
{
  __xcore_set_local_thread_mode_bits(__mode);
}

/** \brief Get the current thread's mode bits
 *
 * Gets the platform-dependent mode bits for the current thread. The value returned will
 * be the bitwise disjunction of zero or more values from thread_mode_t, and those members
 * may be used as masks to interpret the result of this function.
 *
 * \return The calling thread's mode bits
 */
_XCORE_INLINE thread_mode_t local_thread_mode_get_bits(void) _XCORE_NOTHROW
{
  return (thread_mode_t)__xcore_get_local_thread_mode_bits();
}

/** \brief Clear bits in the current thread's mode
 *
 * Given a mask, clear any bits which are set in that mask in the current thread's mode.
 * The mask must be a value from thread_mode_t or the bitwise disjunction of two or more such
 * values. This can be used to unset bits which were set using local_thread_mode_set_bits().
 *
 * \param __mode  The mask of bits to clear
 */
_XCORE_INLINE void local_thread_mode_clear_bits(const thread_mode_t __mode) _XCORE_NOTHROW
{
  __xcore_clear_local_thread_mode_bits(__mode);
}

#ifdef __cplusplus
}
#endif

