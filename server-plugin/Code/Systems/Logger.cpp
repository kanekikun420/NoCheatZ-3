#include "Logger.h"
#include "Misc/Helpers.h"

Logger ILogger;

void Logger::Push(const std::string& msg)
{
	m_msg.insert(m_msg.end(), msg);
}

void Logger::Flush()
{
	if(m_msg.empty()) return;
	if(CIFaceManager::GetInstance()->GetIengine())
	{
		std::string path = Helpers::getStrGameDir() + "/logs/NoCheatZ_4_Logs/" + Helpers::getStrDateTime("NoCheatZ-%d-%b-%Y") + ".log";
		std::ofstream fichier(path.c_str(), std::ios::out | std::ios::app);
		if(fichier)
		{
			std::list<std::string>::const_iterator it = m_msg.cbegin();
			do
			{
				fichier << *it;
			}
			while(it != m_msg.cend());
			m_msg.clear();
		}
		else
		{
			Msg("[NoCheatZ 4] Can't write to logfile at %s ... Please check write access and if the directory exists.\n", path.c_str());
			CIFaceManager::GetInstance()->GetIengine()->LogPrint(Helpers::format("[NoCheatZ 4] Can't write to logfile at %s ... Please check write access and if the directory exists.\n", path.c_str()).c_str());
		}
	}
}

void Helpers::writeToLogfile(const std::string &text)
{
	ILogger.Push(text);
}
