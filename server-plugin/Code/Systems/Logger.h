#include <list>
#include <string>

#include "Preprocessors.h"
#include "Misc/Helpers.h"
#include "Systems/BaseSystem.h"
#include "Players/NczPlayer.h"

#ifndef LOGGER_H
#define LOGGER_H

typedef enum LogLevel
{
	OFF = 0,
	LOG_VERBOSE,
	LOG_HINT,
	LOG_DETECTION
} LogLevelT;

class Logger
{
public:
	Logger() {};
	~Logger(){};

	static void xMsg(LogLevelT loglevel, const char * fmt, ...);

	static void DumpLog(LogLevelT dumplevel = LOG_HINT);

	virtual void SetLogLevel(LogLevelT level){m_loglevel = level;};

private:
	LogLevelT m_loglevel;
	static FILE* m_logfile;
	static std::list<std::string> m_messages;
};

#endif
