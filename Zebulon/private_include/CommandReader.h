#ifndef COMMANDREADER_H
#define COMMANDREADER_H

#include <string>
#include <list>

namespace Zebulon
{

class CommandReader
{
public:
	CommandReader();
	void loadHistory ();

	std::string read ();
 	std::string addHistoryItem (const std::string& item);

	void showHistory () const;
	void clearHistory ();

	void loadHistoryItem (size_t item);
private:
	void saveHistory () const;

	void delChar ();
	std::string getHistoryItem (size_t index) const;
	char m_buf [255];
	char* m_p;
	std::list <std::string> m_history;
	size_t m_pos;
};

}

#endif

