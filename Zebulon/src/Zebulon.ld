MEMORY
{
	RAM (wx) : org = 0x200000, l = 1M
}

SECTIONS
{
	.text :
	{
		*(.text*)
	} > RAM

	.bss (NOLOAD) :
	{
		*(.bss)
	    *(COMMON)
	} > RAM

	__end = . ;
}
