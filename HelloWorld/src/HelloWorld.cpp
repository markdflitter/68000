#include <stdio.h>

int main (void)
{
	puts ("HelloWorld\n\r");
	char* p = new char;
	*p = 'x';
	delete p;
}

