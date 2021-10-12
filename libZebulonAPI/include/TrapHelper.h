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
		const void* _a3 = 0,
		const void* _a4 = 0,
		const void* _a5 = 0,
		unsigned int _d1 = 0) :
			opcode (_opcode),pResult (_pResult), a1 (_a1), a2 (_a2), a3 (_a3), a4 (_a4), a5 (_a5), d1 (_d1)
		{
		}

	unsigned char opcode;
	volatile void* pResult;
	const void* a1;
	const void* a2;
	const void* a3;
	const void* a4;
	const void* a5;
	unsigned int d1;
};

inline void trap (unsigned char trap_num, const trap_params& tp)
{
	asm volatile ("moveb %0, %%d0\n\t"
                  "movel %1, %%a0\n\t"
	              "movel %2, %%a1\n\t"
	              "movel %3, %%a2\n\t"
	              "movel %4, %%a3\n\t"
	              "movel %5, %%a4\n\t"
	              "movel %6, %%a5\n\t"
	              "movel %7, %%d1\n\t"
			      "trap %8\n\t" 
				  : : "m" (tp.opcode), "m" (tp.pResult), "m" (tp.a1), "m" (tp.a2), "m" (tp.a3), "m" (tp.a4), "m" (tp.a5), "m" (tp.d1), "i" (trap_num) : "d0", "d1", "a0", "a1", "a2", "a3", "a4", "a5");
}

inline trap_params untrap ()
{
	trap_params tp;

	asm ("moveb %%d0, %0\n\t"
		 "movel %%a0, %1\n\t"
		 "movel %%a1, %2\n\t" 
		 "movel %%a2, %3\n\t" 
		 "movel %%a3, %4\n\t"
		 "movel %%a4, %5\n\t" 
		 "movel %%a5, %6\n\t"
		 "movel %%d1, %7\n\t" 
			: "=m" (tp.opcode), "=m" (tp.pResult), "=m" (tp.a1), "=m" (tp.a2), "=m" (tp.a3), "=m" (tp.a4), "=m" (tp.a5), "=m" (tp.d1));

	return tp;
}

}

#endif
