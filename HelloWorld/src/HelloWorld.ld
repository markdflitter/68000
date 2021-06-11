MEMORY
{
	RAM (wx) : org = 0x200000, l = 1M
}

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
}
