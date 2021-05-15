#include "MC68230.h"

class ide
{
public:
	ide (unsigned int base_address = 0xA00000);

	void test ();
private:
	MC68230 _controller;

	enum eRegisters {eBlockAddress0_7 = 0x3, eDeviceSelect = 0x6};
	unsigned char read_register (unsigned char reg);
	void write_register (unsigned char reg, unsigned char value);

	class register_access {
	public:
		register_access (MC68230& controller, unsigned char reg);
		
		unsigned char read ();
		void write (unsigned char value);
	private:
		void negate (unsigned char bits);
		void assert (unsigned char bits);
		void write_state ();

		void reset ();
		void setup (unsigned char pin);
		
		enum ePin {eCS = 0x8, eWrite = 0x10, eRead = 0x20};
		void pulse (unsigned char pin);

		unsigned char _reg;
		unsigned char _state;
		MC68230& _controller;
	};

	register_access access_register (unsigned char reg);
};

