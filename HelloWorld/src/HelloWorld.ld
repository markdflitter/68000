SECTIONS
{
	. = 0x270000 + SIZEOF_HEADERS;
	.text : { *(.text*) }
}
