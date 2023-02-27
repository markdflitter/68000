
_HEAP_SIZE = 10K;

SECTIONS
{
	. = 0x270000 + SIZEOF_HEADERS;
	.text : { *(.text*) }

	.ctors :
	{
		__ctors_begin = . ;
		*(.ctors*)
		__ctors_end = . ;
	}
	
	.bss (NOLOAD) :
	{
		__bss_begin = . ;
		*(.bss)
	    *(COMMON)
		__bss_end = . ;
	}

	__end = . ;
	__heap_start = . ;
	. += _HEAP_SIZE ;
	__heap_end = . ;
}
