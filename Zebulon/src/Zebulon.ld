MEMORY
{
	RAM (wx) : org = 0x200000, l = 1M
}

_HEAP_SIZE = 256K;

SECTIONS
{
	.reserved :
	{
		__vector_table = . ;
		. += 1024;
	} > RAM

	.text :
	{
		__begin = . ;
		*(.text*)
	} > RAM

	.ctors :
	{
		__ctors_begin = . ;
		*(.ctors*)
		__ctors_end = . ;
	} > RAM
	
	.bss (NOLOAD) :
	{
		__bss_begin = . ;
		*(.bss)
	    *(COMMON)
		__bss_end = . ;
	} > RAM

	__end = . ;
	__heap_start = . ;
	. += _HEAP_SIZE ;
	__heap_end = . ;

	__stack = 0x2FFFFC;
}
