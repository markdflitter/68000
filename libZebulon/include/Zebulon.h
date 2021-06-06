#ifndef ZEBULON_H
#define ZEBULON_H

namespace Zebulon
{

inline unsigned int _zebulon_time ()
{
	volatile unsigned int result;
	volatile unsigned int* p = &result;

	asm ("movel %0,%%a0\n\t"
		 "trap #0\n\t" : : "m" (p) : "a0");

	return result;
}

inline void _zebulon_putch (int c)
{
	volatile int cc = c;
	volatile int* p = &cc;

	asm ("moveb #1,%%d0\n\t"
		 "movel %0,%%a0\n\t"
		 "trap #1\n\t" : : "m" (p) : "d0", "a0");
}

inline int _zebulon_getch ()
{
	volatile int result;
	volatile int* p = &result;

	asm ("moveb #2,%%d0\n\t"
		 "movel %0,%%a0\n\t"
		 "trap #1\n\t" : : "m" (p) : "d0", "a0");

	return result;
}	

inline int* _zebulon_fopen (const char* filename, const char* mode)
{
	volatile const char* f = filename;
	volatile const char* m = mode;
	
	volatile int result;
	volatile int* p = &result;

	asm ("moveb #1,%%d0\n\t"
		 "movel %0,%%a0\n\t"
		 "movel %1,%%a1\n\t"
		 "movel %2,%%a2\n\t"
		 "trap #2\n\t" : : "m" (f), "m" (m), "m" (p) : "d0", "a0", "a1", "a2");

	return (int*) result;
}

inline long unsigned int _zebulon_fwrite (const void* data, long unsigned int data_size, long unsigned int number_data, void* fptr)
{
	return 0;
}

inline long unsigned int _zebulon_fread (const void* data, long unsigned int data_size, long unsigned int number_data, void* fptr)
{
	return 0;
}

inline void _zebulon_fclose (int* fptr)
{
}

}

#endif

