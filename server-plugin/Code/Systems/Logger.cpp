#include "Logger.h"

void Logger::xMsg(LogLevelT loglevel, const char * fmt, ...)
{
	va_list		argptr;
	static char		string[4096];

	va_start(argptr, fmt);
	vsnprintf(string, sizeof(string), fmt, argptr);
	va_end (argptr);

	std::string finalString = Helpers::tostring("[" NCZ_PLUGIN_NAME "] ");
	if(loglevel == LOG_VERBOSE || loglevel == LOG_DETECTION) finalString.append(Helpers::format("%f %d ", Plat_FloatTime(), CIFaceManager::GetInstance()->GetGlobals()->tickcount));
	if(loglevel == LOG_HINT || loglevel == LOG_DETECTION) finalString.append(Helpers::getStrDateTime("%x %X "));
	finalString.append("- ");
	finalString.append(Helpers::tostring(string));

	if(loglevel == OFF) return;

	CIFaceManager::GetInstance()->GetIengine()->LogPrint(finalString.c_str());
}