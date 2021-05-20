#ifndef DUART_H
#define DUART_H

class DUART
{
public:
	DUART (unsigned int baseAddress = 0xC00001);

	void setInterruptVector (unsigned int vector);
	void enableInterrupts ();
	void disableInterrupts ();

	enum eChannel {channelA = 0, channelB	= 1};
	void writeChar (eChannel channel, char c);
	char readChar (eChannel channel) const;
private:
  	class UART {
		public:
			UART (unsigned int baseAaddress);
			void writeChar (char c);
			char readChar () const;
		private:
			static const char TX_READY = 0x4;
			static const char RX_READY = 0x1;

			struct __attribute__((__packed__)) Registers
			{
				union {
					volatile char mr1;
					volatile char mr2;
				};
				char _1;
				union {
					volatile char csr;
					volatile char sr;
				};
				char _2;
				union {
					volatile char cr;
					volatile char test;
				};
				char _3;
				union {
					volatile char tx;
					volatile char rx;
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
				volatile char acr;
				volatile char ipcr;
			};
			char _2;
			union {
				volatile char imr;
				volatile char isr;
			};
			char _3;
			union {
				volatile char crur;
				volatile char ctu;
			};
			char _4;
			union {
				volatile char ctlr;
				volatile char ctl;
			};
			char _5[8];		// skip port b
			volatile char ivr;
			char _6;
			union {
				volatile char opcr;
				volatile char ip;
			};
			char _7;
			union {
				volatile char set_op_command;
				volatile char start_counter;
			};
			char _8;
			union {
				volatile char reset_op_command;
				volatile char stop_counter;
			};
		};
	
		Registers* m_registers;
};

#endif
