#include "MC68230.h"

class ide
{
public:
	ide (unsigned int base_address = 0xA00000);

	void ident ();
private:
	MC68230 _controller;

	unsigned char read_register (unsigned char reg);
	void write_register (unsigned char reg, unsigned char value);

	unsigned short read_data ();
	void write_data (unsigned short value);
		
	bool has_error ();
	void print_error ();

	void wait (unsigned char what);
	void wait_not (unsigned char what);

	void wait_drive_ready ();
	void wait_not_busy ();
	void wait_drq ();

	unsigned char read_status ();

	void send_command (unsigned char command);

	class register_access {
	public:
		register_access (MC68230& controller, unsigned char reg);
		
		unsigned char read8 ();
		void write8 (unsigned char value);
	
		unsigned short read16 ();
		void write16 (unsigned short value);
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

