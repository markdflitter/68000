#include "MC68230.h"

class ide
{
public:
	ide (unsigned int base_address = 0xA00000);

	void test ();
private:
	MC68230 _controller;

	unsigned char read_register (unsigned char reg);
	void write_register (unsigned char reg, unsigned char value);

	class register_access {
	public:
		register_access (MC68230& controller, unsigned char reg);
		
		unsigned char read ();
		void write (unsigned char value);
	private:
		void reset ();
		void set_address ();
	
		enum eState {assert, negate};	
		void set_strobe (unsigned char strobe, eState state);
		
		void negate_bits (unsigned char bits);
		void assert_bits (unsigned char bits);
		void write_state ();

		unsigned char _reg;
		unsigned char _state;
		MC68230& _controller;
	};

	register_access access_register (unsigned char reg);
};

