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

	void loadHistoryItem (size_t item);
private:
	void delChar ();
	std::string getHistoryItem (size_t index) const;
 	std::string addHistoryItem (const std::string& item);

	char m_buf [255];
	char* m_p;
	std::list <std::string> m_history;
	size_t m_pos;
};

}

#endif

