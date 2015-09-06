#include "AutoStatus.h"
#include "nocheatz.h"
#include "Cg_NCZ_Player.h"

#define STATUS_MAX_JOB_PER_FRAME 1

class NoCheatZ;
extern NoCheatZ g_NoCheatZPlugin;
extern IVEngineServer * engine;
extern CGlobalVars*               gpGlobals;

AutoStatus::AutoStatus(void) :
	isStarted(false),
	allow_status(true),
	ccl(1),
	stage(1),
	job_done(1)
{
}


AutoStatus::~AutoStatus(void)
{
}

void AutoStatus::AutoStatusStart()
{
	if(!isStarted && allow_status)
	{
		isStarted = true;
		ccl = 1;
		stage = 1;

		AutoStatusAllConTell("[NoCheatZ 3] %s :", GetTranslate("AutoStatus").c_str());
		AutoStatusAllConTell("--------------------------");
		AutoStatusAllConTell("uID  - NAME             [     Steam-ID     ] - %s      - K/D - rate / cmd / update / interp (ratio)", GetTranslate("Team").c_str());
	}
}

void AutoStatus::AutoStatusFrame()
{
	if(isStarted && allow_status)
	{
		Cg_NCZ_PlayerManager *pm = g_NoCheatZPlugin.get_m_pm();
		switch(stage)
		{
		case 1 :
			{
				MyInfo = NULL;
				MyPlayer = NULL;
				while(ccl != pm->getMaxClients())
				{
					if(ccl > 0)
					{
						MyPlayer = pm->getPlayer(ccl);
						if(MyPlayer)
						{
							MyInfo = MyPlayer->getPlayerInfo();
							if(MyInfo)
							{
								if(MyInfo->IsConnected())
								{
									if(MyPlayer->isRealPlayer())
									{
										const char * team = GetTranslate("Spectator").c_str();
										if (MyInfo->GetTeamIndex() == 2)
										{
											team = "Terrorist";
										}
										else if (MyInfo->GetTeamIndex() == 3)
										{
											team = "CT";
										}
										AutoStatusAllConTell(
											"%s - %s [%s] - %s - %d/%d - %s", 
											minStrf(tostring(MyPlayer->getUserid()), 4).c_str(),
											minStrf(MyPlayer->getName(), 16).c_str(),
											minStrf(MyPlayer->getSteamID(), 16).c_str(),
											minStrf(team, 9).c_str(),
											MyInfo->GetFragCount(),
											MyInfo->GetDeathCount(),
											getStrRates(ccl).c_str());
									
									}
								}
							}
						}
						++job_done;
					}
					else
					{
						ccl = 0;
					}
					++ccl;
					if(job_done > STATUS_MAX_JOB_PER_FRAME)
					{
						job_done = 1;
						break;
					}
				}

				if(ccl == pm->getMaxClients())
				{
					ccl = 1;
					++stage;
				}
			}
			break;

		case 2 :
			{
				MyEdict = getSourceTV();
				if(MyEdict)
				{
					MyPlayer = pm->getPlayer(MyEdict);
					AutoStatusAllConTell("%s - %s [     Source TV    ] -> TV Delay : %s s, TV Port : %s", minStrf(format("%d", MyPlayer->getUserid()), 4).c_str(), minStrf(MyPlayer->getName(), 16).c_str(), g_pCVar->FindVar("tv_delay")->GetString(), g_pCVar->FindVar("tv_port")->GetString());
				}
				AutoStatusAllConTell("--------------------------");
				AutoStatusAllConTell("Steam Community Links :");
				++stage;
			}
			break;

		case 3 :
			{
				MyPlayer = NULL;
				while(ccl != pm->getMaxClients())
				{
					if(ccl > 0)
					{
						MyPlayer = pm->getPlayer(ccl);
						if(MyPlayer)
						{
							if(MyPlayer->isRealPlayer())
							{
								AutoStatusAllConTell("%s -> %s", MyPlayer->getName(), getCommunityID(MyPlayer->getSteamID()).c_str());
							}
						}
						++job_done;
					}
					else
					{
						ccl = 0;
					}
					++ccl;
					if(job_done > STATUS_MAX_JOB_PER_FRAME)
					{
						job_done = 1;
						break;
					}
				}
				if(ccl == pm->getMaxClients())
				{
					this->End();
				}
			}
			break;

		default:
			ccl = 1;
			stage = 1;
			isStarted = false;
			break;
		}
	}
}

void AutoStatus::AutoStatusAllConTell(const char * fmt, ...)
{
	va_list		argptr;
	static char		string[512];

	va_start(argptr, fmt);
	Q_vsnprintf(string, sizeof(string), fmt, argptr);
	va_end(argptr);

	MyEdict = NULL;
	for (int i=1; i<=gpGlobals->maxClients; i++) // EntIndex 0 is worldspawn, after which come the players
	{
		MyEdict = PEntityOfEntIndex(i);
		if(MyEdict)
		{
			contell(MyEdict, string);
		}
	}
}

void AutoStatus::End()
{
	AutoStatusAllConTell("--------------------------");

	isStarted = false;
	ccl = 1;
	stage = 1;
	job_done = 1;
}
