#include "../include/stdlib.h"
#include "../include/ctype.h"
#include "../include/string.h"
#include "../include/stdio.h"
#include <bsp.h>

extern char __heap_start[];
extern char __heap_end[];

#define MIN_ALLOC 8

namespace
{

inline void* increment (void* p, unsigned int len)
{
	return (unsigned char*) p + len;
}

inline unsigned int& block_len (void* p)
{
	return *((unsigned int*) p);
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

template <class T>
T max (const T& t1, const T& t2)
{ 
	return t1 > t2 ? t1 : t2; 
}

#ifdef DEBUG
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
			
		memset (buf, ' ', 3 * (16 - rowLen) + 4);
		buf [3 * (16 - rowLen) + 4] = '\0';
		__putstr (buf);

		p -= rowLen;
		for (int c = 0; c < rowLen; c++)
		{
			if (isprint (*p))
				{sprintf (buf, "%c",*p); __putstr (buf);}
			else
				{sprintf (buf, "."); __putstr (buf);}
			p++;
		}

		bufferLen -= rowLen;
		sprintf (buf, "\n\r"); __putstr (buf);
	}

	return address;
}


inline void dump_memory (unsigned char* p, unsigned int length)
{
	printBuffer (p, length, (unsigned long) p);
	char buf [255];
	sprintf (buf, "\n\r"); __putstr (buf);
}
#endif

struct heap
{
  heap (char* _base_of_heap, unsigned int _heap_size)
    : base_of_heap (_base_of_heap), freeptr (_base_of_heap), heap_size (_heap_size), heap_limit (_base_of_heap + heap_size), malloc_count (0), free_count (0), trace (0)
	{}

  char* base_of_heap;
  char* freeptr;
  unsigned int heap_size;
  char* heap_limit;

  unsigned int malloc_count;
  unsigned int free_count;
  unsigned int trace;
};

heap initialiseHeap ()
{
	char* p = &__heap_start[0];
	char* e = &__heap_end[0];
	unsigned int heap_size = e - p;

#ifdef DEBUG
	char buf [255];
	sprintf (buf, "HEAP_INIT: start 0x%x end 0x%x size 0x%x (%d) byte(s)\n\r", p, p + heap_size, heap_size, heap_size); __putstr (buf);
#endif

	memset (p, 0xff, heap_size);

	//sentinel
	const unsigned int sentinelBlockSize = MIN_ALLOC;
	block_len (p) = sentinelBlockSize;
	*next_block (p) = increment (p, sentinelBlockSize);
	
	//empty block
	block_len (*next_block (p)) = heap_size - sentinelBlockSize;
	 *next_block (*next_block (p)) = 0;

#ifdef DEBUG
	dump_memory ((unsigned char*) &__heap_end[0], 16);
#endif

	return heap (p, heap_size);
}

static heap the_heap = initialiseHeap ();

size_t roundUp (size_t input)
{
	for (size_t level = 1; level < the_heap.heap_size; level *= 2)
		if (input <= level) return level;
	return input;
}

}

#ifdef DEBUG
void set_heap_trace (unsigned int tr)
{
  the_heap.trace = tr;
}
#endif

