/*
 * @FileName XmosSimulatorInterface.h
 * @Date 20/04/2009
 *
 * @Version 1.0
 * @Description Simulation interface header
 *
 * Copyright XMOS Ltd 2009
 */

#ifndef _Xsi_h_
#define _Xsi_h_

#if defined(WIN32) || defined (__CYGWIN__)
  #define DLL_EXPORT __declspec(dllexport)
#elif defined(__LINUX__) || defined (__linux__) || defined (__APPLE__)
  #define DLL_EXPORT extern __attribute__((visibility("default")))
#endif

#include <stdio.h>

typedef unsigned int XsiWord32;
typedef unsigned int XsiPortData;

enum XsiStatus {
  XSI_STATUS_OK =  0,
  XSI_STATUS_DONE,
  XSI_STATUS_TIMEOUT,
  XSI_STATUS_INVALID_FILE,
  XSI_STATUS_INVALID_INSTANCE,
  XSI_STATUS_INVALID_CORE,
  XSI_STATUS_INVALID_PACKAGE,
  XSI_STATUS_INVALID_PIN,
  XSI_STATUS_INVALID_PORT,
  XSI_STATUS_MEMORY_ERROR,
  XSI_STATUS_PSWITCH_ERROR,
  XSI_STATUS_INVALID_ARGS,
  XSI_STATUS_NULL_ARG,
  XSI_STATUS_INCOMPATIBLE_VERSION,
  XSI_STATUS_INVALID_NODE,
  XSI_STATUS_MEM_READ_DONE,
  XSI_STATUS_ELF_ERROR,
};

enum XsiResetType {
  XSI_RESET_HARD,
};

enum XsiEventType {
  XSI_RESET,
  XSI_IMAGE_LOADED,
};

typedef int (*xsi_fprintf_fn_t)(FILE* fp, const char* fmt, ...);
typedef int (*xsi_fflush_fn_t)(FILE* fp);
typedef size_t (*xsi_fwrite_fn_t)(const void* buf, size_t size, size_t count, FILE* fp);
typedef long (*xsi_write_fn_t)(int fd, const void* buf, size_t count);

typedef struct {
  FILE*            vcdFile;
  FILE*            gtkFile;
  xsi_fprintf_fn_t fprintfFn;
  xsi_fflush_fn_t  fflushFn;
  xsi_fwrite_fn_t  fwriteFn;
  xsi_write_fn_t   writeFn;
} XsiTraceInfo;

#endif /* _Xsi_h_ */
