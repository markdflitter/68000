#define DISPLAY 0xE00001


void f (char n)
{
	char* p = (char*) DISPLAY;
	*p = n; 
}

void main (void)
{
	f (6);
	f (5);
}
