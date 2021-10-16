#include "../include/stdlib.h"
#include "../include/ctype.h"
#include "../include/stdio.h"
#include <bsp.h>
#include <string.h>

extern char __end;

// 128K is yer lot
#define MAX_HEAP_SIZE 0x20000
#define MIN_ALLOC 8

namespace
{

inline void* increment (void* p, unsigned int len)
{
	return (unsigned char*) p + len;
}

inline unsigned int& block_len (void* p)
{
	return *(unsigned int*) p;
}

inline void* next_block (void* p)
{
  return ((unsigned char*) p + sizeof (unsigned int));
}

template <class T1, class T2>
static T1 min (const T1& t1, const T2& t2)
{
	return t1 < t2 ? t1 : t2;
}
	
unsigned long printBuffer (unsigned char* buffer, size_t bufferLen, unsigned long startAddress)
{
	unsigned char* p = buffer;
	
	unsigned long address = startAddress;

	while (bufferLen > 0)
	{
		size_t rowLen = min (bufferLen, 16);

		char buf [255];		
		sprintf (buf, "0x%x\t : ", address); __putstr (buf);
		address += rowLen;

		for (int c = 0; c < rowLen; c++) 
		{
			if (c != 0) {sprintf (buf, " "); __putstr (buf);}
			sprintf (buf, "%x",*p++); __putstr (buf);
		}
			
		bufferLen -= rowLen;
		sprintf (buf, "\n\r"); __putstr (buf);
	}

	return address;
}


inline void dump_memory (void* p, unsigned int length)
{
	printBuffer ((unsigned char*) p, length, (unsigned long) p);
	char buf [255]; sprintf (buf, "\n\r"); __putstr (buf);
}


void* initialiseHeap ()
{
	void* p = &__end;
	memset (p, 0xff, MAX_HEAP_SIZE);

	char buf [255];
	sprintf (buf, "HEAP_INIT: start 0x%x end 0x%x size 0x%x (%d) byte(s)\n\r", p, (unsigned char*) p + MAX_HEAP_SIZE, MAX_HEAP_SIZE, MAX_HEAP_SIZE); __putstr (buf);
	
	//sentinel
	const unsigned int sentinelBlockSize = MIN_ALLOC;
	block_len (p) = sentinelBlockSize;
	*(void**) next_block (p) = increment (p, sentinelBlockSize);
	
	//empty block
	block_len (*(void**) next_block (p)) = MAX_HEAP_SIZE - sentinelBlockSize;
	 * (void**) next_block (*(void**) next_block (p)) = 0;
   	dump_memory (&__end, 32);

	return p;
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

static void* base_of_heap = initialiseHeap ();
static void* freeptr = base_of_heap;
static void* heap_limit = (unsigned char*) base_of_heap + MAX_HEAP_SIZE;

static unsigned int malloc_count = zero ();
static unsigned int free_count = zero ();

void* malloc (size_t requestedSize)
{
	return 0;
}

/*
	printf ("malloc %d\n\r", requestedSize);

	//add 4 bytes for length, min size 8, round up to next power of two
	size_t allocSize = roundUp (max (requestedSize + 4, 8UL));
	printf ("allocSize %d\n\r", allocSize);

	char* ptr = freeptr;
	char* prev = 0;
	char* bestMatchPrev = 0;
	char* bestMatch = 0;
	while (ptr != 0)
	{
		unsigned int length = *((unsigned int*) ptr);
		if (length >= allocSize)
		{
			printf ("found match\n\r");
			if ((bestMatch == 0) || (length < *((unsigned int*) bestMatch)))
			{
				printf ("found best match\n\r");
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
		printf ("processing best match\n\r");
		alloc = bestMatch;
		unsigned int length = *((unsigned int*) bestMatch);
		printf ("found block of size %d at 0x%x\n\r", length, bestMatch);
		if (length > allocSize)
		{
			printf ("splitting block\n\r");

			if (bestMatchPrev)
			{
				printf ("mid match\n\r");
				*((char**) (bestMatchPrev + *((unsigned int*) bestMatchPrev) - sizeof (char*))) = 
					((char*) (bestMatch + allocSize));
				
				*((unsigned int*) (bestMatch + allocSize)) = length - allocSize;
			}
			else
			{
				printf ("front match\n\r");
				freeptr = (char*) (bestMatch + allocSize);
				*((unsigned int*) freeptr) = length - allocSize;
			}
		}
		else
		{
			printf ("block is just fine\n\r");			
			
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
		printf ("[%d] malloc %d -> %d 0x%x\n\r", malloc_count, requestedSize, allocSize, alloc);

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
*/

void free (void* ptr)
{
	char* dealloc = (char*) ptr;	
	dealloc = dealloc - sizeof (unsigned int);
	
	free_count++;
	printf ("[%d] free 0x%x, size %d\n\r", free_count, dealloc, *((unsigned int*) dealloc));
}


void heap_diag (bool detail)
{
	unsigned int totalFree = 0;


	void* ptr = freeptr;
	while (ptr != 0)
	{
		unsigned int length = block_len (ptr);
		totalFree += length;

		printf ("free block 0x%x -> 0x%x, length %d byte(s)\n\r", ptr, (unsigned char*) ptr + length, length);

		ptr = *(void**) next_block (ptr);
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

