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
				IDNF = 0x10, MC = 0x20, UNC = 0x40, BBK = 0x80};

	Result ident (DiskInfo& result);

	Result write (unsigned long LBA, unsigned char data [512]);
	Result read (unsigned long LBA, unsigned char data [512]);
private:
	MC68230 m_controller;

	unsigned char readRegister (unsigned char reg);
	void writeRegister (unsigned char reg, unsigned char value);

	unsigned short readData ();
	void writeData (unsigned short value);
		
	bool hasError ();
	Result error ();

	void wait (unsigned char what);
	void waitNot (unsigned char what);

	void waitDriveReady ();
	void waitNotBusy ();
	void waitDRQ ();

	unsigned char readStatus ();

	void sendCommand (unsigned char command);

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
