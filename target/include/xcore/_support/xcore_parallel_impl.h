// Copyright (c) 2020-2024, XMOS Ltd, All rights reserved
#pragma once

#include <stddef.h>
#include <xcore/_support/xcore_common.h>
#include <xcore/_support/xcore_hardware_assert.h>
#include <xcore/_support/xcore_meta_macro.h>
#include <xcore/_support/xcore_macros.h>
#include <xcore/thread.h>


#ifdef __cplusplus
extern "C" {
#endif

/* Chosen to be an invalid resource handle which is easy to compare. */
#define _XCORE_NULL_SYNCHRONIZER_HANDLE 0

#ifdef __cplusplus
#define _XCORE_VOID_PTR_CAST(_PTR, _TYPE) static_cast<_TYPE>(_PTR)
#define _XCORE_CLINKAGE_BEGIN extern "C" {
#define _XCORE_CLINKAGE_END }
#else
#define _XCORE_VOID_PTR_CAST(_PTR, _TYPE) _PTR
#define _XCORE_CLINKAGE_BEGIN
#define _XCORE_CLINKAGE_END
#endif

#if _XCORE_STACK_ALIGN_REQUIREMENT_WORDS == 1
#define _XCORE_SALIGN_EXPR(_EXPR) _EXPR
#else
#define _XCORE_SALIGN_EXPR(_EXPR) "(" _EXPR " $A " _XCORE_STRINGIFY(_XCORE_STACK_ALIGN_REQUIREMENT_WORDS) ")"
#endif

#define _XCORE_PAR_PFUNC_FUNC_I(_FN, _ARG_) _FN
#define _XCORE_PAR_PFUNC_FUNC(_PACK) _XCORE_PSHIM(_XCORE_PAR_PFUNC_FUNC_I, _PACK)

#define _XCORE_PAR_FUNC_STACK_ENTRY_I(_FN, _ARGS_) \
  ".add_to_set .LPar%=sg,("\
     _XCORE_SALIGN_EXPR(_XCORE_STRINGIFY(_FN) ".nstackwords") \
     " + " _XCORE_STRINGIFY(_XCORE_STACK_ALIGN_REQUIREMENT_WORDS) ") \n\t" \
  ".add_to_set .LPar%=cg," _XCORE_STRINGIFY(_FN) ".maxcores \n\t" \
  ".add_to_set .LPar%=tg," _XCORE_STRINGIFY(_FN) ".maxtimers \n\t" \
  ".add_to_set .LPar%=eg," _XCORE_STRINGIFY(_FN) ".maxchanends \n\t"
#define _XCORE_PAR_FUNC_STACK_ENTRY(_PACK) _XCORE_PSHIM(_XCORE_PAR_FUNC_STACK_ENTRY_I, _PACK)


#define _XCORE_PAR_MASTER_STACK_SYMBOLS_I(_FN_STR) \
  ".set .LPar%=ms,"\
    _XCORE_SALIGN_EXPR("(" \
      _FN_STR ".nstackwords " \
      "$M thread_group_start.nstackwords " \
      "$M thread_group_wait_and_free.nstackwords)") "\n\t" \
  ".add_to_set .LPar%=sg,.LPar%=ms \n\t" \
  ".add_to_set .LPar%=cg," _FN_STR ".maxcores \n\t" \
  ".add_to_set .LPar%=tg," _FN_STR ".maxtimers \n\t" \
  ".add_to_set .LPar%=eg," _FN_STR ".maxchanends \n\t"

#define _XCORE_PAR_MASTER_STACK_SYMBOLS(_FN) \
  _XCORE_PAR_MASTER_STACK_SYMBOLS_I(_XCORE_STRINGIFY(_FN))

#define _XCORE_PAR_STACK_GROUP_SETUP(_PAR_NAME, _FIRST, ...) \
  ".local _fptrgroup." _XCORE_STRINGIFY(_PAR_NAME) ".nstackwords\n\t" \
  ".local _fptrgroup." _XCORE_STRINGIFY(_PAR_NAME) ".nstackwords.group \n\t" \
  ".set .LPar%=sg,0 \n\t" \
  ".local _fptrgroup." _XCORE_STRINGIFY(_PAR_NAME) ".maxcores \n\t" \
  ".local _fptrgroup." _XCORE_STRINGIFY(_PAR_NAME) ".maxcores.group \n\t" \
  ".set .LPar%=cg,0 \n\t" \
  ".local _fptrgroup." _XCORE_STRINGIFY(_PAR_NAME) ".maxtimers \n\t" \
  ".local _fptrgroup." _XCORE_STRINGIFY(_PAR_NAME) ".maxtimers.group \n\t" \
  ".set .LPar%=tg,0 \n\t" \
  ".local _fptrgroup." _XCORE_STRINGIFY(_PAR_NAME) ".maxchanends \n\t" \
  ".local _fptrgroup." _XCORE_STRINGIFY(_PAR_NAME) ".maxchanends.group \n\t" \
  ".set .LPar%=eg,0 \n\t" \
  _XCORE_APPLY_NOSEP(_XCORE_PAR_FUNC_STACK_ENTRY, __VA_ARGS__) \
  _XCORE_PAR_MASTER_STACK_SYMBOLS(_XCORE_PAR_PFUNC_FUNC(_FIRST)) \
  ".sum_reduce .LPar%=sm,.LPar%=sg,0 \n\t" \
  ".add_to_set _fptrgroup." _XCORE_STRINGIFY(_PAR_NAME) ".nstackwords.group,.LPar%=sm \n\t" \
  ".sum_reduce .LPar%=cm,.LPar%=cg,0 \n\t" \
  ".add_to_set _fptrgroup." _XCORE_STRINGIFY(_PAR_NAME) ".maxcores.group,.LPar%=cm \n\t" \
  ".sum_reduce .LPar%=tm,.LPar%=tg,0 \n\t" \
  ".add_to_set _fptrgroup." _XCORE_STRINGIFY(_PAR_NAME) ".maxtimers.group,.LPar%=tm \n\t" \
  ".sum_reduce .LPar%=em,.LPar%=eg,0 \n\t" \
  ".add_to_set _fptrgroup." _XCORE_STRINGIFY(_PAR_NAME) ".maxchanends.group,.LPar%=em \n\t"

#define _XCORE_PAR_STACK_CONFIG(_PAR_NAME, _DEST, ...) \
  asm(_XCORE_PAR_STACK_GROUP_SETUP(_PAR_NAME, __VA_ARGS__) \
      "ldc %[__dest],.LPar%=ms \n" \
      : [__dest]"=r"(_DEST) \
  )

#define _XCORE_PAR_LOAD_STACKWORDS_ASM(_DEST, _NAME) \
  asm(".set .LPar%=jsr,(" \
        _XCORE_SALIGN_EXPR(_XCORE_STRINGIFY(_NAME) ".nstackwords") \
        " + " _XCORE_STRINGIFY(_XCORE_STACK_ALIGN_REQUIREMENT_WORDS) ")\n\t" \
      "ldc %[__dest], .LPar%=jsr\n"  \
      : [__dest] "=r" (_DEST))

#define _XCORE_PAR_PRIME_II(_FUNC, _ARG) \
  thread_group_add(__xcore_par_sync, (void(*)(void *))(_FUNC), _ARG, __xcore_stackbase); \
  { \
    register unsigned __xcore_stackwords_child; \
    _XCORE_PAR_LOAD_STACKWORDS_ASM(__xcore_stackwords_child, _FUNC); \
    __xcore_stackbase = __xcore_substack_advance(__xcore_stackbase, __xcore_stackwords_child); \
  }

#define _XCORE_PAR_PRIME_I(_FUNC, _ARG) _XCORE_PAR_PRIME_II(_FUNC, (_ARG))
#define _XCORE_PAR_PRIME(_PACK) _XCORE_PAR_PRIME_I _PACK

#define _XCORE_PAR_PREPARE_LOCAL_ARG_II(_ARGNAME, _FN_, _ARGEXPR) \
  void * const _ARGNAME = (_ARGEXPR)
#define _XCORE_PAR_PREPARE_LOCAL_ARG_I(_ARGNAME, _PACK) \
  _XCORE_PAR_PREPARE_LOCAL_ARG_II(_ARGNAME, _PACK)
#define _XCORE_PAR_PREPARE_LOCAL_ARG(_PACK, _ARGNAME) \
  _XCORE_PAR_PREPARE_LOCAL_ARG_I(_ARGNAME, _XCORE_UNPACK(_PACK))

#define _XCORE_PAR_LOCAL_EXEC_II(_ARG, _FN, _ARGEXPR_) (_FN)(_ARG)
#define _XCORE_PAR_LOCAL_EXEC_I(_ARG, _PACK) _XCORE_PAR_LOCAL_EXEC_II(_ARG, _PACK)
#define _XCORE_PAR_LOCAL_EXEC(_PACK, _ARG) _XCORE_PAR_LOCAL_EXEC_I(_ARG, _XCORE_UNPACK(_PACK))

#define _XCORE_PAR_SYNCEXEC_I(_FUNC, _ARG) (_FUNC)((_ARG))
#define _XCORE_PAR_SYNCEXEC(_PACK) _XCORE_PSHIM(_XCORE_PAR_SYNCEXEC_I, _PACK)

#define _XCORE_PAR_FUNCS_PAR(_GROUP_NAME, _FIRST, ...) \
  do { \
    register void *__xcore_stackbase; \
    { \
      register unsigned __xcore_stackwords_parent; \
      _XCORE_PAR_STACK_CONFIG(_GROUP_NAME, __xcore_stackwords_parent, _FIRST, __VA_ARGS__); \
      register void *__xcore_sp; \
      asm volatile("ldaw %[__sp], sp[0]" : [__sp] "=r" (__xcore_sp)); \
      __xcore_stackbase = __xcore_substack_advance(__xcore_sp, __xcore_stackwords_parent + _XCORE_STACK_ALIGN_REQUIREMENT_WORDS); \
    } \
    const threadgroup_t __xcore_par_sync = __xcore_allocate_thread_group(); \
    _XCORE_APPLY_NOSEP(_XCORE_PAR_PRIME, __VA_ARGS__) \
    if (__builtin_expect(__xcore_dynamically_false(), 0)) \
    { \
      __attribute__(( fptrgroup(_XCORE_STRINGIFY(_GROUP_NAME)) )) void (* volatile __xcore_f)(void) = 0; \
      __xcore_f(); \
    } \
    { \
      _XCORE_PAR_PREPARE_LOCAL_ARG(_FIRST, __xcore_parg_local); \
      thread_group_start(__xcore_par_sync); \
      _XCORE_PAR_LOCAL_EXEC(_FIRST, __xcore_parg_local); \
      thread_group_wait_and_free(__xcore_par_sync); \
    } \
  } while (0)

#define _XCORE_PAR_FUNCS_SINGLE(_GN_, _FIRST) \
  do { \
    _XCORE_PAR_SYNCEXEC(_FIRST); \
  } while(0)

#define _XCORE_PAR_GET_IMLP(...) _XCORE_IF_EMPTY_LIST_ELSE(_XCORE_PAR_FUNCS_SINGLE, _XCORE_PAR_FUNCS_PAR, __VA_ARGS__)
#define _XCORE_PAR_FUNCS_I(_FLM, _GN, _FIRST, ...) _FLM(_GN, _FIRST, ## __VA_ARGS__)
#define _XCORE_PAR_FUNCS(_GN, _FIRST, ...) \
  _XCORE_PAR_FUNCS_I(_XCORE_PAR_GET_IMLP(__VA_ARGS__), \
                    _GN, _FIRST, ## __VA_ARGS__)


// This lets us identify if a macro's value is exactly `void` - it has a few layers to do this:
// _I   - We concatenate the test value with a reserved prefix - if _TEST is 'void' or 'void ' followed by something else then it'll expand
//        to a ',', otherwise it'll expand to a reserved name
//        Importantly, in the case that it's 'void followed by something else' (e.g. 'void *') our expansion will be a comma followed by that trailing bit
// _II  - Allow full expansion of the previous join
// _III - We 'call' the next layer with a dummy argument followed by the arguments we were passed, but if either of the first two were empty we drop them
// _IV  - Allow expansion to take place and actually drop the empty arugments from the previous layer
// _V   - Take the third argument and ignore everything else. This should work for all cases:
//         - 'void' - In layer _I we expand to ',' and both sides of this comma are dropped
//         - 'void X' - In layer _I we expand to ',X' and the LHS is dropped
//         - anything else - In layer _I we expand to a single (un-defined) name and nothing is dropped in subsequent layers
//        The first arugment is always a (dummy) '0', if the tested name is 'void' then the 2nd argument is the 'else' argument as we dropped
//        any preceding arugments in the earlier layers. For any other value we've inserted exactly one dummy argument to push the 'then' value
//        to the third argument position.
#define _XCORE_PICK_IF_VOID_I__void ,
#define _XCORE_DROP_EMPTY_ARG_I(...) , ## __VA_ARGS__
#define _XCORE_DROP_EMPTY_ARG(...) _XCORE_DROP_EMPTY_ARG_I(__VA_ARGS__)
#define _XCORE_PICK_IF_VOID_V(_D_, _A_, _CHOSEN, ...) _CHOSEN
#define _XCORE_PICK_IF_VOID_IV(...) _XCORE_PICK_IF_VOID_V(__VA_ARGS__)
#define _XCORE_PICK_IF_VOID_III(_A, _B, ...) _XCORE_PICK_IF_VOID_IV(0 _XCORE_DROP_EMPTY_ARG(_A) _XCORE_DROP_EMPTY_ARG(_B), __VA_ARGS__)
#define _XCORE_PICK_IF_VOID_II(...) _XCORE_PICK_IF_VOID_III(__VA_ARGS__)
#define _XCORE_PICK_IF_VOID_I(_TEST, _IF_VOID, _ELSE) _XCORE_PICK_IF_VOID_II(_XCORE_PICK_IF_VOID_I__ ## _TEST, _ELSE, _IF_VOID)
#define _XCORE_PICK_IF_VOID(_TEST, _IF_VOID, _ELSE) _XCORE_PICK_IF_VOID_I(_TEST, _IF_VOID, _ELSE)


#define _XCORE_PFIRST_I(_F, ...) _F
#define _XCORE_PFIRST(_PACK) _XCORE_PSHIM(_XCORE_PFIRST_I, _PACK, )
#define _XCORE_IF_VOID_PACK(_PACK, _IF_VOID, _ELSE) _XCORE_PICK_IF_VOID(_XCORE_PFIRST(_PACK), _IF_VOID, _ELSE)

#define _XCORE_PAR_ARG_STRUCT_NAME_I(_FNAME) __xcore_ugs_ ## _FNAME ## __xcore_args
#define _XCORE_PAR_ARG_STRUCT_NAME(_FNAME) _XCORE_PAR_ARG_STRUCT_NAME_I(_FNAME)

#define _XCORE_PAR_UNIQUE_ARG_LABEL(_) _XCORE_UNIQUE_LABEL(__xcore_parg)
#define _XCORE_PAR_PACK_ASSIGN_NAME(_ATYPE) (_ATYPE, _XCORE_PAR_UNIQUE_ARG_LABEL(_ATYPE))
#define _XCORE_PAR_ARG_PACK_PREPARE_I(...) _XCORE_APPLY(_XCORE_PAR_PACK_ASSIGN_NAME, ## __VA_ARGS__)
#define _XCORE_PAR_ARG_PACK_PREPARE(_PACK) (_XCORE_PAR_ARG_PACK_PREPARE_I _PACK)

#define _XCORE_PAR_ARG_PACK_STRUCT_ENTRY_I(_TYPE, _UNAME) _TYPE _XCORE_PICK_IF_VOID(_TYPE,,_UNAME)
#define _XCORE_PAR_ARG_PACK_STRUCT_ENTRY(_PACK) _XCORE_PAR_ARG_PACK_STRUCT_ENTRY_I _PACK
#define _XCORE_PAR_ARG_PACK_STRUCT_ENTRIES(...) _XCORE_APPLY_I(_XCORE_PAR_ARG_PACK_STRUCT_ENTRY, _XCORE_SEP_SEMICOLON, __VA_ARGS__)
#define _XCORE_PAR_ARG_PACK_PARAM_LIST(...) _XCORE_APPLY(_XCORE_PAR_ARG_PACK_STRUCT_ENTRY, __VA_ARGS__)

#define _XCORE_PAR_ARG_PACK_EXTRACT_ARG_I(_TYPE, _UNAME) _XCORE_PICK_IF_VOID(_TYPE,, __xcore_pargs->_UNAME)
#define _XCORE_PAR_ARG_PACK_EXTRACT_ARG(_PACK) _XCORE_PAR_ARG_PACK_EXTRACT_ARG_I _PACK
#define _XCORE_PAR_ARG_PACK_EXTRACT_PACK(...) _XCORE_APPLY(_XCORE_PAR_ARG_PACK_EXTRACT_ARG,  __VA_ARGS__)

#define _XCORE_DECLARE_JOB_DECL(_FNAME, _STRUCT_NAME, _ARG_PACKS, _ARG_TYPES) \
  void _FNAME _ARG_TYPES _XCORE_CFUNC_EXCEPT_SPEC asm(_XCORE_STRINGIFY(_FNAME)); \
  struct _STRUCT_NAME { \
    _XCORE_IF_VOID_PACK(_ARG_TYPES, ,_XCORE_PSHIM(_XCORE_PAR_ARG_PACK_STRUCT_ENTRIES, _ARG_PACKS)); \
  }; \
  /* TODO: Factor out names! */ \
  static void __xcore_ugs_shim_ ## _FNAME  (void *__xcore_pargs_) _XCORE_NOTHROW asm(_XCORE_STRINGIFY(__xcore_ugs_shim_ ## _FNAME))

#define _XCORE_DECLARE_JOB_DEFN(_FNAME, _STRUCT_NAME, _ARG_PACKS, _ARG_TYPES) \
  __attribute__((unused)) \
  __attribute__((noinline)) \
  static void __xcore_ugs_shim_ ## _FNAME  (void *__xcore_pargs_) _XCORE_NOTHROW \
  { \
    const struct _STRUCT_NAME * const __xcore_pargs __attribute__((unused)) = \
      _XCORE_VOID_PTR_CAST(__xcore_pargs_, const struct _STRUCT_NAME *); \
    _FNAME(_XCORE_PSHIM(_XCORE_PAR_ARG_PACK_EXTRACT_PACK, _ARG_PACKS)); \
    asm volatile("" : : : "memory"); \
  }

// Intellisense doesn't like the counter-based naming so we don't show it the definition of the shim
#ifndef __INTELLISENSE__
#define _XCORE_DECLARE_JOB(_FNAME, _STRUCT_NAME, _ARG_PACKS, _ARG_TYPES) \
  _XCORE_DECLARE_JOB_DECL(_FNAME, _STRUCT_NAME, _ARG_PACKS, _ARG_TYPES); \
  _XCORE_DECLARE_JOB_DEFN(_FNAME, _STRUCT_NAME, _ARG_PACKS, _ARG_TYPES)
#else
#define _XCORE_DECLARE_JOB(_FNAME, _STRUCT_NAME, _ARG_PACKS, _ARG_TYPES) \
  _XCORE_DECLARE_JOB_DECL(_FNAME, _STRUCT_NAME, _ARG_PACKS, _ARG_TYPES)
#endif

_XCORE_INLINE void *__xcore_substack_advance(void * const __base, const unsigned __headroom_words)
{
  register void *__result;
  asm("ldaw %[__result], %[__base][-%[__words]]"
      : [__result] "=r" (__result)
      : [__base] "r" (__base)
      , [__words] "r" (__headroom_words));
  return __result;
}

_XCORE_INLINE __xcore_bool_t __xcore_dynamically_false(void)
{
  register int __df;
  asm("ldc %[__dest], 0"
      : [__dest] "=r" (__df));
  return __df;
}

/* This is intended to be used to implement PAR_SYNC - it can be used to allocate a synchroniser
 * only at the point that one is needed.
 * In a multi-way PAR:
 *  * The parent thread can use this to allocate its synchronizer _or_ pass a pointer to a
 *    pre-allocated one;
 *  * Children can pass a NULL pointer to be given a NULL synchornizer
 * In a single-way PAR, this function can be passed a pointer to a NULL_SYNCHRONIZER and it will
 * initialize that synchronizer and return it. This allows a single-way PAR_JOBS to avoid
 * allocating a synchronizer if PAR_SYNC is not expanded in the parameter pack.
 */
_XCORE_INLINE resource_t __xcore_alloc_or_get_synchronizer(resource_t * const __s) _XCORE_NOTHROW
{
  if (__s != NULL)
  {
    if (*__s == _XCORE_NULL_SYNCHRONIZER_HANDLE)
    {
      const register resource_t __a = __xcore_allocate_thread_group();
      __xcore_ecallf(__a != _XCORE_NULL_SYNCHRONIZER_HANDLE);
      *__s = __a;
    }
    return *__s;
  }
  else
  {
    return _XCORE_NULL_SYNCHRONIZER_HANDLE;
  }
}

#define _XCORE_JPAR_LOAD_STACKWORDS_ASM(_DEST, _NAME) \
  asm(".set .LPar%=jsr,(" \
        _XCORE_SALIGN_EXPR(_XCORE_STRINGIFY(_NAME) ".nstackwords") \
        " + " _XCORE_STRINGIFY(_XCORE_STACK_ALIGN_REQUIREMENT_WORDS) ")\n\t" \
      "ldc %[__dest], .LPar%=jsr\n"  \
      : [__dest] "=r" (_DEST))

#define _XCORE_JPAR_DISPATCH_II(_FUNC, _ARGS, _ANAME) \
  const struct _XCORE_PAR_ARG_STRUCT_NAME(_FUNC) _ANAME = {_XCORE_UNPACK(_ARGS)}; \
  thread_group_add(__xcore_par_sync, \
                   (__xcore_ugs_shim_ ## _FUNC), \
                   (void *)&_ANAME, \
                   __xcore_stackbase); \
  { \
    register unsigned __xcore_stackwords_child; \
    _XCORE_JPAR_LOAD_STACKWORDS_ASM(__xcore_stackwords_child, __xcore_ugs_shim_ ## _FUNC); \
    __xcore_stackbase = __xcore_substack_advance(__xcore_stackbase, __xcore_stackwords_child); \
  }

#define _XCORE_JPAR_DISPATCH_I(_FUNC, _ARG, _ANAME, _SNAME) _XCORE_JPAR_DISPATCH_II(_FUNC, _ARG, _ANAME)
#define _XCORE_JPAR_DISPATCH(_PACK) _XCORE_PSHIM(_XCORE_JPAR_DISPATCH_I, _PACK, _XCORE_UNIQUE_LABEL(__xcore_pargs), _XCORE_UNIQUE_LABEL(__xcore_substack))

#define _XCORE_JPAR_PJOB_FUNC_I(_FN, _ARGS_) _FN
#define _XCORE_JPAR_PJOB_FUNC(_PACK) _XCORE_JPAR_PJOB_FUNC_I _PACK

#define _XCORE_JPAR_PREPARE_LOCAL_ARGS_II(_ANAME, _FUNC, _ARGS) \
  const struct _XCORE_PAR_ARG_STRUCT_NAME(_FUNC) _ANAME = {_XCORE_UNPACK(_ARGS)}
#define _XCORE_JPAR_PREPARE_LOCAL_ARGS_I(_ANAME, _PACK) \
  _XCORE_JPAR_PREPARE_LOCAL_ARGS_II(_ANAME, _PACK)
#define _XCORE_JPAR_PREPARE_LOCAL_ARGS(_PACK, _ANAME) \
  _XCORE_JPAR_PREPARE_LOCAL_ARGS_I(_ANAME, _XCORE_UNPACK(_PACK))

#define _XCORE_JPAR_LOCAL_EXEC_II(_FUNC, _PSTRUCT) \
  __xcore_ugs_shim_ ## _FUNC ((void *)&_PSTRUCT)
#define _XCORE_JPAR_LOCAL_EXEC_I(_FUNC, _PSTRUCT) _XCORE_JPAR_LOCAL_EXEC_II(_FUNC, _PSTRUCT)
#define _XCORE_JPAR_LOCAL_EXEC(_PACK, _PSTRUCT) _XCORE_JPAR_LOCAL_EXEC_I(_XCORE_JPAR_PJOB_FUNC(_PACK), _PSTRUCT)

#define _XCORE_JPAR_FUNC_STACK_ENTRY(_FN) \
  ".add_to_set .LPar%=sg,(" \
    _XCORE_SALIGN_EXPR(_XCORE_STRINGIFY(_FN) ".nstackwords") \
    " + " _XCORE_STRINGIFY(_XCORE_STACK_ALIGN_REQUIREMENT_WORDS) ") \n\t" \
  ".add_to_set .LPar%=cg," _XCORE_STRINGIFY(_FN) ".maxcores \n\t" \
  ".add_to_set .LPar%=tg," _XCORE_STRINGIFY(_FN) ".maxtimers \n\t" \
  ".add_to_set .LPar%=eg," _XCORE_STRINGIFY(_FN) ".maxchanends \n\t"

					
#define _XCORE_JPAR_MASTER_STACK_SYMBOLS_II(_FN_STR) \
  ".set .LPar%=ms,"\
  _XCORE_SALIGN_EXPR("(" \
    _FN_STR ".nstackwords " \
    "$M thread_group_start.nstackwords " \
    "$M thread_group_wait_and_free.nstackwords)") "\n\t" \
  ".add_to_set .LPar%=sg,.LPar%=ms \n\t" \
  ".add_to_set .LPar%=cg," _FN_STR ".maxcores \n\t" \
  ".add_to_set .LPar%=tg," _FN_STR ".maxtimers \n\t" \
  ".add_to_set .LPar%=eg," _FN_STR ".maxchanends \n\t"

#define _XCORE_JPAR_MASTER_STACK_SYMBOLS_I(_FN) \
  _XCORE_JPAR_MASTER_STACK_SYMBOLS_II(_XCORE_STRINGIFY(__xcore_ugs_shim_ ## _FN))

#define _XCORE_JPAR_MASTER_STACK_SYMBOLS(_FN) \
  _XCORE_JPAR_MASTER_STACK_SYMBOLS_I(_FN)

#define _XCORE_JPAR_FUNC_SHIM_STACK_ENTRY_II(_FN) _XCORE_JPAR_FUNC_STACK_ENTRY(__xcore_ugs_shim_ ## _FN)
#define _XCORE_JPAR_FUNC_SHIM_STACK_ENTRY_I(_FN, _ARGS_) _XCORE_JPAR_FUNC_SHIM_STACK_ENTRY_II(_FN)
#define _XCORE_JPAR_FUNC_SHIM_STACK_ENTRY(_PACK) _XCORE_JPAR_FUNC_SHIM_STACK_ENTRY_I _PACK

#define _XCORE_JPAR_STACK_GROUP_SETUP(_PAR_NAME, _FIRST, ...) \
  ".local _fptrgroup." _XCORE_STRINGIFY(_PAR_NAME) ".nstackwords \n\t" \
  ".local _fptrgroup." _XCORE_STRINGIFY(_PAR_NAME) ".nstackwords.group \n\t" \
  ".set .LPar%=sg,0 \n\t" \
  ".local _fptrgroup." _XCORE_STRINGIFY(_PAR_NAME) ".maxcores \n\t" \
  ".local _fptrgroup." _XCORE_STRINGIFY(_PAR_NAME) ".maxcores.group \n\t" \
  ".set .LPar%=cg,0 \n\t" \
  ".local _fptrgroup." _XCORE_STRINGIFY(_PAR_NAME) ".maxtimers \n\t" \
  ".local _fptrgroup." _XCORE_STRINGIFY(_PAR_NAME) ".maxtimers.group \n\t" \
  ".set .LPar%=tg,0 \n\t" \
  ".local _fptrgroup." _XCORE_STRINGIFY(_PAR_NAME) ".maxchanends \n\t" \
  ".local _fptrgroup." _XCORE_STRINGIFY(_PAR_NAME) ".maxchanends.group \n\t" \
  ".set .LPar%=eg,0 \n\t" \
  _XCORE_APPLY_NOSEP(_XCORE_JPAR_FUNC_SHIM_STACK_ENTRY, __VA_ARGS__) \
  _XCORE_JPAR_MASTER_STACK_SYMBOLS(_XCORE_JPAR_PJOB_FUNC(_FIRST)) \
  ".sum_reduce .LPar%=sm,.LPar%=sg,0 \n\t" \
  ".add_to_set _fptrgroup." _XCORE_STRINGIFY(_PAR_NAME) ".nstackwords.group,.LPar%=sm \n\t" \
  ".sum_reduce .LPar%=cm,.LPar%=cg,0 \n\t" \
  ".add_to_set _fptrgroup." _XCORE_STRINGIFY(_PAR_NAME) ".maxcores.group,.LPar%=cm \n\t" \
  ".sum_reduce .LPar%=tm,.LPar%=tg,0 \n\t" \
  ".add_to_set _fptrgroup." _XCORE_STRINGIFY(_PAR_NAME) ".maxtimers.group,.LPar%=tm \n\t" \
  ".sum_reduce .LPar%=em,.LPar%=eg,0 \n\t" \
  ".add_to_set _fptrgroup." _XCORE_STRINGIFY(_PAR_NAME) ".maxchanends.group,.LPar%=em \n\t"

#define _XCORE_JPAR_STACK_CONFIG(_PAR_NAME, _DEST, ...) \
  asm(_XCORE_JPAR_STACK_GROUP_SETUP(_PAR_NAME, __VA_ARGS__) \
      "ldc %[__dest],.LPar%=ms \n" \
      : [__dest]"=r"(_DEST) \
  )

#define _XCORE_JPAR_SYNCEXEC_II(_FUNC, _ARG_PACK) (_FUNC) _ARG_PACK
#define _XCORE_JPAR_SYNCEXEC_I(_PACK) _XCORE_JPAR_SYNCEXEC_II(_PACK)
#define _XCORE_JPAR_SYNCEXEC(_PACK) _XCORE_JPAR_SYNCEXEC_I(_XCORE_UNPACK(_PACK))

#ifdef __cplusplus
#define _XCORE_NOTHROW_SITE_START []() __attribute__((noinline)) noexcept {
#define _XCORE_NOTHROW_SITE_END }();
#else
#define _XCORE_NOTHROW_SITE_START
#define _XCORE_NOTHROW_SITE_END
#endif

#define _XCORE_JPAR_JOBS_PAR(_GROUP_NAME, _FIRST, ...) \
  do { \
    register void *__xcore_stackbase; \
    { \
      register unsigned __xcore_stackwords_parent; \
      _XCORE_JPAR_STACK_CONFIG(_GROUP_NAME, __xcore_stackwords_parent, _FIRST, __VA_ARGS__); \
      register void *__xcore_sp; \
      asm volatile("ldaw %[__sp], sp[0]" : [__sp] "=r" (__xcore_sp)); \
      __xcore_stackbase = __xcore_substack_advance(__xcore_sp, __xcore_stackwords_parent + _XCORE_STACK_ALIGN_REQUIREMENT_WORDS); \
    } \
    threadgroup_t __xcore_par_sync = __xcore_allocate_thread_group(); \
    __builtin_assume(__xcore_par_sync != _XCORE_NULL_SYNCHRONIZER_HANDLE); \
    _XCORE_UNUSED threadgroup_t * const __xcore_par_sync_local = NULL; \
    _XCORE_APPLY_NOSEP(_XCORE_JPAR_DISPATCH, __VA_ARGS__) \
    if (__builtin_expect(__xcore_dynamically_false(), 0)) \
    { \
      _XCORE_NOTHROW_SITE_START \
      __attribute__(( fptrgroup(_XCORE_STRINGIFY(_GROUP_NAME)) )) void (* volatile __xcore_f)(void) = 0; \
      __xcore_f(); \
      _XCORE_NOTHROW_SITE_END \
      __builtin_unreachable(); \
    } \
    { \
        _XCORE_UNUSED threadgroup_t * const __xcore_par_sync_local = &__xcore_par_sync; \
        _XCORE_JPAR_PREPARE_LOCAL_ARGS(_FIRST, __xcore_pargs_local); \
        thread_group_start(__xcore_par_sync); \
        _XCORE_JPAR_LOCAL_EXEC(_FIRST, __xcore_pargs_local); \
    } \
    thread_group_wait_and_free(__xcore_par_sync); \
  } while (0)

#define _XCORE_JPAR_JOBS_SINGLE(_GN_, _FIRST) \
  do { \
    threadgroup_t __xcore_par_sync = _XCORE_NULL_SYNCHRONIZER_HANDLE, \
                  * const _XCORE_UNUSED __xcore_par_sync_local = &__xcore_par_sync; \
    _XCORE_JPAR_SYNCEXEC(_FIRST); \
    if (__xcore_par_sync != _XCORE_NULL_SYNCHRONIZER_HANDLE) \
    { \
      thread_group_wait_and_free(__xcore_par_sync); \
    } \
  } while(0)

#define _XCORE_JPAR_GET_IMLP(...) _XCORE_IF_EMPTY_LIST_ELSE(_XCORE_JPAR_JOBS_SINGLE, _XCORE_JPAR_JOBS_PAR, __VA_ARGS__)
#define _XCORE_JPAR_JOBS_I(_FLM, _GN, _FIRST, ...) _FLM(_GN, _FIRST, ## __VA_ARGS__)
#define _XCORE_JPAR_JOBS(_GN, _FIRST, ...) \
  _XCORE_JPAR_JOBS_I(_XCORE_JPAR_GET_IMLP(__VA_ARGS__), \
                     _GN, _FIRST, ## __VA_ARGS__)

#define _XCORE_JPAR_PAR_SYNC (__xcore_alloc_or_get_synchronizer(__xcore_par_sync_local))

#ifdef __cplusplus
}
#endif

