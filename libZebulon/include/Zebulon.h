#ifndef ZEBULON_H
#define ZEBULON_H

namespace Zebulon
{
enum eTrap {get_time = 0x0, char_io = 0x1};
const unsigned char putch = 0x1;
const unsigned char getch = 0x2;

inline void trap (eTrap trap)
{
	asm ("trap %0" : : "m" (trap));
}

inline void trap (eTrap trap, unsigned char opcode)
{
	asm ("moveb %1, %%d1"
		 "trap %0" : : "m" (trap), "m" (opcode) : "d1");
}

inline void trap (eTrap trap, volatile void* p0)
{
	asm ("movel %1, %%a0"
		 "trap %0" : : "m" (trap), "m" (p0): "a0");
}

inline void trap (eTrap trap, unsigned char opcode, volatile void* p0)
{
	asm ("moveb %1, %%d1"
 		 "movel %2, %%a0"
		 "trap %0" : : "m" (trap), "m" (opcode), "m" (p0): "d1", "a0");
}

inline unsigned int _zebulon_time ()
{
	volatile unsigned int result;
	trap (get_time, &result);
	return result;
}

inline void _zebulon_putch (int c)
{
	volatile int cc = c;
	trap (char_io, putch, &cc);
}

inline int _zebulon_getch ()
{
	volatile int result;
	trap (char_io, getch, &result);
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
	unsigned int index;

	char name [MAX_FILENAME_LENGTH + 1];

	long unsigned int size;	
	long unsigned int sizeOnDisk;	

	bool bootable;
	unsigned int loadAddress;
	unsigned int goAddress;
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
		 "trap #3\n\t" : : "m" (a0), "m" (a1), "m" (a2) : "a0", "a1", "a2");

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

inline void _zebulon_delete_file (const char* filename)
{
	const volatile void* a0 = filename;

	asm ("movel %0, %%a0\n\t"
		 "trap #5\n\t" : : "m" (a0) : "a0");
}

inline void _zebulon_boot (const char* filename, unsigned int bootSlot)
{
	volatile unsigned int b = bootSlot;
	volatile void* a0 = &b;

	const volatile void* a1 = filename;

	asm ("moveb #1, %%d0\n\t"
		 "movel %0, %%a0\n\t"
		 "movel %1, %%a1\n\t"
		 "trap #6\n\t" : : "m" (a0), "m" (a1) : "d0", "a0", "a1");
}

inline void _zebulon_unboot (unsigned int bootSlot)
{
	volatile unsigned int b = bootSlot;
	volatile void* a0 = &b;

	asm ("moveb #2, %%d0\n\t"
		 "movel %0, %%a0\n\t"
		 "trap #6\n\t" : : "m" (a0) : "d0", "a0");
}

struct _zebulon_boot_table_entry
{
	bool empty;

	unsigned int index;
 	char name [20];
	unsigned int file_index;

	long unsigned int size;	
	unsigned int loadAddress;
	unsigned int goAddress;

	long unsigned int startBlock;
};

inline void _zebulon_index (_zebulon_boot_table_entry bte [10])
{
	volatile void* a0 = bte;

	asm ("moveb #3, %%d0\n\t"
		 "movel %0, %%a0\n\t"
		 "trap #6\n\t" : : "m" (a0) : "d0", "a0");
}

inline void _zebulon_format (unsigned int blocks)
{
	volatile unsigned int b = blocks;
	volatile void* a0 = &b;

	asm ("movel %0, %%a0\n\t"
		 "trap #7\n\t" : : "m" (a0) : "a0");
}

inline void _zebulon_dump_block (unsigned int block)
{
	volatile unsigned int b = block;
	volatile void* a0 = &b;

	asm ("movel %0, %%a0\n\t"
		 "trap #8\n\t" : : "m" (a0) : "a0");
}


}
#endif

