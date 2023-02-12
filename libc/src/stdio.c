#include "../include/stdio.h"
#include "../include/string.h"
#include <ZebulonAPI.h>

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

void bin2hex (unsigned int num, char* buf)
{
	const char* LUT = "0123456789ABCDEF";
	if (num < 16)
	{
		buf [0] = LUT [(num & 0xF0) >> 4];
		buf [1] = LUT [num & 0xF];
		buf [2] = '\0';

		return ;
	}

	unsigned int w = num;
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

bool copyStr (char*& dst, const char* src, size_t& n)
{ 
	size_t count = strlen (src);
	if (count > n) count = n;

	memcpy (dst, src, count);

	dst += count;
	n -= count;

	return n ==0;
}

int vsprintfn (char* str, size_t n, const char* format, va_list ap)
{
	n--;			// leave space for '0';

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
				if (copyStr (str, buf, n)) break;
			}
			else if (*format == 'd')
			{
				unsigned int i = va_arg (ap, unsigned int);
				char buf [11];
				bin2dec (i, buf);
				if (copyStr (str, buf, n)) break;
			}
			else if (*format == 's')
			{
				char* s = va_arg (ap, char*);
				if (copyStr (str, s, n)) break;
			}
			else if (*format == 'c')
			{
				char c = (char) va_arg (ap, int);
				*str++ = c;
				if (--n == 0) break;
			}
			else if (*format == '%')
			{
				*str++ = '%';
				if (--n == 0) break;
			}
			format++;
		}
		else
		{
		  *str++ = ch;
			if (--n == 0) break;
		}
	}

	*str++ = '\0';

	return 0;
}

int sprintf (char* str, const char* format, ...)
{
	va_list ap;
	va_start (ap, format);

	int result = vsprintfn (str, -1, format, ap);

	va_end (ap);

	return result;
}

int sprintfn (char* str, size_t n, const char* format, ...)
{
	va_list ap;
	va_start (ap, format);

	int result = vsprintfn (str, n, format, ap);

	va_end (ap);

	return result;
}


int printf (const char* format, ...)
{
	va_list ap;
	va_start (ap, format);

	size_t bufferSizeGuess = 2 * strlen (format);
	if (bufferSizeGuess < 1024) bufferSizeGuess = 1024;

	char buf [bufferSizeGuess];	
	int result = vsprintfn (buf, bufferSizeGuess, format, ap);
	
	puts (buf);

	va_end (ap);

	return result;
}


FILE* fopen (const char* filename, const char* mode)
{
	return (FILE*) Zebulon::_zebulon_fopen (filename, mode);
}

int feof (FILE* fptr)
{
	return Zebulon::_zebulon_feof ((unsigned int) fptr);
}

long unsigned int fwrite (const void* data, long unsigned int data_size, long unsigned int number_data, void* fptr)
{
	return Zebulon::_zebulon_fwrite (data, data_size, number_data, (unsigned int) fptr);
}

long unsigned int fread (const void* data, long unsigned int data_size, long unsigned int number_data, void* fptr)
{
	return Zebulon::_zebulon_fread (data, data_size, number_data, (unsigned int) fptr);
}

void fclose (FILE* fptr)
{
	return Zebulon::_zebulon_fclose ((unsigned int) fptr);
}
