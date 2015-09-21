#include <list>
#include <string>
#include <fstream>

#include "Preprocessors.h"
#include "Misc/Helpers.h"

#ifndef LOGGER_H
#define LOGGER_H


/*
	Messages to be written on the plugin's logfile.

	To prevent the game to hang on write, the flush is done only when we are outside a round.

	This class should be threadsafe.
*/

class Logger
{
public:
	Logger() {};
	~Logger(){};

	void Push(const std::string& msg);
	void Flush();

private:
	std::list<std::string> m_msg;
};

extern Logger ILogger;

#endif
