#ifndef UTILS_H
#define UTILS_H

#include <bsp.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <ZebulonAPI.h>

namespace Zebulon
{

class Utils
{
public:
	static void printIdeError (::ide_result error)
	{
		if (error == ::IDE_AMNF)
			printf (">>>  address mark not found\n\r");
		if (error == ::IDE_TK0NF)
			printf (">>>  track 0 not found\n\r");
		if (error == ::IDE_ABRT)
			printf (">>>  aborted command");
		if (error == ::IDE_MCR)
			printf (">>>  media change requested\n\r");
		if (error == ::IDE_IDNF)
			printf (">>>  ID not found\n\r");
		if (error == ::IDE_MC)
			printf (">>>  media change\n\r");
		if (error == ::IDE_UNC)
			printf (">>>  uncorrectable data error\n\r");
		if (error == ::IDE_BBK)
			printf (">>>  bad block\n\r");
	}

	static std::string pad (const std::string& str, char c, size_t length)
	{
		std::string padding (length - str.length (), c);	
		return str + padding;
	}

	static std::string padFilename (const std::string& filename)
	{
		return pad (filename, ' ', MAX_FILENAME_LENGTH);
	}

	static std::vector<std::string> tokenize (const std::string& command)
	{
		std::vector<std::string> result;

		const char* str = command.c_str ();
		char buf [255];
		char* cur = buf;
		while (*str != '\0')
		{
			if (*str != ' ')
				*cur++ = *str;
			else
			{
				*cur = '\0';
				result.push_back (std::string (buf));
				cur = buf;
			}
			str++;
		}

		if (cur != buf)
		{
			*cur = '\0';
			result.push_back (std::string (buf));
			cur = buf;
		}

		return result;
	}
};

}

#endif

