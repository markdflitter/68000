#ifndef TRAPHELPER_H
#define TRAPHELPER_H

namespace
{

struct trap_params
{
	trap_params (
		unsigned char _opcode = 0,
		volatile void* _pResult = 0,
		const void* _a1 = 0,
		const void* _a2 = 0,
		const void* _a3 = 0) :
		opcode (_opcode),pResult (_pResult), a1 (_a1), a2 (_a2), a3 (_a3)
		{
		}

	unsigned char opcode;
	volatile void* pResult;
	const void* a1;
	const void* a2;
	const void* a3;
};

inline void trap (unsigned char trap_num, const trap_params& tp)
{
	asm volatile ("moveb %0, %%d0\n\t"
                  "movel %1, %%a0\n\t"
	              "movel %2, %%a1\n\t"
	              "movel %3, %%a2\n\t"
	              "movel %4, %%a3\n\t"
	              "trap %5\n\t" 
				  : : "m" (tp.opcode), "m" (tp.pResult), "m" (tp.a1), "m" (tp.a2), "m" (tp.a3), "i" (trap_num) : "d0", "a0", "a1", "a2", "a3");
}

inline trap_params untrap ()
{
	trap_params tp;

	asm ("moveb %%d0, %0\n\t"
		 "movel %%a0, %1\n\t"
		 "movel %%a1, %2\n\t" 
		 "movel %%a2, %3\n\t" 
		 "movel %%a3, %4\n\t" : "=m" (tp.opcode), "=m" (tp.pResult), "=m" (tp.a1), "=m" (tp.a2), "=m" (tp.a3));

	return tp;
}

}

#endif
