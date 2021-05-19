#include "space_manager.h"

class FAT
{
public:
	FAT ();
	FAT (unsigned int size);

	unsigned int create_file (char* name, size);
private:
 	space_manager _sm;
  
	struct file
	{
		char* _name;
		
	};
};
