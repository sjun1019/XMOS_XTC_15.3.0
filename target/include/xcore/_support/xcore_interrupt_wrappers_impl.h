// Copyright (c) 2016-2024, XMOS Ltd, All rights reserved
#pragma once

// This file contains private implementation details and is not part of the API.
// The contents may vary between releases.
#include <xcore/_support/xcore_common.h>
#include <xcore/_support/xcore_macros.h>
#include <xcore/_support/xcore_meta_macro.h>
#include <xcore/_support/xcore_type_check.h>

#ifdef __cplusplus
extern "C" {
#endif


// The user may define a larger kstack for their own worse case use.
#ifndef LIBXCORE_KSTACK_WORDS
#define LIBXCORE_KSTACK_WORDS 0
#endif

#define _XCORE_INTERRUPT_PERMITTED(__root_function) \
    __xcore_interrupt_permitted_ugs_ ## __root_function

#define _XCORE_DECLARE_INTERRUPT_PERMITTED(__ret, __root_function, ...) \
    __ret _XCORE_INTERRUPT_PERMITTED(__root_function) (__VA_ARGS__);\
    __ret __root_function(__VA_ARGS__)

#define _XCORE_DEFINE_INTERRUPT_PERMITTED_I(__grp, __root_function, __interrupt_permitted, __sym_base) \
    asm( \
    ".weak _fptrgroup." __grp ".nstackwords.group \n" \
    ".max_reduce " __sym_base ".groupwords, _fptrgroup." __grp ".nstackwords.group, 0 \n" \
    /* Allocate an additional slot for the initial LR spill in the root function */ \
    ".set " __sym_base ", ((" __sym_base ".groupwords $M " _XCORE_STRINGIFY(LIBXCORE_KSTACK_WORDS) ") $A " _XCORE_STRINGIFY(_XCORE_STACK_ALIGN_REQUIREMENT_WORDS) ") + " _XCORE_STRINGIFY(_XCORE_STACK_ALIGN_REQUIREMENT_WORDS) " \n" \
    ".globl __xcore_interrupt_permitted_common \n" \
    ".globl " __interrupt_permitted " \n" \
    ".align " _XCORE_STRINGIFY(_XCORE_CODE_ALIGNMENT) " \n" \
    ".type " __interrupt_permitted ",@function \n" \
    ".cc_top " __interrupt_permitted ".function, " __interrupt_permitted " \n" \
    __interrupt_permitted ": \n" \
    "  " _XCORE_ENTSP(_XCORE_STACK_ALIGN(3)) " \n" \
    "  stw r5, sp[2] \n" \
    "  stw r4, sp[1] \n" \
    "  ldc r4, " __sym_base " \n" \
    "  ldap r11, " __root_function " \n" \
    "  add r5, r11, 0 \n"\
    "  ldap r11, __xcore_interrupt_permitted_common \n" \
    "  bau r11 \n" \
    ".cc_bottom " __interrupt_permitted ".function \n" \
    ".set " __interrupt_permitted ".nstackwords, " _XCORE_STRINGIFY(_XCORE_STACK_ALIGN(3)) " + " __sym_base " + __xcore_interrupt_permitted_common.nstackwords + " __root_function ".nstackwords  \n" \
    ".globl " __interrupt_permitted ".nstackwords \n" \
    ".set " __interrupt_permitted ".maxcores, 1 $M __xcore_interrupt_permitted_common.maxcores $M " __root_function ".maxcores \n" \
    ".globl " __interrupt_permitted ".maxcores \n" \
    ".set " __interrupt_permitted ".maxtimers, 0 $M __xcore_interrupt_permitted_common.maxtimers $M " __root_function ".maxtimers \n" \
    ".globl " __interrupt_permitted ".maxtimers \n" \
    ".set " __interrupt_permitted ".maxchanends, 0 $M __xcore_select_callback_common.maxchanends $M " __root_function ".maxchanends \n" \
    ".globl " __interrupt_permitted ".maxchanends \n" \
    ".size " __interrupt_permitted ", . - " __interrupt_permitted " \n" \
    )

#define _XCORE_CHECK_IP_PARAM_TYPE_IF_NOT_EMPTY_I(_P, _PN) \
    _XCORE_IF_EMPTY_LIST_ELSE(,_XCORE_TC_CHECK_NAMED(_P, _XCORE_TC_PRED_ARG_PROBABLY_PASSED_IN_REGISTER, __xcore_interrupt_permitted_param_##_PN##_has_invalid_type),_P)
#define _XCORE_CHECK_IP_PARAM_TYPE_IF_NOT_EMPTY(_PICKER, _PN, _PARAMS...) \
    _XCORE_CHECK_IP_PARAM_TYPE_IF_NOT_EMPTY_I(_PICKER(_PARAMS), _PN)
#define _XCORE_CHECK_IP_PARAM_LIST_LENGTH_I(_MAYBE_T) \
    _XCORE_IF_EMPTY_LIST_ELSE(,_XCORE_TC_CHECK_NAMED(_MAYBE_T,_XCORE_TC_PRED_FALSE,__xcore_interrupt_permitted_function_has_too_many_parameters);,_MAYBE_T)
#define _XCORE_CHECK_IP_PARAM_LIST_LENGTH(_PARAMS...) \
    _XCORE_CHECK_IP_PARAM_LIST_LENGTH_I(_XCORE_PICK_4_OR_EMPTY(_PARAMS))
#define _XCORE_CHECK_IP_RETURN_TYPE(_T) \
    _XCORE_TC_CHECK_NAMED(_T, _XCORE_TC_PRED_TYPE_PROBABLY_RETURNED_IN_REGISTER, __xcore_interrupt_permitted_return_type_invalid)

#define _XCORE_DEFINE_INTERRUPT_PERMITTED(__grp, __ret, __root_function, ...) \
    _XCORE_DEFINE_INTERRUPT_PERMITTED_I(_XCORE_STRINGIFY(__grp), _XCORE_STRINGIFY(__root_function), \
                                        _XCORE_STRINGIFY(_XCORE_INTERRUPT_PERMITTED(__root_function)), \
                                        _XCORE_STRINGIFY(_XCORE_UNIQUE_LABEL(__xcore_interrupt_permitted_kstackwords))); \
    __ret _XCORE_INTERRUPT_PERMITTED(__root_function) (__VA_ARGS__); \
    _XCORE_CHECK_IP_PARAM_TYPE_IF_NOT_EMPTY(_XCORE_PICK_0_OR_EMPTY, 0, __VA_ARGS__); \
    _XCORE_CHECK_IP_PARAM_TYPE_IF_NOT_EMPTY(_XCORE_PICK_1_OR_EMPTY, 1, __VA_ARGS__); \
    _XCORE_CHECK_IP_PARAM_TYPE_IF_NOT_EMPTY(_XCORE_PICK_2_OR_EMPTY, 2, __VA_ARGS__); \
    _XCORE_CHECK_IP_PARAM_TYPE_IF_NOT_EMPTY(_XCORE_PICK_3_OR_EMPTY, 3, __VA_ARGS__); \
    _XCORE_CHECK_IP_PARAM_LIST_LENGTH(__VA_ARGS__) \
    _XCORE_CHECK_IP_RETURN_TYPE(__ret) \
    __ret __root_function(__VA_ARGS__)

#define _XCORE_INTERRUPT_CALLBACK(__intrpt) \
    __xcore_interrupt_callback_ ## __intrpt

#define _XCORE_DECLARE_INTERRUPT_CALLBACK(__intrpt, __data) \
    void _XCORE_INTERRUPT_CALLBACK(__intrpt)(void);\
    void __intrpt(void* __data)

#define _XCORE_DEFINE_INTERRUPT_CALLBACK_DEF(__grp, __root_function, __intrpt) \
    asm( \
    ".globl __xcore_interrupt_callback_common \n"\
    ".weak _fptrgroup." __grp ".nstackwords.group \n" \
    ".add_to_set _fptrgroup." __grp ".nstackwords.group, " __intrpt ".nstackwords, " __intrpt " \n" \
    ".globl " __intrpt " \n" \
    ".align " _XCORE_STRINGIFY(_XCORE_CODE_ALIGNMENT) " \n" \
    ".type " __intrpt ",@function \n" \
    ".cc_top " __intrpt ".function," __intrpt " \n" \
    __intrpt ": \n" \
    "  " _XCORE_SINGLE_ISSUE " \n" /* Do we know what KEDI is set to? */ \
    "  kentsp " _XCORE_STRINGIFY(_XCORE_STACK_ALIGN(7)) " \n" \
    "  stw r11, sp[5] \n" \
    "  stw r1, sp[3] \n" \
    "  ldap r11, " __root_function " \n" \
    "  add r1, r11, 0 \n"\
    "  ldap r11, __xcore_interrupt_callback_common \n"\
    "  bau r11 \n"\
    ".cc_bottom " __intrpt ".function \n" \
    ".set " __intrpt ".nstackwords, " _XCORE_STRINGIFY(_XCORE_STACK_ALIGN(7)) " + __xcore_interrupt_callback_common.nstackwords + " __root_function ".nstackwords \n" \
    ".globl " __intrpt ".nstackwords \n" \
    ".set " __intrpt ".maxcores, 1 $M __xcore_interrupt_callback_common.maxcores $M " __root_function ".maxcores \n" \
    ".globl " __intrpt ".maxcores \n" \
    ".set " __intrpt ".maxtimers, 0 $M __xcore_interrupt_callback_common.maxtimers $M " __root_function ".maxtimers \n" \
    ".globl " __intrpt ".maxtimers \n" \
    ".set " __intrpt ".maxchanends, 0 $M __xcore_interrupt_callback_common.maxchanends $M " __root_function ".maxchanends  \n" \
    ".globl " __intrpt ".maxchanends \n" \
    ".size " __intrpt ", . - " __intrpt " \n" \
    )

#define _XCORE_DEFINE_INTERRUPT_CALLBACK(__grp, __intrpt, __data) \
    _XCORE_DEFINE_INTERRUPT_CALLBACK_DEF(_XCORE_STRINGIFY(__grp), _XCORE_STRINGIFY(__intrpt), \
                                         _XCORE_STRINGIFY(_XCORE_INTERRUPT_CALLBACK(__intrpt)) ); \
    _XCORE_DECLARE_INTERRUPT_CALLBACK(__intrpt, __data)


#ifdef __cplusplus
}
#endif

