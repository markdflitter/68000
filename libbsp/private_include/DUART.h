#ifndef DUART_H
#define DUART_H

class DUART
{
public:
	DUART (unsigned int baseAddress = 0xC00001);

	void setInterruptVector (unsigned int vector);
	void enableInterrupts ();
	void disableInterrupts ();
	void clearInterrupts ();
	
	void setTimerDivisor (unsigned char msb, unsigned char lsb);

	enum eChannel {channelA = 0, channelB	= 1};
	void writeChar (eChannel channel, char c);
	char readChar (eChannel channel) const;
private:
  	class UART {
		public:
			UART (unsigned int baseAaddress);
			void writeChar (char c);
			char readChar () const;
			void setCSRA (unsigned char value);
		private:
			static const char TX_READY = 0x4;
			static const char RX_READY = 0x1;

			struct __attribute__((__packed__)) Registers
			{
				union {
					volatile unsigned char mr1;
					volatile unsigned char mr2;
				};
				char _1;
				union {
					volatile unsigned char csr;
					volatile unsigned char sr;
				};
				char _2;
				union {
					volatile unsigned char cr;
					volatile unsigned char test;
				};
				char _3;
				union {
					volatile unsigned char tx;
					volatile unsigned char rx;
				};
			};

			bool spaceToSend () const;
			bool charReady () const;

			Registers* m_registers;
		};

		UART m_a;
		UART m_b;
		UART* m_uart[2];
	
		struct __attribute__((__packed__)) Registers
		{
			char _1 [8];		// skip port a
			union {
				volatile unsigned char acr;
				volatile unsigned char ipcr;
			};
			char _2;
			union {
				volatile unsigned char imr;
				volatile unsigned char isr;
			};
			char _3;
			union {
				volatile unsigned char ctur;
				volatile unsigned char ctu;
			};
			char _4;
			union {
				volatile unsigned char ctlr;
				volatile unsigned char ctl;
			};
			char _5[8];		// skip port b
			volatile unsigned char ivr;
			char _6;
			union {
				volatile unsigned char opcr;
				volatile unsigned char ip;
			};
			char _7;
			union {
				volatile unsigned char set_op_command;
				volatile unsigned char start_counter;
			};
			char _8;
			union {
				volatile unsigned char reset_op_command;
				volatile unsigned char stop_counter;
			};
		};
	
		Registers* m_registers;
};

#endif
