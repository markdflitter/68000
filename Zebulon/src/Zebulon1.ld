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

	.rodata :
	{
		*(.rodata)
	} > RAM

	.ehframe :
	{
		*(.ehframe)
	} > RAM


	__ctors_begin = . ;
	.ctors :
	{
		*(.ctors*)
	} > RAM
	__ctors_end = . ;
	
	.bss (NOLOAD) :
	{
		*(.bss)
	    *(COMMON)
	} > RAM

	__end = . ;
}
