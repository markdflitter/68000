#include <stdlib.h>

void operator delete (void* p, size_t l)
{
	free (p);
}

void operator delete[] (void* p, size_t l)
{
	free (p);
}

void operator delete (void* p)
{
	free (p);
}

void operator delete[] (void* p)
{
	free (p);
}
