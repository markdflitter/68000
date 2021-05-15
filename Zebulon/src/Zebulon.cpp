#include <stdio.h>
#include <string.h>
#include <ide.h>

void printhelp (void)
{
	printf ("exit\t - exit to monitor\n\r");
	printf ("version\t - print version\n\r");
	printf ("help\t - print this help\n\r");
	printf ("ident\t - ident the disk\n\r");
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
	
		printf ("general\t\t\t\t\t: %x\n\r",info.general);
		printf ("number of cylinders\t\t\t: %d\n\r",info.num_cylinders);
		printf ("number of heads\t\t\t\t: %d\n\r",info.num_heads);
		printf ("bytes per track\t\t\t\t: %d\n\r",info.num_bytes_per_track);
		printf ("bytes per sector\t\t\t: %d\n\r",info.num_bytes_per_sector);
		printf ("sectors per track\t\t\t: %d\n\r",info.num_sectors_per_track);
		printf ("buffer type\t\t\t\t: %x\n\r",info.buffer_type);
		printf ("buffer size\t\t\t\t: %d\n\r",info.buffer_size);
		printf ("number of ECC bytes\t\t\t: %d\n\r",info.num_ECC_bytes);
		printf ("double word io\t\t\t\t: %x\n\r",info.double_word_io);
		printf ("capabilities\t\t\t\t: %x\n\r",info.capabilities);
		printf ("PIO mode\t\t\t\t: %d\n\r",info.PIO_mode);
		printf ("DMA mode\t\t\t\t: %d\n\r",info.DMA_mode);
		printf ("number of current cylinders\t\t: %d\n\r",info.num_current_cylinders);
		printf ("number of current heads\t\t\t: %d\n\r",info.num_current_heads);
		printf ("number of current sectors per track\t: %d\n\r",info.num_current_sectors_per_track);
		printf ("current capacity in sectors\t\t: %d\n\r",info.current_capacity_in_sectors);
		
		printf ("total number of user sectors\t\t: %d\n\r",info.total_num_of_user_sectors);
		printf ("singleword DMA\t\t\t\t: %d\n\r",info.singleword_DMA);
		printf ("multiword DMA\t\t\t\t: %d\n\r",info.multiword_DMA);
	}
	else
		printf ("ERROR: ident failed\n\r");
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
	}

	return 0;
}

