class MC68230
{
public:
	MC68230 (unsigned int base_address = 0xA00000);

	inline void set_port_c_direction (unsigned char directions)
	{
		_rm->port_c_data_direction = directions;
	}

	inline void write_port_c (unsigned char value)
	{	
		_rm->port_c_data = value;
	}
private:
	struct __attribute__((__packed__)) register_map
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

	register_map* _rm;
};

