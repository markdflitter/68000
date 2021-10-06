#ifndef TRAPHELPER_H
#define TRAPHELPER_H

namespace
{

inline void call_trap (unsigned char trap)
{
	asm ("trap %0\n\t" : : "i" (trap));
}


inline void setA0 (volatile void* p)
{
	volatile void* a0 = p;
	asm volatile ("movel %0, %%a0\n\t" : : "m" (a0) : "a0");
}


inline volatile void* getA0 ()
{
    volatile void* a0 = 0;
	asm volatile ("movel %%a0, %0\n\t" : "=m" (a0));
	
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
	asm ("moveb %0, %%d0\n\t" : : "m" (d0) : "d0");
}


inline unsigned char getOpcode ()
{
	volatile char d0 = 0;
	asm volatile ("moveb %%d0, %0\n\t" : "=m" (d0));
	return d0;
}


inline void setP1 (unsigned long p1)
{
	unsigned long d1 = p1;
	asm ("movel %0, %%d1\n\t" : : "m" (d1) : "d1");
}


inline unsigned long getP1 ()
{
	volatile long d1 = 0;
	asm volatile ("movel %%d1, %0\n\t" : "=m" (d1));
	return d1;
}

}

#endif