void* malloc (size_t requestedSize)
{
#ifdef DEBUG
	if (the_heap.trace > 0) printf ("malloc %d\n\r", requestedSize);
	if (the_heap.trace > 1) heap_diag (the_heap.trace > 2);
#endif

	//add 4 bytes for length, min size 8, round up to next power of two
	size_t allocSize = roundUp (max (requestedSize + 4, 8UL));
#ifdef DEBUG
	//printf ("allocSize %d\n\r", allocSize);
#endif

	char* prev = the_heap.freeptr;
	char* p = (char*) *next_block (prev);
#ifdef DEBUG
	//printf ("p 0x%x\n\r", p);
#endif
	char* bestMatchPrev = 0;
	char* bestMatch = 0;

	while (p != 0)
	{
		unsigned int length = block_len (p);
		if (length >= allocSize)
		{
#ifdef DEBUG
			//printf ("found match\n\r");
#endif
			if ((bestMatch == 0) || (length < *((unsigned int*) bestMatch)))
			{
#ifdef DEBUG	
				//printf ("found best match\n\r");
#endif
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
#ifdef DEBUG
		//printf ("processing best match\n\r");
#endif
		alloc = bestMatch;
		unsigned int length = block_len (bestMatch);
#ifdef DEBUG
		//printf ("found block of size %d at 0x%x\n\r", length, bestMatch);
#endif
		if (length >= allocSize + MIN_ALLOC)
		{
#ifdef DEBUG	
			//printf ("splitting block %d into %d and %d\n\r", length, allocSize, length - allocSize);
#endif
			*next_block (bestMatchPrev) = bestMatch + allocSize;
			block_len (*next_block (bestMatchPrev)) = length - allocSize;
#ifdef DEBUG
			//printf ("split len %d\n\r", block_len (*next_block (bestMatchPrev)));	
#endif
			*next_block (*next_block (bestMatchPrev)) = *next_block (bestMatch); 
		}
		else
		{
#ifdef DEBUG
			//printf ("block is just fine\n\r");
#endif
			*next_block (bestMatchPrev) = *next_block (bestMatch);
		}
	
#ifdef DEBUG
		the_heap.malloc_count++;
		if (the_heap.trace) printf ("[%d] malloc %d -> %d 0x%x\n\r", the_heap.malloc_count, requestedSize, allocSize, alloc);
		if (the_heap.trace > 1) heap_diag (the_heap.trace > 2);
#endif

		block_len (bestMatch) = allocSize;
		alloc += sizeof (unsigned int);
	}
	else
	{
		puts (">>> heap full - aborting\n\r");
		abort ();
	}

	memset (alloc, 0xaa, allocSize - sizeof (unsigned int));
	
	return (void*) alloc;
}

void free (void* f)
{
#ifdef DEBUG
	if (the_heap.trace > 0) printf ("free p 0x%x\n\r", f);
	if (the_heap.trace > 1) {printf ("before free: "); heap_diag (the_heap.trace > 2);}
#endif

	if (f == 0) return;

	char* p = (char*) f;
	p -= sizeof (unsigned int);
	char* sentinel = the_heap.freeptr;
#ifdef DEBUG
	//printf ("sentinel 0x%x\n\r", sentinel);
#endif

	*next_block (p) = *next_block (sentinel);
	*next_block (sentinel) = p;

	the_heap.free_count++;
#ifdef DEBUG
	if (the_heap.trace > 0) printf ("[%d] free 0x%x, size %d\n\r", the_heap.free_count, p, block_len (p));
	if (the_heap.trace > 1) {printf ("after free: "); heap_diag (the_heap.trace > 2); printf ("\n\rdone\n\r");}
#endif
}

void heap_diag (bool detail)
{
#ifdef DEBUG
	unsigned int totalFree = 0;

	{
		char* p = the_heap.freeptr;
		while (p != 0)
		{
			unsigned int length = block_len (p);
			totalFree += length;
			p = (char*) *next_block (p);
		}
	}

	unsigned int totalUsed = the_heap.heap_size - totalFree;

	printf ("HEAP: base 0x%x limit 0x%x used %d / %d (%d%%) -> %d free [%d allocated]\n\r", the_heap.base_of_heap, the_heap.heap_limit, totalUsed, the_heap.heap_size, 100 * totalUsed / the_heap.heap_size, totalFree, the_heap.malloc_count - the_heap.free_count);

	if (detail)
	{
		printf (" %d malloc'd %d freed\n\r\n\r", the_heap.malloc_count, the_heap.free_count);

		char* p = the_heap.freeptr;
		int c = 0;
		while (p != 0)
		{	
			unsigned int length = block_len (p);

			printf ("[%d]\t", c++);
			if (p == the_heap.freeptr)
				printf ("sntl");
			else
				printf ("free");
			printf (" block 0x%x -> 0x%x, length %d byte(s)\n\r", p, p + length, length);
		
			dump_memory ((unsigned char*) p, 16);
			p = (char*) *next_block (p);
		}
	}
#endif
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

long readdec (const char* c)
{
	bool negate = false;

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

long readhex (const char* c)
{
	long result = 0;
	while (*c != '\0')
	{
		if (isdigit (*c))
		{
			result *= 16;
			result += (*c - '0');
		}
		else if (*c >= 'a' && *c <= 'f')
		{
			result *= 16;
			result += (*c - 'a' + 10);
		}
		else if (*c >= 'A' && *c <= 'F')
		{
			result *= 16;
			result += (*c - 'A' + 10);
		}
		c++;
	}

	return result;
}

long atol (const char* str)
{
	const char* c = str;

	// skip whitespace
	while (*c != '\0' && (*c == ' ' || *c == '\t')) c++;
    if (*c == '\0') return 0;

	if (*c == '0' && *(c + 1) == 'x')
		return readhex (c + 2);
	else
		return readdec (c);
}
