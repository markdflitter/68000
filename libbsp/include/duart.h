class duart
{
public:
	duart (unsigned int base_address = 0xC00001);

	void write_char (int channel, char c);
	char read_char (int channel) const;
private:
  	class uart {
		public:
			uart (unsigned int base_address);
			void write_char (char c);
			char read_char () const;
		private:
			static const char TX_READY = 0x4;
			static const char RX_READY = 0x1;

			struct __attribute__((__packed__)) uart_register_map
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

			bool space_to_send () const;
			bool char_ready () const;

			uart_register_map* _rm;
		};

		uart _a;
		uart _b;
		uart* _uart[2];
	
		struct __attribute__((__packed__)) duart_register_map
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
	
		duart_register_map* _rm;
};

