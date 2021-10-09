#ifndef TRAPHELPER_H
#define TRAPHELPER_H

namespace
{

void trap (unsigned char trap_num, volatile void* pResult)
{
	asm volatile ("movel %0, %%a0\n\t"
		 		  "trap %1\n\t" : : "m" (pResult), "i" (trap_num) : "a0");
}

void* untrap ()
{
	void* pResult;
	asm ("movel %%a0, %0\n\t" : "=m" (pResult));
	return pResult;
}


void trap (unsigned char trap_num, unsigned char opcode, volatile void* pResult)
{
	asm volatile ("moveb %0, %%d0\n\t"
	              "movel %1, %%a0\n\t"
	              "trap %2\n\t" : : "m" (opcode), "m" (pResult), "i" (trap_num) : "d0", "a0");
}

void* untrap (unsigned char& opcode)
{
	void* pResult;

	asm ("moveb %%d0, %0\n\t"
		 "movel %%a0, %1\n\t" : "=m" (opcode), "=m" (pResult));

	return pResult;
}

void trap (unsigned char trap_num, unsigned char opcode, unsigned long p1, volatile void* a1, volatile void* pResult)
{
	asm volatile ("moveb %0, %%d0\n\t"
				  "movel %1, %%d1\n\t"
                  "movel %2, %%a0\n\t"
	              "movel %3, %%a1\n\t"
	              "trap %4\n\t" 
				  : : "m" (opcode), "m" (p1), "m" (pResult), "m" (a1), "i" (trap_num) : "d0", "d1", "a0", "a1");
}

void* untrap (unsigned char& opcode, unsigned long& p1, void*& a1)
{
	void* pResult;

	asm ("moveb %%d0, %0\n\t"
		 "movel %%d1, %1\n\t"
		 "movel %%a0, %2\n\t"
		 "movel %%a1, %3\n\t" : "=m" (opcode), "=m" (p1), "=m" (pResult), "=m" (a1));

	return pResult;
}


void trap (unsigned char trap_num, unsigned char opcode, const volatile void* a1, const volatile void* a2, const volatile void* a3, volatile void* pResult)
{
	asm volatile ("moveb %0, %%d0\n\t"
                  "movel %1, %%a0\n\t"
	              "movel %2, %%a1\n\t"
	              "movel %3, %%a2\n\t"
	              "movel %4, %%a3\n\t"
	              "trap %5\n\t" 
				  : : "m" (opcode), "m" (pResult), "m" (a1), "m" (a2), "m" (a3), "i" (trap_num) : "d0", "a0", "a1", "a2", "a3");
}

void* untrap (unsigned char& opcode, const volatile void*& a1, const volatile void*& a2, const volatile void*& a3)
{
	void* pResult;

	asm ("moveb %%d0, %0\n\t"
		 "movel %%a0, %1\n\t"
		 "movel %%a1, %2\n\t" 
		 "movel %%a2, %3\n\t" 
		 "movel %%a3, %4\n\t" : "=m" (opcode), "=m" (pResult), "=m" (a1), "=m" (a2), "=m" (a3));

	return pResult;
}

}

#endif
