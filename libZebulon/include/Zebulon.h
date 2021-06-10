#ifndef ZEBULON_H
#define ZEBULON_H

namespace Zebulon
{
// a lot of stuff in this file is technically not volatile, but marking it sit so forces the compiler to make a local copy of the parameter which defeats a compiler bug where it refences the wrong stack location
	
inline unsigned int _zebulon_time ()
{
	volatile unsigned int result;
	volatile void* a0 = &result;

	asm ("movel %0, %%a0\n\t"
		 "trap #0\n\t" : : "m" (a0) : "a0");

	return result;
}

inline void _zebulon_putch (int c)
{
	volatile int cc = c;	// although this is technically not volatile, it forces the compiler to make a local copy of the parameter which defeats a compiler bug where it refences the wrong stack location
	volatile void* a0 = &cc;

	asm ("moveb #1, %%d0\n\t"
		 "movel %0, %%a0\n\t"
		 "trap #1\n\t" : : "m" (a0) : "d0", "a0");
}

inline int _zebulon_getch ()
{
	volatile int result;
	volatile void* a0 = &result;

	asm ("moveb #2, %%d0\n\t"
		 "movel %0, %%a0\n\t"
		 "trap #1\n\t" : : "m" (a0) : "d0", "a0");

	return result;
}	

inline int _zebulon_fopen (const char* filename, const char* mode)
{
	volatile int fptr;
	volatile void* a0 = &fptr;

	const volatile void* a1 = filename;
	const volatile void* a2 = mode;
	
	asm ("moveb #1, %%d0\n\t"
		 "movel %0, %%a0\n\t"
		 "movel %1, %%a1\n\t"
		 "movel %2, %%a2\n\t"
		 "trap #2\n\t" : : "m" (a0), "m" (a1), "m" (a2) : "d0", "a0", "a1", "a2");

	return fptr == -1 ? 0 : fptr + 1;
}

inline int _zebulon_feof (int fptr)
{
	volatile int f = fptr - 1;
	const volatile void* a0 = &f;

	volatile long unsigned int result;
	volatile void* a2 = &result;

	asm ("moveb #5, %%d0\n\t"
		 "movel %0, %%a0\n\t"
		 "movel %1, %%a2\n\t"
		 "trap #2\n\t" : : "m" (a0), "m" (a2) : "d0", "a0", "a2");

	return result;
}


inline long unsigned int _zebulon_fwrite (const void* data, long unsigned int data_size, long unsigned int number_data, int fptr)
{
	volatile int f = fptr - 1;
	const volatile void* a0 = &f;
	const volatile void* a1 = data;

	volatile long unsigned int result;
	volatile void* a2 = &result;

	volatile long unsigned int d1 = data_size * number_data;

	asm ("moveb #2, %%d0\n\t"
		 "movel %0, %%a0\n\t"
		 "movel %1, %%a1\n\t"
		 "movel %2, %%a2\n\t"
		 "movel %3, %%d1\n\t"
		 "trap #2\n\t" : : "m" (a0), "m" (a1), "m" (a2), "m" (d1) : "d0", "a0", "a1", "a2", "d1");

	return result;
}

inline long unsigned int _zebulon_fread (const void* data, long unsigned int data_size, long unsigned int number_data, int fptr)
{
	volatile int f = fptr - 1;
	const volatile void* a0 = &f;
	const volatile void* a1 = data;

	volatile long unsigned int result;
	volatile void* a2 = &result;

	volatile long unsigned int d1 = data_size * number_data;

	asm ("moveb #3, %%d0\n\t"
		 "movel %0, %%a0\n\t"
		 "movel %1, %%a1\n\t"
		 "movel %2, %%a2\n\t"
		 "movel %3, %%d1\n\t"
		 "trap #2\n\t" : : "m" (a0), "m" (a1), "m" (a2), "m" (d1) : "d0", "a0", "a1", "a2", "d1");

	return result;
}

inline void _zebulon_fclose (int fptr)
{
	volatile int f = fptr - 1;
	volatile void* a0 = &f;

	asm ("moveb #4,%%d0\n\t"
		 "movel %0,%%a0\n\t"
		 "trap #2\n\t" : : "m" (a0) : "d0", "a0");
}

#define MAX_FILENAME_LENGTH 255

struct _zebulon_stat
{
	char name [MAX_FILENAME_LENGTH + 1];
	long unsigned int size;	
};

inline bool _zebulon_stat (const char* filename, struct _zebulon_stat* zs)
{
	const volatile void* a0 = filename;
	volatile void* a1 = zs;
	bool result;
	volatile void* a2 = &result;

	asm ("movel %0, %%a0\n\t"
		 "movel %1, %%a1\n\t"
		 "movel %2, %%a2\n\t"
		 "trap #3\n\t" : : "m" (a0), "m" (a1), "m" (a2) : "a0", "a1");

	return result;
}

inline int _zebulon_find_first_file (struct _zebulon_stat* s)
{
	volatile void* a0 = s;
	int result;
	volatile void* a2 = &result;

	asm ("moveb #1, %%d0\n\t"
		 "movel %0, %%a0\n\t"
		 "movel %1, %%a2\n\t"
		 "trap #4\n\t" : : "m" (a0), "m" (a2) : "d0", "a0", "a1");

	return result;
}

inline bool _zebulon_find_next_file (int find_handle, struct _zebulon_stat* s)
{
	volatile void* a0 = s;

	int fh = find_handle;
	volatile void* a1 = &fh;

	bool result;
	volatile void* a2 = &result;

	asm ("moveb #2, %%d0\n\t"
		 "movel %0, %%a0\n\t"
		 "movel %1, %%a1\n\t"
		 "movel %2, %%a2\n\t"
		 "trap #4\n\t" : : "m" (a0), "m" (a1), "m" (a2) : "d0", "a0", "a1", "a2");

	return result;
}

inline void _zebulon_close_find (int find_handle)
{
	volatile int fh = find_handle;
	volatile void* a1 = &fh;

	asm ("moveb #3, %%d0\n\t"
		 "movel %0, %%a1\n\t"
		 "trap #4\n\t" : : "m" (a1) : "d0", "a1");
}

}

#endif

