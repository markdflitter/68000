#ifndef COMMANDREADER_INCLUDED
#define COMMANDREADER_INCLUDED

#include <string>
#include <list>


namespace Zebulon
{

class CommandReader
{
public:
	CommandReader();

	std::string read ();

	void showHistory () const;
	void clearHistory ();

	std::string getHistoryItem (size_t index) const;
private:
	void delChar (char* buf, char*& p);
 	std::string addHistoryItem (const std::string& item);

	std::list <std::string> m_history;
	size_t m_pos;
};

}

#endif

