
_HEAP_SIZE = 10K;

SECTIONS
{
	. = 0x270000 + SIZEOF_HEADERS;
	.text : { *(.text*) }
	__end = . ;
	__heap_start = . ;
	. += _HEAP_SIZE ;
	__heap_end = . ;

}
