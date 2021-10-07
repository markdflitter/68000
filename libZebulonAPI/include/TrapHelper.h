#ifndef TRAPHELPER_H
#define TRAPHELPER_H

namespace
{

void trap (unsigned char trap_num, void* a0)
{
	asm volatile ("movel %0, %%a0\n\t"
		 		  "trap %1\n\t" : : "m" (a0), "i" (trap_num) : "a0");
}

void* untrap ()
{
	void* a0;
	asm ("movel %%a0, %0\n\t" : "=m" (a0));
	return a0;
}


void trap (unsigned char trap_num, unsigned char opcode, volatile void* a0)
{
	asm volatile ("moveb %0, %%d0\n\t"
	              "movel %1, %%a0\n\t"
	              "trap %2\n\t" : : "m" (opcode), "m" (a0), "i" (trap_num) : "d0", "a0");
}

void* untrap (unsigned char& opcode)
{
	void* a0;

	asm ("moveb %%d0, %0\n\t"
		 "movel %%a0, %1\n\t" : "=m" (opcode), "=m" (a0));

	return a0;
}


void trap (unsigned char trap_num, unsigned char opcode, unsigned long p1, void* data, void* result)
{
	asm volatile ("moveb %0, %%d0\n\t"
				  "movel %1, %%d1\n\t"
                  "movel %2, %%a0\n\t"
	              "movel %3, %%a1\n\t"
	              "trap %4\n\t" 
				  : : "m" (opcode), "m" (p1), "m" (result), "m" (data), "i" (trap_num) : "d0", "d1", "a0", "a1");
}

void* untrap (unsigned char& opcode, unsigned long& p1, void*& data)
{
	void* a0;

	asm ("moveb %%d0, %0\n\t"
		 "movel %%d1, %1\n\t"
		 "movel %%a0, %2\n\t"
		 "movel %%a1, %3\n\t" : "=m" (opcode), "=m" (p1), "=m" (a0), "=m" (data));

	return a0;
}

}

#endif
