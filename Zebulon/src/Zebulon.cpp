#include <stdio.h>
#include <string.h>
#include <ide.h>

void printhelp (void)
{
	printf ("exit\t - exit to monitor\n\r");
	printf ("version\t - print version\n\r");
	printf ("help\t - print this help\n\r");
	printf ("ident\t - ident the disk\n\r");
	printf ("write\t - write data to disk\n\r");
	printf ("read\t - read data from disk\n\r");
	printf ("save\t - save code to disk\n\r");
}

void ident ()
{
	ide i;
	ide::disk_info info;
	if (i.ident (info))
	{
		printf ("model number\t\t\t\t: %s\n\r",info.model_number);
		printf ("serial number\t\t\t\t: %s\n\r",info.serial_number);
		printf ("firmware revision\t\t\t: %s\n\r",info.firmware_revision);
		printf ("\n\r");

		if (info.fixed_drive) printf ("fixed drive\n\r");
		if (info.removeable_cartridge_drive) printf ("removeable cartridge drive\n\r");
		if (info.hard_sectored) printf ("hard sectored\n\r");
		if (info.soft_sectored) printf ("soft sectored\n\r");
		if (info.LBA_supported) printf ("LBA supported\n\r");
		if (info.DMA_supported) printf ("DMA supported\n\r");
		if (info.not_MFM_encoded) printf ("not MFM encoded\n\r");
		if (info.head_switch_time_15uS) printf ("head switch time > 15uS\n\r");
		if (info.transfer_rate_lt_5Mbs) printf ("disk transfer rate < 5Mbs\n\r");
		if (info.transfer_rate_lt_10Mbs) printf ("disk transfer rate > 5Mbs buyt <- 10Mbs\n\r");
		if (info.transfer_rate_gt_10Mbs) printf ("disk transfer rate > 10Mbs\n\r");
		if (info.double_word_io) printf ("can perform double word io\n\r");
		if (info.spindle_motor_control_option_implemented) printf ("spindle motor control option implemented\n\r");
		if (info.data_strobe_offset_option_available) printf ("data strobe offset option availabe\n\r");
		if (info.track_offset_option_available) printf ("track offset option available\n\r");
		if (info.format_speed_tolerance_gap_required) printf ("format speed tolerance gap required\n\r");
		printf ("\n\r");

		printf ("number of cylinders\t\t\t: %d\n\r",info.num_cylinders);
		printf ("number of heads\t\t\t\t: %d\n\r",info.num_heads);
		printf ("bytes per track\t\t\t\t: %d\n\r",info.num_bytes_per_track);
		printf ("bytes per sector\t\t\t: %d\n\r",info.num_bytes_per_sector);
		printf ("sectors per track\t\t\t: %d\n\r",info.num_sectors_per_track);
		printf ("\n\r");

		printf ("buffer type\t\t\t\t: %x\n\r",info.buffer_type);
		printf ("buffer size\t\t\t\t: %d\n\r",info.buffer_size);
		printf ("number of ECC bytes\t\t\t: %d\n\r",info.num_ECC_bytes);
		printf ("max rw sectors per interrupt\t\t: %x\n\r",info.max_rw_sectors_per_interrupt);
		printf ("\n\r");

		printf ("PIO mode\t\t\t\t: %d\n\r",info.PIO_mode);
		printf ("DMA mode\t\t\t\t: %d\n\r",info.DMA_mode);	
		printf ("\n\r");

		printf ("number of current cylinders\t\t: %d\n\r",info.num_current_cylinders);
		printf ("number of current heads\t\t\t: %d\n\r",info.num_current_heads);
		printf ("number of current sectors per track\t: %d\n\r",info.num_current_sectors_per_track);
		printf ("current capacity in sectors\t\t: %d\n\r",info.current_capacity_in_sectors);
		printf ("total number of user sectors\t\t: %d\n\r",info.total_num_of_user_sectors);
		printf ("\n\r");
		
		printf ("single word DMA modes supported\t\t: %d\n\r",info.singleword_DMA_modes_supported);
		printf ("single word DMA modes active\t\t: %d\n\r",info.singleword_DMA_modes_active);
		printf ("multi word DMA modes supported\t\t: %d\n\r",info.multiword_DMA_modes_supported);
		printf ("multi word DMA modes active\t\t: %d\n\r",info.multiword_DMA_modes_active);
	}
	else
		printf ("ERROR: ident failed\n\r");
}

const unsigned long lba = 512;

void write ()
{
	unsigned char data [512] = "The house stood on a slight rise just on the edge of the village. It stood on its own and looked out over a broad spread of West Country farmland. Not a remarkable house by any means—it was about thirty years old, squattish, squarish, made of brick, and had four windows set in the front of a size and proportion which more or less exactly failed to please the eye.  The only person for whom the house was in any way special was Arthur Dent, and that was only because it happened to be the one he lived in.";
	
	ide i;
	i.write (lba, data);
}


void read ()
{
	unsigned char data [512];
	
	ide i;
	i.read (lba, data);

	printf ("%s\n\r",data);
}


extern char* __begin;
extern char* __end;
extern char* start;

void save ()
{
	ide i;

	const int startBlock = 513;
	int curBlock = startBlock;


	static char* begin = (char*) &__begin;
	static char* end = (char*) &__end;
	static char* entry = (char*) &start;

	printf ("start 0x%x end 0x%x entry 0x%x\n\r", begin, end, entry);

	size_t length = end - begin;
	size_t numBlocks = (length / 512) + 1;

	printf ("%d bytes, which is %d blocks\n\r", length, numBlocks);

	for (int b = 0; b < length;)
	{
		printf ("b %d\n\r", b);

		unsigned char block [512];
		if (b == 0)
		{
			memcpy (&block, &begin, 4);
			memcpy (&block [4], &end, 4);
			memcpy (&block [8], &entry, 4);
			memcpy (&block [12], begin, 500);
			b += 500;
		}
		else
		{
			memcpy (&block, begin + b, 512);
			b += 512;
		}

		printf ("writing block %d\n\r", curBlock);
		i.write (curBlock, block);
		curBlock++;
	}
}


int main ()
{
	const char* version = "Zebulon V1.9";
	printf ("%s\n\r",version);

	printf ("type help for help\n\r");

	char buf [21];
	int exit = 0;
	while (!exit)
	{
		printf ("$ ");
		char* p = gets (buf);
		*p = '\0';
		printf ("\n\r");

		if (strcmp (buf, "exit") == 0) exit = 1;
		if (strcmp (buf, "version") == 0) printf ("%s",version);
		if (strcmp (buf, "help") == 0) printhelp ();
		if (strcmp (buf, "ident") == 0) ident ();
		if (strcmp (buf, "read") == 0) read ();
		if (strcmp (buf, "write") == 0) write ();
		if (strcmp (buf, "save") == 0) save ();
	}

	return 0;
}

