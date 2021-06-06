#ifndef STDIO_H
#define STDIO_H

#include "stdarg.h"
#include "size_t.h"

#define NULL 0

extern "C" int getchar(void);
extern "C" char* gets (char* str);

extern "C" int putchar (int c);
extern "C" int puts (const char* s);

extern "C" int printf (const char* format, ...);
extern "C" int sprintf (char* s, const char* format, ...);
extern "C" int sprintfn (char* s, size_t n, const char* format, ...);

typedef int FILE;

extern "C" FILE* fopen (const char* filename, const char* mode);
extern "C" long unsigned int fwrite (const void* data, long unsigned int data_size, long unsigned int number_data, void* fptr);
extern "C" long unsigned int fread (const void* data, long unsigned int data_size, long unsigned int number_data, void* fptr);
extern "C" void fclose (FILE* fptr);

#endif
