#ifndef STRING_H
#define STRING_H

#include <size_t.h>

extern "C" void* memset (void* ptr, int value, size_t num);
extern "C" void* memcpy (void* destination, const void* source, size_t num);

extern "C" size_t strlen (const char* s);
extern "C" int strcmp (const char* s1, const char* s2);
extern "C" int strncmp (const char* s1, const char* s2, size_t n);

#endif
