#ifndef STDLIB_H
#define STDLIN_H

#include "size_t.h"

extern "C" void* malloc (size_t size);
extern "C" void free (void* ptr);

extern "C" void heap_diag ();

extern "C" void abort (void);

extern "C" long atol (const char* str);
extern "C" int atoi (const char* str);

#endif
