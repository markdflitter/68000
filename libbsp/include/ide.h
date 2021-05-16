#include "MC68230.h"

class ide
{
public:
	ide (unsigned int base_address = 0xA00000);

	struct disk_info {
		unsigned short general;
		bool hard_sectored;
		bool soft_sectored;
		bool not_MFM_encoded;
		bool head_switch_time_15uS;
		bool spindle_motor_control_option_implemented;
		bool fixed_drive;
		bool removeable_cartridge_drive;
		bool transfer_rate_lt_5Mbs;
		bool transfer_rate_lt_10Mbs;
		bool transfer_rate_gt_10Mbs;
		bool rotational_speed_tolerance;
		bool data_strobe_offset_option_available;
		bool track_offset_option_available;
		bool format_speed_tolerance_gap_required;

		unsigned short num_cylinders;
		unsigned short num_heads;
		unsigned short num_bytes_per_track;
		unsigned short num_bytes_per_sector;
		unsigned short num_sectors_per_track;
		unsigned char serial_number [20];
		unsigned short buffer_type;
		unsigned short buffer_size;
		unsigned short num_ECC_bytes;
		unsigned short firmware_revision [8];
		unsigned char model_number[40];
		unsigned short max_rw_sectors_per_interrupt;
		bool double_word_io;
		unsigned short capabilities;
		bool LBA_supported;
		bool DMA_supported;
		unsigned short PIO_mode;
		unsigned short DMA_mode;
		bool current_valid;
		unsigned short num_current_cylinders;
		unsigned short num_current_heads;
		unsigned short num_current_sectors_per_track;
		unsigned int current_capacity_in_sectors;
		unsigned int total_num_of_user_sectors;
		unsigned short current_rw_sectors_per_interrupt;
		unsigned char singleword_DMA_modes_supported;
		unsigned char singleword_DMA_modes_active;
		unsigned char multiword_DMA_modes_supported;
		unsigned char multiword_DMA_modes_active;
	};

	bool ident (disk_info& result);

	bool write (unsigned long lba, unsigned char data [512]);
	bool read (unsigned long lba, unsigned char data [512]);
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

	void set_lba (unsigned long lba);

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

