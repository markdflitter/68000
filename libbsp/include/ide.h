#include "MC68230.h"

class ide
{
public:
	ide (unsigned int base_address = 0xA00000);

	void test ();
private:
	MC68230 _controller;

	enum eRegisters {eDeviceSelect = 0x6};
	unsigned char read_register (unsigned char reg);

	class register_access {
	public:
		register_access (MC68230& controller, unsigned char reg);
		
		unsigned char read ();
	private:
		inline void setup ();
		inline void reset ();


		enum ePin {eCS = 0x8, eWrite = 0x10, eRead = 0x20};
		inline void pulse (unsigned char pin);

		unsigned char _reg;
		MC68230& _controller;
	};

	register_access access_register (unsigned char reg);
};

