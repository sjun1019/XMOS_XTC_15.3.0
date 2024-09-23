// Copyright (c) 2016-2020, XMOS Ltd, All rights reserved
#pragma once

/** \file
 *  \brief Macros for efficiently handling events from multiple resources
 *
 *  Provides macros which expand to code for efficiently waiting for events on an arbitrary group
 *  of resources (analogous to a socket \c select).
 *  The general form of user code is:
 *
 *  \code
      SELECT_MACRO(CASE_MACRO([...,] CASE_LABEL) [, ...])
      // "select block":
      {
      CASE_LABEL:
        [...;]
        CASE_TERMINATOR;
      [...;]
      } // End of select block
    \endcode
 *
 *  Where:
 *
 *  \c SELECT_MACRO defines the how the checking of events is implemented and is one of:
 *    \li SELECT_RES() - \copybrief SELECT_RES
 *    \li SELECT_RES_ORDERED() - \copybrief SELECT_RES_ORDERED
 *
 *  \c CASE_MACRO determines the condition under which a case label will be jumped to and is one of:
 *    \li CASE_THEN() - \copybrief CASE_THEN
 *    \li CASE_GUARD_THEN() - \copybrief CASE_GUARD_THEN
 *    \li CASE_NGUARD_THEN() - \copybrief CASE_NGUARD_THEN
 *    \li DEFAULT_THEN() - \copybrief DEFAULT_THEN
 *    \li DEFAULT_GUARD_THEN() - \copybrief DEFAULT_GUARD_THEN
 *    \li DEFAULT_NGUARD_THEN() \copybrief DEFAULT_NGUARD_THEN
 *
 *  \c CASE_LABEL is a label inside the 'select block'
 *
 *  The \b select \b block is compound statement consisting of sequences of statements prefixed with
 *  a \c CASE_LABEL and followed by a \c CASE_TERMINATOR
 *
 *  \c CASE_TERMINATOR determines whether control exits the select block or handles further events
 *  and is one of:
 *    \li \c break - Exits the select block
 *    \li \c continue - Waits for/handles another event or default
 *    \li \ref SELECT_CONTINUE_RESET - \copybrief SELECT_CONTINUE_RESET
 *    \li \ref SELECT_CONTINUE_NO_RESET - \copybrief SELECT_CONTINUE_NO_RESET
 *
 *  The default case is a case with no associated resource which is passed using one of the
 *  \c DEFAULT_* macros. There may only be one default case; if more than one expansion of
 *  a \c DEFAULT_* macro is passed to a \c SELECT_MACRO then it is unspecified which one is
 *  effective, even if a condition means only one can ever be enabled.
 *  If the effective default case is expanded from DEFAULT_GUARD_THEN() and its condition is false
 *  \b or it is expanded from DEFAULT_NGUARD_THEN() and its condition is true, then the effect
 *  is as if there were no default case.
 *
 *  When the code expanded from a \c SELECT_MACRO is executed, qualifying resources are checked
 *  for events. Qualifying resources are those which were passed to the immediately enclosing 
 *  select block's SELECT_MACRO:
 *    \li By expanding CASE_THEN(); or
 *    \li By expanding CASE_GUARD_THEN() and where the condition evaluates true; or
 *    \li By expanding CASE_NGUARD_THEN() and where the conditional evaluates false.
 *
 *  If there is an event waiting on a qualifying resource then control is transferred to the
 *  associated label (as passed to its \c CASE_MACRO). If no such event is available then:
 *    \li If there is a default case (and it is not disabled by a condition) then control is 
 *        transferred to the associated label;
 *    \li Otherwise execution pauses until an event is available at which point control
 *        is transferred to its associated label.
 *
 *
 *  Control continues as usual within a select block until a \c CASE_TERMINATOR is executed.
 *  At that point the behaviour depends on the terminator:
 *    \li If control reaches a \c break then control will exit the immediately enclosing
 *        select block.
 *    \li If control reaches the expansion of \c SELECT_CONTINUE_NO_RESET then the effect is as
 *        if control were transferred back to the beginning of the expansion of the \c SELECT_MACRO,
 *        except that some setup is skipped. If an inner (i.e. nested) \c SELECT_MACRO expansion
 *        has been executed (even as a result of calling a function), since control was transferred
 *        into the immediately enclosing select block by the immediately preceding \c SELECT_MACRO
 *        expansion, then the use of \c SELECT_CONTINUE_NO_RESET to terminate the outer select
 *        construct has undefined behaviour.
 *    \li If control reaches the expansion of \ref SELECT_CONTINUE_RESET then the effect is
 *        as if control were transferred to the beginning of the expansion of the \c SELECT_MACRO
 *        for the immediately enclosing select block. This effectively resets the select
 *        construct and makes ineffective any setup performed by any nested \c SELECT_MACRO
 *        which would affect the execution of the immediately enclosing one.
 *    \li If control reaches a \c continue then the effect is as if control had reached the
 *        expansion of SELECT_CONTINUE_RESET if a SELECT_MACRO expansion has been executed
 *        since since control was transferred into the immediately enclosing select block by the
 *        immediately preceding \c SELECT_MACRO. Otherwise the effect is as if the expansion of
 *        \c SELECT_CONTINUE_RESET were executed.
 *        \note This is intended to provide a safe means of handling further events if arbitrary
 *              code has been executed which may have used a select construct. Its use is likely
 *              to incur a runtime performance penalty compared to using the most appropriate of
 *              \c SELECT_CONTINUE_RESET and \c SELECT_CONTINUE_NO_RESET.
 *    \li If control reaches the end of a select block without reaching a CASE_TERMINATOR
 *        then the behaviour will be as if a \c CASE_TERMINATOR had been reached but it is
 *        unspecified which one. (Thus this may result in undefined behaviour if a nested
 *        select construct has been executed.)
 * 
 *  \note Normal language rules apply - \c break and \c continue affect the relevant enclosing
 *        construct - so if they are nested within e.g. a loop then they will not terminate
 *        the select block. This also applies to other case terminators which must only appear
 *        within a select block. Thus it is impossible to exit a select block from a function.
 *        
 *
 *  The behaviour is undefined if control is transferred into a select block by any other
 *  means than by:
 *    \li Executing the immediately preceding \c SELECT_MACRO expansion;
 *    \li Returning from a function call;
 *    \li Executing a \c CASE_TERMINATOR.
 *  Additionally, transferring control out of a select block by any means other than calling a
 *  function or executing \c break as a \c CASE_TERMINATOR has undefined behaviour.
 *
 *  If the expansion of a \c SELECT_MACRO or a \c CASE_TERMINATOR is executed and:
 *    \li There are no qualifying resources, and
 *    \li There is no default case or the effective default case is disabled by a condition,
 *
 *  then the behaviour is undefined. \c CASE_MACRO (including default case) conditions are evaluated on
 *  initial entry to the SELECT_MACRO expansion and after each execution of a \c CASE_TERMINATOR
 *  other than \c break. If evaluation of a case condition has side effects then the behaviour is
 *  of the select construct is undefined.
 *
 *  If a \c CASE_LABEL is placed within a select block such that it would be illegal to use \c goto
 *  to jump to it from the point at which \c SELECT_MACRO is expanded, then the behaviour is undefined.
 *
 *  Each resource passed (though a \c CASE_MACRO) to a \c SELECT_MACRO must be unique within that
 *  \c SELECT_MACRO expansion, otherwise the behaviour of the select construct is undefined.
 *  That is, the same resource must not be used for two different cases. This applies even if 
 *  cases are guarded by mutually exclusive conditions.
 */

