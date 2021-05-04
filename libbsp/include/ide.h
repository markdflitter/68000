class ide
{
public:
	ide (unsigned int base_address = 0xA00000);

	char read_status() const;
	void ident (short* buf);
private:
	struct __attribute__((__packed__)) ide_register_map
	{
		short data;
		
		char _1 [13];
		char command;
	};

	enum command {_ident = 0xEC};
	void send_command (char command);

	enum status {_ready = 0x64};
	void wait_ready () const;

	ide_register_map* _rm;
};

