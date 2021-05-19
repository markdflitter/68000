#include "MC68230.h"

class IDE
{
public:
	IDE (unsigned int baseAddress = 0xA00000);

	struct DiskInfo {
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
	
	bool ident (DiskInfo& result);

	bool write (unsigned long LBA, unsigned char data [512]);
	bool read (unsigned long LBA, unsigned char data [512]);
private:
	MC68230 m_controller;

	unsigned char readRegister (unsigned char reg);
	void writeRegister (unsigned char reg, unsigned char value);

	unsigned short readData ();
	void writeData (unsigned short value);
		
	bool hasError ();
	void printError ();

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
		
		unsigned char read8 ();
		void write8 (unsigned char value);
	
		unsigned short read16 ();
		void write16 (unsigned short value);
	private:
		void reset ();
		void setAddress ();
	
		enum eState {assert, negate};	
		void setStrobe (unsigned char strobe, eState state);
		
		void negateBits (unsigned char bits);
		void assertBits (unsigned char bits);
		void writeState ();

		unsigned char m_register;
		unsigned char m_state;
		MC68230& m_controller;
	};

	RegisterAccess accessRegister (unsigned char reg);
};
