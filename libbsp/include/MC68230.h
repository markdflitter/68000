#ifndef MC68230_H
#define MC68230_H

#include <rw.h>

class MC68230
{
public:
	MC68230 (unsigned int baseAddress = 0xA00000);

	enum eDirection {in = 0x0, out = 0xFF}; 

	inline void setGeneralControl (unsigned char control)
	{
		write (&m_registers->port_general_control, control);
	}

	inline void setPortAControl (unsigned char control)
	{
		write (&m_registers->port_a_control, control);
	}

	inline void setPortADirection (eDirection direction)
	{
		write (&m_registers->port_a_data_direction, direction);
	}

	inline void setPortBControl (unsigned char control)
	{
		write (&m_registers->port_b_control, control);
	}

	inline void setPortBDirection (eDirection direction)
	{
		write (&m_registers->port_b_data_direction, direction);
	}

	inline void setPortCDirection (eDirection direction)
	{
		write (&m_registers->port_c_data_direction, direction);
	}

	inline unsigned char readPortA () const
	{	
		return read (&m_registers->port_a_data);
	}

	inline void writePortA (unsigned char value)
	{	
		write (&m_registers->port_a_data, value);
	}

	inline unsigned char readPortB () const
	{	
		return read (&m_registers->port_b_data);
	}

	inline void writePortB (unsigned char value)
	{	
		write (&m_registers->port_b_data, value);
	}

	inline unsigned char readPortC () const
	{	
		return read (&m_registers->port_c_data);
	}

	inline void writePortC (unsigned char value)
	{	
		write (&m_registers->port_c_data, value);
	}
private:
	struct __attribute__((__packed__)) Registers
	{
		unsigned char _0;

		volatile unsigned char port_general_control;
		unsigned char _1;

		volatile unsigned char port_service_request;
		unsigned char _2;

		volatile unsigned char port_a_data_direction;
		unsigned char _3;

		volatile unsigned char port_b_data_direction;
		unsigned char _4;

		volatile unsigned char port_c_data_direction;
		unsigned char _5;

		volatile unsigned char port_interrupt_vector;
		unsigned char _6;

		volatile unsigned char port_a_control;
		unsigned char _7;

		volatile unsigned char port_b_control;
		unsigned char _8;

		volatile unsigned char port_a_data;
		unsigned char _9;

		volatile unsigned char port_b_data;
		unsigned char _10;

		volatile unsigned char port_a_alternate;
		unsigned char _11;

		volatile unsigned char port_b_alternate;
		unsigned char _12;

		volatile unsigned char port_c_data;
		unsigned char _13;

		volatile unsigned char port_status;
	};

	Registers* m_registers;
};

#endif
