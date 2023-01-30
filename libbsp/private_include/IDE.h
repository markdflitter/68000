#ifndef IDE_H
#define IDE_H

#include "../include/bsp.h"
#include "rw.h"
#include "MC68230.h"

class IDE
{
public:
	IDE (unsigned int baseAddress = 0xA00000);

	enum Result {OK = 0x0, AMNF = 0x1, TK0NF = 0x2, ABRT = 0x4, MCR = 0x8,
				IDNF = 0x10, MC = 0x20, UNC = 0x40, BBK = 0x80, TIMEDOUT = 0x100};

	Result ident (DiskInfo& result, get_time_fn_ptr get_time, unsigned int timeout_time);

	Result write (unsigned long LBA, unsigned char data [ide_block_size], get_time_fn_ptr get_time, unsigned int timeout_time);
	Result read (unsigned long LBA, unsigned char data [ide_block_size], get_time_fn_ptr get_time, unsigned int timeout_time);
private:
	MC68230 m_controller;

	unsigned char readRegister (unsigned char reg);
	void writeRegister (unsigned char reg, unsigned char value);

	unsigned short readData ();
	void writeData (unsigned short value);
		
	bool hasError ();
	Result error ();

	bool wait (unsigned char what, get_time_fn_ptr get_time, unsigned int timeout_time);
	bool waitNot (unsigned char what, get_time_fn_ptr get_time, unsigned int timeout_time);

	unsigned char readStatus ();

	bool sendCommand (unsigned char command, get_time_fn_ptr get_time, unsigned int timeout_time);

	void setLBA (unsigned long LBA);

	class RegisterAccess {
	public:
		RegisterAccess (MC68230& controller, unsigned char reg);
		
		inline unsigned char read8 ();
		inline void write8 (unsigned char value);
	
		inline unsigned short read16 ();
		inline void write16 (unsigned short value);
	private:
		inline void reset ();
		inline void setAddress ();
	
		enum eState {assert, negate};	
		inline void setStrobe (unsigned char strobe, eState state);
		
		inline void negateBits (unsigned char bits);
		inline void assertBits (unsigned char bits);
		inline void writeState ();

		unsigned char m_register;
		unsigned char m_state;
		MC68230& m_controller;
	};

	RegisterAccess accessRegister (unsigned char reg);
};

#endif
