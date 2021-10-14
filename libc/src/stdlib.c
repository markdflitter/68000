#include "../include/stdlib.h"
#include "../include/ctype.h"
#include "../include/stdio.h"

extern char __end;

// 128K is yer lot
#define MAX_HEAP_SIZE 0x20000

namespace
{

char* initialiseHeap ()
{
	char* freespace = &__end;
	*((unsigned int*) freespace) = MAX_HEAP_SIZE;
	*((char*) (freespace + MAX_HEAP_SIZE - sizeof (char*))) = 0;

	return freespace;
}

unsigned int zero ()
{
	return 0;
}

template <class T>
T max (const T& t1, const T& t2) { return t1 > t2 ? t1 : t2; }

size_t roundUp (size_t input)
{
	for (size_t level = 1; level < MAX_HEAP_SIZE; level *= 2)
		if (input <= level) return level;
	return input;
}

}

static char* base_of_heap = initialiseHeap ();
static char* freeptr = base_of_heap;
static char* heap_limit = base_of_heap + MAX_HEAP_SIZE;

static unsigned int malloc_count = zero ();
static unsigned int free_count = zero ();

void* malloc (size_t requestedSize)
{
//	printf ("malloc %d\n\r", requestedSize);

	//add 4 bytes for length, min size 8, round up to next power of two
	size_t allocSize = roundUp (max (requestedSize + 4, 8UL));
//	printf ("allocSize %d\n\r", allocSize);

	char* ptr = freeptr;
	char* prev = 0;
	char* bestMatchPrev = 0;
	char* bestMatch = 0;
	while (ptr != 0)
	{
		unsigned int length = *((unsigned int*) ptr);
		if (length >= allocSize)
		{
			//printf ("found match\n\r");
			if ((bestMatch == 0) || (length < *((unsigned int*) bestMatch)))
			{
				//printf ("found best match\n\r");
				bestMatchPrev = prev;
				bestMatch = ptr;
			}
		}
		prev = ptr;
		ptr = (char*) *(ptr + length - sizeof (char*));	
	}

	char* alloc = 0;

	if (bestMatch) 
	{
//		printf ("processing best match\n\r");
		alloc = bestMatch;
		unsigned int length = *((unsigned int*) bestMatch);
//		printf ("found block of size %d at 0x%x\n\r", length, bestMatch);
		if (length > allocSize)
		{
//			printf ("splitting block\n\r");

			if (bestMatchPrev)
			{
//				printf ("mid match\n\r");
				*((char**) (bestMatchPrev + *((unsigned int*) bestMatchPrev) - sizeof (char*))) = 
					((char*) (bestMatch + allocSize));
				
				*((unsigned int*) (bestMatch + allocSize)) = length - allocSize;
			}
			else
			{
//				printf ("front match\n\r");
				freeptr = (char*) (bestMatch + allocSize);
				*((unsigned int*) freeptr) = length - allocSize;
			}
		}
		else
		{
//			printf ("block is just fine\n\r");			
			
			if (bestMatchPrev)
			{
				*(bestMatchPrev + *((unsigned int*) bestMatchPrev) - sizeof (char*)) = 
					*(bestMatch + *((unsigned int*) bestMatch) - sizeof (char*)); 
			}
			else
			{
				freeptr = (char*) 
					*(bestMatch + *((unsigned int*) bestMatch) - sizeof (char*)); 
			}
		}
	
		malloc_count++;
//		printf ("[%d] malloc %d -> %d 0x%x\n\r", malloc_count, requestedSize, allocSize, alloc);

		*((unsigned int*) alloc) = allocSize;
		alloc = alloc + sizeof (unsigned int);
	}
	else
	{
		printf (">>> heap full - aborting\n\r");
		abort ();
	}

	return (void*) alloc;
}

void free (void* ptr)
{
	char* dealloc = (char*) ptr;	
	dealloc = dealloc - sizeof (unsigned int);
	
	free_count++;
//	printf ("[%d] free 0x%x, size %d\n\r", free_count, dealloc, *((unsigned int*) dealloc));
}


void heap_diag (bool detail)
{
	unsigned int totalFree = 0;

//	printf ("free 0x%x\n\r", freeptr);
//	printf ("size 0x%x\n\r", *((unsigned int*) freeptr));

	char* ptr = freeptr;
	while (ptr != 0)
	{
		unsigned int length = *((unsigned int*) ptr);
		totalFree += length;

//		printf ("free block 0x%x -> 0x%x, length %d byte(s)\n\r", ptr, ptr + length, length);

		ptr = (char*) *(ptr + length - sizeof (char*));
	}

	unsigned int totalUsed = MAX_HEAP_SIZE - totalFree;

	printf ("HEAP: base 0x%x, limit 0x%x - used %d / %d (%d%%) [%d malloc'd - %d freed = %d allocated]\n\r", base_of_heap, heap_limit, totalUsed, MAX_HEAP_SIZE, 100 * totalUsed / MAX_HEAP_SIZE, malloc_count, free_count, malloc_count - free_count);
}

void abort (void)
{
 	asm ("movel #4, %a0");
 	asm ("jmp (%a0)");

	while (1)
		;
}

int atoi (const char* str)
{
	bool negate = false;

	const char* c = str;
	while (*c != '\0' && !isdigit (*c))
		if (*c++ == '-') negate = !negate;

	int result = 0;
	while (*c != '\0')
	{
		if (isdigit (*c))
		{
			result *= 10;
			result += (*c - '0');
		}
		c++;
	}

	return negate ? -result : result;
}


long atol (const char* str)
{
	bool negate = false;

	const char* c = str;
	while (*c != '\0' && !isdigit (*c))
		if (*c++ == '-') negate = !negate;

	long result = 0;
	while (*c != '\0')
	{
		if (isdigit (*c))
		{
			result *= 10;
			result += (*c - '0');
		}
		c++;
	}

	return negate ? -result : result;
}



