class ide
{
public:
	ide (unsigned int base_address = 0xA00000);

	char read_status() const;
	void ident (char* buf);
private:
	struct __attribute__((__packed__)) ide_register_map
	{
		volatile unsigned short data;
		unsigned char _1 [1];
		volatile unsigned char error;
			
		unsigned char _2 [9];

		volatile unsigned char device_select;
		unsigned char _3 [1];
		volatile unsigned char command;
	};

	enum command {_ident = 0xEC};
	void send_command (char command);

	enum status {_ready = 0x40};
	void wait_ready () const;

	ide_register_map* _rm;
};