#if defined(__XMOS_LEGACY_C_CPP_COMPILER__) || defined(NO_XCLANG_ADDITIONS)
#include <xcore/_support/xcore_select_impl_xcc.h>
#else
#include <xcore/_support/xcore_select_impl_xclang.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** \brief Repeatedly wait for an event on an arbitrary set of resources.
 *
 *  Selects on one or more resources with an optional default. If more than one event is available then it is
 *  upspecified which one will be taken.
 *
 *  \param ...  case specifications as expanded from the \c CASE_* or \c DEFAULT_* macros
 *
 *  Example:
 *  \code
      int button_up = 1;
      int button_event_count = 0;

      SELECT_RES(
        CASE_GUARD_THEN(button_chanend, button_event_count < 21, event_button_chanend),
        CASE_THEN(led_chanend, event_led_chanend),
        DEFAULT_NGUARD_THEN(button_up, default_label))
      {
      default_label:
        puts("Button is still down!");
        SELECT_RESET;
      event_button_chanend:
        {
          uint32_t tmp = chan_in_word(button_chanend);
          chan_out_word(led_chanend, tmp);
          button_up = tmp;
        }
        button_event_count += 1;
        continue;
      event_led_chanend:
        {
          uint32_t tmp = chan_in_word(led_chanend);
          chan_out_word(button_chanend, tmp);
        }
        continue;
      }
    \endcode
 *  \hideinitializer
 */
#define SELECT_RES(...) _XCORE_SELECT_RES(__VA_ARGS__)

/** \brief Repeatedly wait for an event on an arbitrary set of events giving priority to resources based on order.
 *
 *  Behaviour is like SELECT_RES() except that events are checked in the order their respective
 *  \c CASE_* expansions are passed. This can be used to ensure that high priority events
 *  are serviced before lower priority ones.
 *
 *  If there is an event on the highest priority qualifying resource upon execution of the expansion
 *  of SELECT_RES_ORDERED(), or a select terminator within its immediately following select block, then
 *  if is guaranteed that event will be taken.
 *  For any other priority in this case (if it is the highest priority event) then it is guarateed that
 *  it will be taken, but only if no higher priority event becomes available (in which case the event
 *  taken is unspecified).
 *  If no event is available then the default case will be taken if there is a default case and it is
 *  enabled. If there is no default case or the default case is disabled by a condition then execution
 *  will pause and wait for an event on a qualifying resource - in this case prioties no longer apply
 *  and the first event which becomes available will be taken.
 *
 *  \param ...  case specifications as expanded from the \c CASE_* or \c DEFAULT_* macros
 *
 *  \note There is a runtime overhead associated with checking events in order so it is suggested
 *        that SELECT_RES() be used whenever possible.
 *
 *  \hideinitializer
 */
