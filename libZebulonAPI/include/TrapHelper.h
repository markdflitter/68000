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


void trap (unsigned char trap_num, unsigned char opcode, void* p1)
{
	unsigned char d0 = opcode;
	void* a0 = p1;
	asm volatile ("moveb %0, %%d0\n\t"
	              "movel %1, %%a0\n\t"
	              "trap %2\n\t" : : "m" (d0), "m" (a0), "i" (trap_num) : "d0", "a0");
}


void* untrap (unsigned char& opcode)
{
	void* a0;

	asm ("moveb %%d0, %0\n\t"
		 "movel %%a0, %1\n\t" : "=m" (opcode), "=m" (a0));

	return a0;
}


/*
void trap (unsigned char trap_num, unsigned char opcode, int a0)
{
	asm ("moveb %0, %%d0\n\t" : : "m" (opcode) : "d0"
		 "movel %0, %%a0\n\t" : : "m" (&result) : "a0"
	     "trap %0\n\t" : : "i" (trap_num));
}


void trap (unsigned char trap_num, unsigned char opcode, int p1)
{
	asm ("moveb %0, %%d0\n\t" : : "m" (opcode) : "d0"
		 "movel %0, %%a0\n\t" : : "m" (&result) : "a0"
	     "trap %0\n\t" : : "i" (trap_num));
}

*/

/*
inline void call_trap (unsigned char trap)
{
	asm ("trap %0\n\t" : : "i" (trap));
}


inline void setResultPtr (volatile void* p)
{
	//volatile void* a0 = p;
	asm volatile ("movel %0, %%a0\n\t" : : "m" (p) : "a0");
}


inline volatile void* getResultPtr ()
{
    volatile void* a0 = 0;
v	asm volatile ("movel %%a0, %0\n\t" : "=m" (a0));
y	
	return a0;
}

inline void setA1 (volatile void* p)
{
	volatile void* a1 = p;
	asm volatile ("movel %0, %%a1\n\t" : : "m" (a1) : "a1");
}


inline volatile void* getA1 ()
{
    volatile void* a1 = 0;
	asm volatile ("movel %%a1, %0\n\t" : "=m" (a1));
	
	return a1;
}


inline void setOpcode (unsigned char opcode)
{
	unsigned char d0 = opcode;
v	asm ("moveb %0, %%d0\n\t" : : "i" (d0) : "d0");
}


inline unsigned char getOpcode ()
{
	volatile unsigned char d0 = 0;
	asm volatile ("moveb %%d0, %0\n\t" : "=m" (d0));
	return d0;
}


inline void setP1 (unsigned long p1)
{
	unsigned long d1 = p1;
	asm ("movel %0, %%d1\n\t" : : "m" (d1) : "d1");
}


inline volatile unsigned long getP1 ()
{
	volatile unsigned long d1 = 0;
	asm volatile ("movel %%d1, %0\n\t" : "=m" (d1));
	return d1;
}
*/
}

#endif
