#include <list>
#include <string>

#include "Preprocessors.h"
#include "Misc/Helpers.h"
#include "Misc/IFaceManager.h"
#include "Players/NczPlayer.h"
#include "Systems/BaseSystem.h"

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
		this->m_timestamp = 0.0;
		this->m_tick = 0;
		memset(&(this->m_dataStruct), 0, sizeof(playerDataStructT));
	};
	~SubDetection(){};

	void PrepareDetectionData(playerDataStructT* dataStruct)
	{
		this->m_timestamp = Plat_FloatTime();
		this->m_tick = CIFaceManager::GetInstance()->GetGlobals()->tickcount;
		memcpy(this->GetDataStruct(), dataStruct, sizeof(playerDataStructT));
	};

	playerDataStructT* GetDataStruct() const
	{
		return (playerDataStructT*)(&(this->m_dataStruct));
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
		this->m_playerName[0] = 0;
		this->m_playerAdress[0] = 0;
		this->m_playerSteamID[0] = 0;
		this->m_testerName = nullptr;
	};
	~LogDetection(){};

	virtual const char * GetDetectionLogMessage(){return nullptr;};

	virtual void Log()
	{
		std::string msg;
		msg = Helpers::format("%f %d - %s triggered a detection : %s [%s - %s] is using a %s.\0", this->m_timestamp, this->m_tick, this->m_testerName, this->m_playerName, this->m_playerSteamID, this->m_playerAdress, this->GetDetectionLogMessage()); 
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
			this->m_playerName[x] = player->GetName()[x];
		}
		this->m_playerName[32] = '\0'; 

		for(int x = 0; x < 32; ++x)
		{
			this->m_playerAdress[x] = player->GetIPAddress()[x];
		}
		this->m_playerAdress[32] = '\0';

		for(int x = 0; x < 32; ++x)
		{
			this->m_playerSteamID[x] = player->GetSteamID()[x];
		}
		this->m_playerAdress[32] = '\0';
	};

	virtual const char * GetDataDump(){return nullptr;};

protected:
	const char * m_testerName;
	char m_playerName[32];
	char m_playerAdress[32];
	char m_playerSteamID[32];
};

template <typename playerDataStructT>
class FalseDetection : public LogDetection<playerDataStructT>
{
	typedef LogDetection<playerDataStructT> BaseClass;
public:
	FalseDetection() : BaseClass()
	{
		this->m_playerName[0] = 0;
		this->m_playerAdress[0] = 0;
		this->m_playerSteamID[0] = 0;
		this->m_testerName = nullptr;
	};
	~FalseDetection(){};

	virtual const char * GetDetectionLogMessage(){return nullptr;};

	virtual void Log()
	{
		std::string msg;
		msg = Helpers::format("%f %d - %s triggered a false detection on %s [%s - %s].\0", this->m_timestamp, this->m_tick, this->m_testerName, this->m_playerName, this->m_playerSteamID, this->m_playerAdress); 
		Helpers::writeToLogfile(msg);
	};

	void PrepareDetectionLog(NczPlayer* player, BaseSystem* tester)
	{
		m_testerName = tester->GetName();
		
		for(int x = 0; x < 32; ++x)
		{
			this->m_playerName[x] = player->GetName()[x];
		}
		this->m_playerName[32] = '\0'; 

		for(int x = 0; x < 32; ++x)
		{
			this->m_playerAdress[x] = player->GetIPAddress()[x];
		}
		this->m_playerAdress[32] = '\0';

		for(int x = 0; x < 32; ++x)
		{
			this->m_playerSteamID[x] = player->GetSteamID()[x];
		}
		this->m_playerAdress[32] = '\0';
	};

	virtual const char * GetDataDump(){return nullptr;};

protected:
	const char * m_testerName;
	char m_playerName[32];
	char m_playerAdress[32];
	char m_playerSteamID[32];
};

#endif
