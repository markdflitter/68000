#include "../include/stdio.h"
#include "../include/string.h"
#include <Zebulon.h>

int getchar(void)
{
	return Zebulon::_zebulon_getch ();
}

char* gets (char* s)
{
	char* buf = s;
	for (;;)
	{
 		char c = getchar ();
		putchar (c);

		if (c == '\r')
			break;		
 		else if (c == 127) //backspace
		{
			if (buf > s) buf--;
		}
		else
			*buf++ = c;
 	}
 
	return buf;
}

int putchar (int c)
{
	Zebulon::_zebulon_putch (c);
	return 0;
}


int puts (const char* s)
{
	while (*s != 0)
	{
		putchar (*s++);
	}
	return 0;
}

void bin2hex (int num, char* buf)
{
	const char* LUT = "0123456789ABCDEF";
	if (num < 16)
	{
		buf [0] = LUT [(num & 0xF0) >> 4];
		buf [1] = LUT [num & 0xF];
		buf [2] = '\0';

		return ;
	}

	int w = num;
	for (int i = 0; i < 8; i++)
	{
		buf [7 - i] = LUT [w & 0xF];
		w = w >> 4;
	}

	buf [8] = '\0';

	char* p = buf;
	while (*p++ == '0')
		;

	if (p != buf) memcpy (buf, p - 1, 9 - (p - 1 - buf));
}

void bin2dec (unsigned int num, char* buf)
{
	if (num == 0)
	{
		buf [0] = '0';
		buf [1] = '\0';
		return ;
	}

	unsigned int w = num;
	const char* LUT = "0123456789";

	int i = 0;
	while (w != 0)
	{
		buf [i++] = LUT [w % 10];
		w = w / 10;
	}
	buf[i] = '\0';

	for (int j = 0; j < i/2; j++)
	{
		char temp = buf [j];
		buf [j] = buf [i - j - 1];
		buf [i - j - 1] = temp;
	}
}

int vsprintf (char* str, const char* format, va_list ap)
{
	char ch;
	while ((ch = *format++) != '\0')
    {
		if (ch == '%')
		{
			if (*format == 'x')
			{
				int i = va_arg (ap, int);
				char buf [9];
				bin2hex (i, buf);
				memcpy (str, buf, strlen (buf));
				str += strlen (buf);
			}
			else if (*format == 'd')
			{
				unsigned int i = va_arg (ap, unsigned int);
				char buf [11];
				bin2dec (i, buf);
				memcpy (str, buf, strlen (buf));
				str += strlen (buf);
			}
			else if (*format == 's')
			{
				char* s = va_arg (ap, char*);
				memcpy (str, s, strlen (s));
				str += strlen (s);
			}
			else if (*format == 'c')
			{
				char c = (char) va_arg (ap, int);
				*str++ = c;
			}
			else if (*format == '%')
				*str++ = '%';

			format++;
		}
		else
		  *str++ = ch;
	}

	*str++ = '\0';

	return 0;
}

int sprintf (char* str, const char* format, ...)
{
	va_list ap;
	va_start (ap, format);

	int result = vsprintf (str, format, ap);

	va_end (ap);

	return result;
}

int printf (const char* format, ...)
{
	va_list ap;
	va_start (ap, format);

	char buf [200];	
	int result = vsprintf (buf, format, ap);
	
	puts (buf);

	va_end (ap);

	return result;
}