#define SELECT_RES_ORDERED(...) _XCORE_SELECT_RES_ORDERED(__VA_ARGS__)

/** \brief Restores the configuration of the immediately enclosing select block and continues.
 *
 *  This may be used instead of \c continue as a select terminator if a different (to the immediately
 *  enclosing one) select construct has been executed. The effect is that the resource setup which happens
 *  at the start of the select construct is guaranteed to re-run so that the correct events will be enabled.
 *  Using this instead of \c continue (when required) is likely to result in a slight performance boost as 
 *  it eliminates the need to check global state.
 *
 *  \hideinitializer
 */
#define SELECT_CONTINUE_RESET _XCORE_SELECT_RESET_I

/** \brief Continues to handle another event in the immediately inclosing select block without applying global resource setup.
 *
 *  This may be used in stead of \c continue as a select terminator if different select block has not been executed during handling
 *  of the previous event (or default). In the event that another select construct has executed this will have undefined behaviour.
 *
 *  Using this instead of \c continue (when safe to do so) is likely to result in a slight performance improvement.
 *
 * \hideinitializer
 */
#define SELECT_CONTINUE_NO_RESET _XCORE_CONTINUE_NO_RESET_I

/** \brief Unconditionally wait for an event on a given resource.
 *
 *  \param _RES    The resource to wait for an event on.
 *  \param _LABEL  The label (within the following select block) to jump to when an event
 *                 occurs on \a res. 
 *
 *  \hideinitializer
 */
#define CASE_THEN(_RES, _LABEL) _XCORE_CASE_RES(_RES, _LABEL, _XCORE_GUARD_NONE)

/** \brief Wait for an event on a given resource if a condition evaluates true.
 *
 *  \param _RES         The resource to wait for an event on.
 *  \param _GUARD_EXPR  Expression to evaluate to determine if this case should be enabled.
 *                      Must not have side effects.
 *  \param _LABEL       The label (within the following select block) to jump to when an event
 *                      occurs on \a res. 
 *
 *  \hideinitializer
 */
#define CASE_GUARD_THEN(_RES, _GUARD_EXPR, _LABEL) _XCORE_CASE_RES(_RES, _LABEL, _XCORE_GUARD_TRUE(_GUARD_EXPR))

/** \brief Wait for an event on a given resource is a condition evaluates false.
 *
 *  \param _RES         The resource to wait for an event on.
 *  \param _GUARD_EXPR  Expression to evaluate to determine if this case should be disabled.
 *                      Must not have side effects.
 *  \param _LABEL       The label (within the following select block) to jump to when an event
 *                      occurs on \a res. 
 *
 *  \hideinitializer
 */
#define CASE_NGUARD_THEN(_RES, _GUARD_EXPR, _LABEL) _XCORE_CASE_RES(_RES, _LABEL, _XCORE_GUARD_FALSE(_GUARD_EXPR))

/** \brief Defines a label to jump to if no enabled resource events are ready.
 *
 *  \param _LABEL  The label (within the following select block) to jump to if there are
 *                 no events to take.                 
 *
 *  \hideinitializer
 */
#define DEFAULT_THEN(_LABEL) _XCORE_CASE_DEFAULT(_LABEL, _XCORE_GUARD_NONE)

/** \brief Defines a label to jump to if no enabled resource events are ready and a condition evaluates true.
 *
 *  \param _LABEL       The label (within the following select block) to jump to if there are
 *                      no events to take.
 *  \param _GUARD_EXPR  Expression to determine it the default case should be enabled.
 *
 *  \hideinitializer
 */
#define DEFAULT_GUARD_THEN(_GUARD_EXPR, _LABEL) _XCORE_CASE_DEFAULT(_LABEL, _XCORE_GUARD_TRUE(_GUARD_EXPR))

/** \brief Defines a label to jump to if no enabled resource events are ready and a condition evaluates false.
 *
 *  \param _LABEL       The label (within the following select block) to jump to if there are
 *                      no events to take.
 *  \param _GUARD_EXPR  Expression to determine it the default case should be disabled.
 * 
 *  \hideinitializer
 */
#define DEFAULT_NGUARD_THEN(_GUARD_EXPR, _LABEL) _XCORE_CASE_DEFAULT(_LABEL, _XCORE_GUARD_FALSE(_GUARD_EXPR))

#ifdef __cplusplus
}
#endif

