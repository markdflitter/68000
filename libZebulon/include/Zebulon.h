#ifndef ZEBULON_H
#define ZEBULON_H

namespace Zebulon
{
const unsigned char nop = 0x0;

const unsigned char time = 0x0;
const unsigned char con_io = 0x1;
	const unsigned char putch = 0x1;
	const unsigned char getch = 0x2;
const unsigned char file_io = 0x2;
	const unsigned char fopen = 0x1;
	const unsigned char fwrite = 0x2;
	const unsigned char fread = 0x3;
	const unsigned char fclose = 0x4;
	const unsigned char feof = 0x5;
const unsigned char stat = 0x3;
const unsigned char find = 0x4;
	const unsigned char first = 0x1;
	const unsigned char next = 0x2;
	const unsigned char close = 0x3;
const unsigned char filer = 0x5;
 	const unsigned char rm = 0x1;
const unsigned char boot = 0x6;
 	const unsigned char doboot = 0x1;
 	const unsigned char unboot = 0x2;
 	const unsigned char index = 0x3;
const unsigned char disk = 0x7;
 	const unsigned char format = 0x1;
const unsigned char dump_block = 0x8;
 	const unsigned char do_dumpblock = 0x1;
   
template <unsigned char n> void call_trap (unsigned char opcode, volatile void* p0 = 0, volatile void* p1 = 0, volatile void* p2 = 0, unsigned long int length = 0)
{
	volatile unsigned char d0 = opcode;
	volatile unsigned long int d1 = length;
	volatile void* a0 = p0;
	volatile void* a1 = p1;
	volatile void* a2 = p2;

	asm ("moveb %1, %%d0\n"
		 "movel %2, %%d1\n"
		 "movel %3, %%a0\n"
		 "movel %4, %%a1\n"
		 "movel %5, %%a2\n"
		 "trap %0\n" : : "i" (n), "m" (d0), "m" (d1), "m" (a0), "m" (a1), "m" (a2) : "d0", "d1", "a0", "a1", "a2");
}

inline unsigned int _zebulon_time ()
{
	volatile unsigned int result;
	call_trap<time> (nop, &result);
	return result;
}

inline void _zebulon_putch (int c)
{
	volatile int cc = c;
	call_trap<con_io> (putch, &cc);
}

inline int _zebulon_getch ()
{
	volatile int result;
	call_trap <con_io> (getch, &result);
	return result;
}	

inline int _zebulon_fopen (const char* filename, const char* mode)
{
	volatile int fptr;
	call_trap <file_io> (fopen, &fptr, (char*) filename, (char*) mode);
	return fptr == -1 ? 0 : fptr + 1;
}

inline int _zebulon_feof (int fptr)
{
	volatile int f = fptr - 1;
	volatile int result;
	call_trap<file_io> (feof, &f, 0, &result);
	return result;
}

inline long unsigned int _zebulon_fwrite (const void* data, long unsigned int data_size, long unsigned int number_data, int fptr)
{
	volatile int f = fptr - 1;
	volatile long unsigned int result;
	call_trap <file_io> (fwrite, &f, (void*) data, &result, data_size * number_data);
	return result;
}

inline long unsigned int _zebulon_fread (void* data, long unsigned int data_size, long unsigned int number_data, int fptr)
{
	volatile int f = fptr - 1;
	volatile long unsigned int result;
	call_trap <file_io> (fread, &f, (void*) data, &result, data_size * number_data);
	return result;
}

inline void _zebulon_fclose (int fptr)
{
	volatile int f = fptr - 1;
	call_trap <file_io> (close, &f);
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
	bool result;
	call_trap <stat> (nop, (char*) filename, zs, &result);
	return result;
}

inline int _zebulon_find_first_file (struct _zebulon_stat* s)
{
	int result;
	call_trap <find> (first, s, 0, &result);
	return result;
}

inline bool _zebulon_find_next_file (int find_handle, struct _zebulon_stat* s)
{
	int fh = find_handle;
	bool result;
	call_trap <find> (next, s, &fh, &result);
	return result;
}

inline void _zebulon_close_find (int find_handle)
{
	volatile int fh = find_handle;
	call_trap <find> (close, 0, &fh);
}

inline void _zebulon_delete_file (const char* filename)
{
	call_trap <filer> (rm, (char*) filename);
}	

inline void _zebulon_boot (const char* filename, unsigned int bootSlot)
{
	volatile unsigned int b = bootSlot;
	call_trap <boot> (doboot, &b, (char*) filename);
}

inline void _zebulon_unboot (unsigned int bootSlot)
{
	volatile unsigned int b = bootSlot;
	call_trap <boot> (unboot, &b);
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
	call_trap <boot> (index, bte);
}

inline void _zebulon_format (unsigned int blocks)
{
	volatile unsigned int b = blocks;
	call_trap <disk> (format, &b);
}

inline void _zebulon_dump_block (unsigned int block)
{
	volatile unsigned int b = block;
	call_trap <dump_block> (do_dumpblock, &b);
}


}
#endif

