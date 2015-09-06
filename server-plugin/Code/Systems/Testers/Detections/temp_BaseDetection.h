#include <list>
#include <string>
//#include <mutex>

#include "Preprocessors.h"
#include "Misc/temp_BaseQueue.h"
#include "Misc/Helpers.h"
#include "Misc/IFaceManager.h"
#include "Players/NczPlayer.h"
#include "Systems/BaseSystem.h"
#include "Systems/Pipeline/Pipeline.h"

#ifndef BASEDETECTION
#define BASEDETECTION

class BaseDetection
{
public:
	BaseDetection(){};
	virtual ~BaseDetection(){};
};

template <typename playerDataStructT>
class SubDetection : public BaseDetection
{
public:
	SubDetection()
	{
		m_timestamp = 0.0;
		m_tick = 0;
		memset(&m_dataStruct, 0, sizeof(playerDataStructT));
	};
	~SubDetection(){};

	void PrepareDetectionData(playerDataStructT* dataStruct)
	{
		m_timestamp = Plat_FloatTime();
		m_tick = CIFaceManager::GetInstance()->GetGlobals()->tickcount;
		memcpy(GetDataStruct(), dataStruct, sizeof(playerDataStructT));
	};

	playerDataStructT* GetDataStruct() const
	{
		return (playerDataStructT*)(&m_dataStruct);
	};

	virtual const char * GetDataDump(){return nullptr;};

protected:
	double m_timestamp;
	int m_tick;
	playerDataStructT m_dataStruct;
};

template <typename playerDataStructT>
class LogDetection : public SubDetection<playerDataStructT>
{
	typedef SubDetection<playerDataStructT> BaseClass;
public:
	LogDetection() : BaseClass()
	{
		m_playerName[0] = 0;
		m_playerAdress[0] = 0;
		m_playerSteamID[0] = 0;
		m_testerName = nullptr;
	};
	~LogDetection(){};

	virtual const char * GetDetectionLogMessage(){return nullptr;};

	virtual void Log()
	{
		std::string msg;
		msg = Helpers::format("%f %d - %s triggered a detection : %s [%s - %s] is using a %s.\0", m_timestamp, m_tick, m_testerName, m_playerName, m_playerSteamID, m_playerAdress, GetDetectionLogMessage()); 
		Helpers::writeToLogfile(msg);
		msg = Helpers::format("[" NCZ_PLUGIN_NAME "] %s\0", msg.c_str());
		CIFaceManager::GetInstance()->GetIengine()->LogPrint(msg.c_str());
		if(NczPlayerManager::GetInstance()->GetPlayerHandlerBySteamID(m_playerSteamID)->status != INVALID)
		{
			Helpers::noTell(NczPlayerManager::GetInstance()->GetPlayerHandlerBySteamID(m_playerSteamID)->playerClass->GetEdict(), msg);
		}
		else
		{
			Helpers::chatmsg(msg);
		}
	};

	void PrepareDetectionLog(NczPlayer* player, BaseSystem* tester)
	{
		m_testerName = tester->GetName();
		
		for(int x = 0; x < 32; ++x)
		{
			m_playerName[x] = player->GetName()[x];
		}
		m_playerName[32] = '\0'; 

		for(int x = 0; x < 32; ++x)
		{
			m_playerAdress[x] = player->GetIPAddress()[x];
		}
		m_playerAdress[32] = '\0';

		for(int x = 0; x < 32; ++x)
		{
			m_playerSteamID[x] = player->GetSteamID()[x];
		}
		m_playerAdress[32] = '\0';
	};

	virtual const char * GetDataDump(){return nullptr;};

protected:
	const char * m_testerName;
	char m_playerName[32];
	char m_playerAdress[32];
	char m_playerSteamID[32];

#ifdef GNUC // Why do I use protected ?
	double m_timestamp;
	int m_tick;
	playerDataStructT m_dataStruct;
#endif
};

template <typename playerDataStructT>
class FalseDetection : public LogDetection<playerDataStructT>
{
	typedef LogDetection<playerDataStructT> BaseClass;
public:
	LogDetection() : BaseClass()
	{
		m_playerName[0] = 0;
		m_playerAdress[0] = 0;
		m_playerSteamID[0] = 0;
		m_testerName = nullptr;
	};
	~LogDetection(){};

	virtual const char * GetDetectionLogMessage(){return nullptr;};

	virtual void Log()
	{
		std::string msg;
		msg = Helpers::format("%f %d - %s triggered a false detection on %s [%s - %s].\0", m_timestamp, m_tick, m_testerName, m_playerName, m_playerSteamID, m_playerAdress); 
		Helpers::writeToLogfile(msg);
	};

	void PrepareDetectionLog(NczPlayer* player, BaseSystem* tester)
	{
		m_testerName = tester->GetName();
		
		for(int x = 0; x < 32; ++x)
		{
			m_playerName[x] = player->GetName()[x];
		}
		m_playerName[32] = '\0'; 

		for(int x = 0; x < 32; ++x)
		{
			m_playerAdress[x] = player->GetIPAddress()[x];
		}
		m_playerAdress[32] = '\0';

		for(int x = 0; x < 32; ++x)
		{
			m_playerSteamID[x] = player->GetSteamID()[x];
		}
		m_playerAdress[32] = '\0';
	};

	virtual const char * GetDataDump(){return nullptr;};

protected:
	const char * m_testerName;
	char m_playerName[32];
	char m_playerAdress[32];
	char m_playerSteamID[32];

#ifdef GNUC // Why do I use protected ?
	double m_timestamp;
	int m_tick;
	playerDataStructT m_dataStruct;
#endif
};

template <typename playerDataStructT>
class ReportDetection : public LogDetection<playerDataStructT>
{
	typedef LogDetection<playerDataStructT> BaseClass;
public:
	ReportDetection() : BaseClass() {};
	~ReportDetection(){};

	static const char * GetReportFirstLine()
	{
		std::string line;
		line += NCZ_PLUGIN_NAME;
		line += " (v";
		line += NCZ_VERSION_STR;
		line += ") - ";
		line += CIFaceManager::GetInstance()->GetGlobals()->mapname;
		line += " (";
		line += CIFaceManager::GetInstance()->GetGlobals()->mapversion;
		line += ") - ";
		line += g_pCVar->FindVar("hostname")->GetString();
		line += "(version ";
		line += CIFaceManager::GetInstance()->GetIengine()->GetServerVersion();
		line += ") (";
		line += g_pCVar->GetCommandLineValue("ip");
		line += ":";
		line += g_pCVar->FindVar("hostport")->GetInt();
		line += ") - ";
		line += Helpers::getStrDateTime("%x %X");
	};

	virtual const char * GetDetectionLogMessage(){return nullptr;};

	virtual const char * GetDataDump(){return nullptr;};

	void Report()
	{
		JobInfoT job = {REPORT, this};
		Pipeline::GetInstance()->Job.Push(job);
	};

#ifdef GNUC
	private:
	const char * m_testerName;
	char m_playerName[32];
	char m_playerAdress[32];
	char m_playerSteamID[32];

	double m_timestamp;
	int m_tick;
	playerDataStructT m_dataStruct;
#endif
};

#endif
