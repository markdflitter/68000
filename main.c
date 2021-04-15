#define DISPLAY 0xE00000


void f (char n)
{
	n = n / 2;
	char* p = (char*) DISPLAY;
	*p = n; 
}

void main (void)
{
	f ('\5');
	//f (7);
}
