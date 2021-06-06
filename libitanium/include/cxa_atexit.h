#ifndef CXA_ATEXIT_H
#define CXA_ATEXIT_H

#include "dso_handle.h"

extern "C" int __cxa_atexit (void (*func) (void*), void* arg, void* dso_handle);

extern dso_handle __dso_handle;

#endif
