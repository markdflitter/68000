#include "size_t.h"

extern "C" void* malloc (size_t size);
extern "C" void free (void* ptr);

extern "C" void abort (void);
