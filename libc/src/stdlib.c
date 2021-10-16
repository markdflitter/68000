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

inline void** next_block (void* p)
{
  return (void**) ((unsigned char*) p + sizeof (unsigned int));
}

template <class T1, class T2>
static T1 min (const T1& t1, const T2& t2)
{
	return t1 < t2 ? t1 : t2;
}
	
unsigned long printBuffer (char* buffer, size_t bufferLen, unsigned long startAddress)
{
	char* p = buffer;
	
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


inline void dump_memory (char* p, unsigned int length)
{
	printBuffer (p, length, (unsigned long) p);
	char buf [255];
	sprintf (buf, "\n\r"); __putstr (buf);
}


char* initialiseHeap ()
{
	char* p = &__end;
	memset (p, 0xff, MAX_HEAP_SIZE);

	char buf [255];
	sprintf (buf, "HEAP_INIT: start 0x%x end 0x%x size 0x%x (%d) byte(s)\n\r", p, p + MAX_HEAP_SIZE, MAX_HEAP_SIZE, MAX_HEAP_SIZE); __putstr (buf);
	
	//sentinel
	const unsigned int sentinelBlockSize = MIN_ALLOC;
	block_len (p) = sentinelBlockSize;
	*next_block (p) = increment (p, sentinelBlockSize);
	
	//empty block
	block_len (*next_block (p)) = MAX_HEAP_SIZE - sentinelBlockSize;
	 *next_block (*next_block (p)) = 0;
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

static char* base_of_heap = initialiseHeap ();
static char* freeptr = base_of_heap;
static char* heap_limit = base_of_heap + MAX_HEAP_SIZE;

static unsigned int malloc_count = zero ();
static unsigned int free_count = zero ();

void* malloc (size_t requestedSize)
{
	//printf ("malloc %d\n\r", requestedSize);

	//add 4 bytes for length, min size 8, round up to next power of two
	size_t allocSize = roundUp (max (requestedSize + 4, 8UL));
	//printf ("allocSize %d\n\r", allocSize);

	char* prev = freeptr;
	char* p = (char*) *next_block (prev);
	//printf ("p 0x%x\n\r", p);

	char* bestMatchPrev = 0;
	char* bestMatch = 0;

	while (p != 0)
	{
		unsigned int length = block_len (p);
		if (length >= allocSize)
		{
			//printf ("found match\n\r");
			if ((bestMatch == 0) || (length < *((unsigned int*) bestMatch)))
			{
				//printf ("found best match\n\r");
				bestMatchPrev = prev;
				bestMatch = p;
			}
		}
		prev = p;
		p = (char*) *next_block (p);
	}

	char* alloc = 0;

	if (bestMatch) 
	{
		//printf ("processing best match\n\r");
		alloc = bestMatch;
		unsigned int length = block_len (bestMatch);
		//printf ("found block of size %d at 0x%x\n\r", length, bestMatch);
		if (length >= allocSize + MIN_ALLOC)
		{
			//printf ("splitting block\n\r");

			*next_block (bestMatchPrev) = bestMatch + allocSize;
			block_len (*next_block (bestMatchPrev)) = length - allocSize;
			*next_block (*next_block (bestMatchPrev)) = *next_block (bestMatch); 
		}
		else
		{
			//printf ("block is just fine\n\r");			
			*next_block (bestMatchPrev) = *next_block (bestMatch);
		}
	
		malloc_count++;
		//printf ("[%d] malloc %d -> %d 0x%x\n\r", malloc_count, requestedSize, allocSize, alloc);

		block_len (bestMatch) = allocSize;
		alloc += sizeof (unsigned int);
	}
	else
	{
		printf (">>> heap full - aborting\n\r");
		abort ();
	}

	memset (alloc, 0xaa, allocSize - sizeof (unsigned int));
	return (void*) alloc;
}

void free (void* f)
{
	char* p = (char*) f;
	p -= sizeof (unsigned int);
	//printf ("p 0x%x\n\r", p);

	char* sentinel = freeptr;
	//printf ("sentinel 0x%x\n\r", sentinel);

	*next_block (p) = *next_block (sentinel);
	*next_block (sentinel) = p;

	free_count++;
	//printf ("[%d] free 0x%x, size %d\n\r", free_count, p, block_len (p));
}


void heap_diag (bool detail)
{
	unsigned int totalFree = 0;

	char* p = freeptr;
	while (p != 0)
	{
		unsigned int length = block_len (p);
		totalFree += length;

		if (p == freeptr)
			printf ("sntl");
		else
			printf ("free");
		printf (" block 0x%x -> 0x%x, length %d byte(s)\n\r", p, p + length, length);
		
		dump_memory (p, 16);
		
		p = (char*) *next_block (p);
	}

	unsigned int totalUsed = MAX_HEAP_SIZE - totalFree;

	printf ("HEAP: base 0x%x, limit 0x%x - used %d / %d (%d%%) -> %d free [%d malloc'd - %d freed = %d allocated]\n\r", base_of_heap, heap_limit, totalUsed, MAX_HEAP_SIZE, 100 * totalUsed / MAX_HEAP_SIZE, totalFree, malloc_count, free_count, malloc_count - free_count);
	dump_memory (base_of_heap, 128);
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


