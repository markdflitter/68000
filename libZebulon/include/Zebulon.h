#ifndef ZEBULON_H
#define ZEBULON_H

namespace Zebulon
{

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
	int cc = c;
	void* a0 = &cc;

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

	const void* a1 = filename;
	const void* a2 = mode;
	
	asm ("moveb #1, %%d0\n\t"
		 "movel %0, %%a0\n\t"
		 "movel %1, %%a1\n\t"
		 "movel %2, %%a2\n\t"
		 "trap #2\n\t" : : "m" (a0), "m" (a1), "m" (a2) : "d0", "a0", "a1", "a2");

	return fptr;
}

inline long unsigned int _zebulon_fwrite (const void* data, long unsigned int data_size, long unsigned int number_data, int fptr)
{
	int f = fptr;
	const void* a0 = &f;
	const void* a1 = data;

	volatile long unsigned int result;
	volatile void* a2 = &result;

	long unsigned int d1 = data_size * number_data;

	asm ("moveb #4, %%d0\n\t"
		 "movel %0, %%a0\n\t"
		 "movel %1, %%a1\n\t"
		 "movel %2, %%a2\n\t"
		 "movel %3, %%d1\n\t"
		 "trap #2\n\t" : : "m" (a0), "m" (a1), "m" (a2), "m" (d1) : "d0", "a0", "a1", "a2", "d1");

	return result;
}

inline long unsigned int _zebulon_fread (const void* data, long unsigned int data_size, long unsigned int number_data, int fptr)
{
	int f = fptr;
	const void* a0 = &f;
	const void* a1 = data;

	volatile long unsigned int result;
	volatile void* a2 = &result;

	long unsigned int d1 = data_size * number_data;

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
	int f = fptr;
	volatile void* a0 = &f;

	asm ("moveb #4,%%d0\n\t"
		 "movel %0,%%a0\n\t"
		 "trap #2\n\t" : : "m" (a0) : "d0", "a0");
}

}

#endif

