//===== Copyright 2011, DJ Fire-Black production, All rights reserved. ======//
//
// Purpose:
//
// $NoKeywords: $
//
//===========================================================================//

/*
TODO list :
Finir le nouveau détecteur
2 joueurs minimum avant de lancer la record auto
Fix traductions
Ajouter traductions
Pouvoir envoyer la tv si détection

2 joueurs minimum avant de lancer la record auto.
Nouveau détecteur d'aimbot.
Changement de l'extension des fichiers logs en .log
Correction du temps de connexion des joueurs (Mauvaise initialisation d'une variable)
*/


#include <vector> 
#include <sstream>
#include <string>
#include <iostream>
#include <stdio.h>
#include <exception>
#include <fstream>
#include <sstream>
#include <cctype>
#include <stdexcept>
#include <map>
#include <exception>
#include <list>
#include <time.h>
#include "Preprocessors.h"
#include "MRecipientFilter.h"
#include "bitbuf.h"
#include "nocheatz.h"
#include "interface.h"
#include "engine/iserverplugin.h"
#include "eiface.h"
#include "igameevents.h"
#include "convar.h"
#include "tier2/tier2.h"
#ifndef NCZ_EP1
#include "game/server/iplayerinfo.h"
#else
#include "dlls/iplayerinfo.h"
#endif
#include "server_class.h"
#if !defined(NCZ_CSGO) && !defined(NCZ_CSP)
#include "EntityProps.h"
#endif
#include "AutoStatus.h"
#include "NczHttpSockHandler.h"
#include "NczThread.h"
#include "Cg_NCZ_Player.h"
#include "iservernetworkable.h"

//#include "baseentity.h"

#ifdef WIN32
#include <tchar.h>
#else
#include <wchar.h>
#endif
#include "inetchannelinfo.h"
#ifdef WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32")
typedef int socklen_t;
WSADATA wsaData; // structure needed to initialize a WinSock library
HOSTENT* host;         // structure which is used to store information about the host
IN_ADDR addr;         // this structure contains the g
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket(s) close(s)
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
in_addr addr;
hostent* host;
#endif

// memdbgon must be the last include file in a .cpp file!!!
//#include "tier0/memdbgon.h"

#undef fopen
//#define GAME_DLL

NoCheatZ g_NoCheatZPlugin;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(NoCheatZ, IServerPluginCallbacks, INTERFACEVERSION_ISERVERPLUGINCALLBACKS, g_NoCheatZPlugin );


bool                       norestart                = false;
bool                       check_client_vars        =  true;
bool                       check_aimbots            =  true;
bool                       aimbots_extra_filter     = false;
bool                       check_bunny              =  true;
bool                       check_triggerbot         =  true;
bool                       report_cheats            =  true;
bool                       kick_ban                 = false;
bool                       auto_dem                 = false;
bool                       use_banlists             = true;
bool                       use_ncz_banlists         =  true;
bool                       use_lgz_banlists         =  true;
bool                       use_eac_banlists         =  true;
bool                       use_esl_banlists         =  true;
bool                       use_esea_banlists         =  true;
bool                       recording                = false;
bool                       ncz_active               =  true;
bool                       cvarCheckLaunched        = false;
//bool                       web_thread_killed        = false;
bool                       report_ukcmd             =  true;
bool                       test_config              =  true;
bool                       registered               = false;
//bool                       unregister_asked         = false;
//bool                       needtokill               = false;
bool                       ForceConsistency         = false;
bool                       NoTell_active            =  true;
bool                       isRoundPlaying           = false;
bool                       must_thread_be_killed    = false;
bool                       UseNet                   =  true;
bool                       FirstGameFrame           =  true;
bool                       teleport_lock            =  true;
bool                       lockServer               = false;
bool                       requireRestart           = false;
bool                       maySendHeartbeat         = true;
bool                       firstMapOfGame           = true;
#ifdef NCZ_CSS
int                        aimbot_system_number     = 1;
#else
int                        aimbot_system_number     = 1;
#endif
int                        HeartbeatRemain          = 0;
//bool                       allow_status             =  true;

//int                        vax_offset               =     0;
//int                        vay_offset               =     0;
//int                        ground_offset            =     0;
//int                        player_count             =     0;
int                        t_tickrate               =     0; 
int                        tickrate                 =     0;
//int                        maxplayers               =     0;
int                        frameStage               =     0;
int                        frameStage_PVAC          =     0;

int               demoticks                =     0;
//int               n_tick                   =     0;

float                      nextSecond               =   0.0f;
//float                      lastTickrateCheck        =   0.0;
//float                      nextTickrateCheck        =   0.0;
float                      UkCmdCheck               =   0.0f;
float                      nextstopcheck            =   0.0f;
float mapstart = 0.0f;
#define GET_MAPTIME (GetTime()-mapstart)

//float                      next_cvar_check          =  0.0f;
//float                      next_cvar_check_timer    = 20.0f;
//float                      roundPlayingCheck        =  0.0f;
float                      lastNetOff               =  0.0f;
int                        disconnectNumber         =     0;

std::string                recordFilename           =    "";
std::string                gb_admin_id              =    "";
std::string                gb_reason_id             =    "";
std::string                our_ip                   =    "";

Cg_NCZ_Queue*              queue_send_report        =  NULL;
Cg_NCZ_Queue*              banlists                 =  NULL;
Cg_NCZ_Queue*              disconnectingPlayers     =  NULL;
Cg_NCZ_Queue*              serverMessages           =  NULL;
//Cg_NCZ_Queue*              player_conf              =  NULL;

CGlobalVars*               gpGlobals                =  NULL;

std::map<std::string, std::string> g_translate;

edict_t *                  PlayerList               =  NULL;

IVEngineServer *           engine                   =  NULL;
IGameEventManager2 *       gameeventmanager         =  NULL;
IPlayerInfoManager *       playerinfomanager        =  NULL;
IServerPluginHelpers *     helpers                  =  NULL;
IServerGameDLL *           gamedll                  =  NULL;
IServerGameEnts *          gameents                 =  NULL;
IServerGameClients *       gameclients              =  NULL;
#if !defined(NCZ_CSGO) && !defined(NCZ_CSP)
EntityPropsManager *       entpropsmanager          =  NULL;
#endif
IEngineTrace *             enginetrace              =  NULL;

NczThread *TheWebThread = NULL;
//NczThread *UpdateThread = NULL;

#ifdef NCZ_EP1
static ICvar *g_pCVar;
#endif

class CCSPlayer;

#include "usercmd.h"
#ifdef WIN32
#include <Windows.h>
#else
#include <signal.h>
#include <sys/mman.h>
#include <assert.h>
#include <errno.h>
#endif

#ifdef WIN32
typedef int (__stdcall *ShouldTransmit_t)(const CCheckTransmitInfo *);
typedef int (__stdcall *UpdateTransmitState_t)();
#else
typedef int (*ShouldTransmit_t)(CCSPlayer *, const CCheckTransmitInfo *);
typedef int (*UpdateTransmitState_t)(CCSPlayer *);
#endif

#ifdef WIN32
typedef void (__stdcall *PlayerRunCommand_t)(CUserCmd*, IMoveHelper*);
#else
typedef void (*PlayerRunCommand_t)(CCSPlayer *, CUserCmd*, IMoveHelper*);
#endif

void xMsg(const char *fmt, ...);

/*
inline DWORD GetVFuncAddr( DWORD* classptr, int vtable)
{
	VirtualProtect( &classptr[vtable], sizeof(DWORD), PAGE_EXECUTE_READWRITE, NULL );
	return classptr[vtable];
}*/
bool hasBeenHooked = false;

ConvarList g_cvars;

DWORD VirtualTableHook( DWORD* classptr, int vtable, DWORD newInterface )
{
		DWORD dwOld, dwStor = 0x0;
		if(!(classptr || vtable || newInterface)) return NULL;
	#ifdef WIN32
		if(!VirtualProtect(&classptr[vtable], (vtable * sizeof(void *)) + 4, PAGE_EXECUTE_READWRITE, &dwOld ))
		{
			xMsg("VirtualProtect: GetLastError() -> %X", GetLastError());
			xMsg("Unable to use anti-aimbot system level 2 (Hook failed). Switching to system level 1.");
			aimbot_system_number = 1;
			return NULL;
		}
	#else

        DWORD psize = sysconf(_SC_PAGESIZE);
		void *p = (void *)((DWORD)(&classptr[vtable]) & ~(psize-1));
		if(mprotect(p, ((vtable * sizeof(void *)) + ((DWORD)(&classptr[vtable]) & (psize-1))), PROT_READ | PROT_WRITE | PROT_EXEC ) < 0)
		{
			xMsg("mprotect: errno -> %d", errno);
			xMsg("Unable to use anti-aimbot system level 2 (Hook failed). Switching to system level 1.");
			aimbot_system_number = 1;
			return NULL;
		}
	#endif
		dwStor = classptr[vtable];
	#ifdef WIN32
		*(DWORD*)&(classptr[vtable]) = newInterface;
		VirtualProtect(&classptr[vtable], (vtable * sizeof(void *)) + 4, dwOld, &dwOld);
	#else
		*(DWORD*)&(classptr[vtable]) = newInterface;
		mprotect(p, ((vtable * sizeof(void *)) + ((DWORD)(&classptr[vtable]) & (psize-1))), PROT_READ | PROT_EXEC );
	#endif
		return dwStor;
}

PlayerRunCommand_t gpPlayerRunCommand = NULL;
ShouldTransmit_t gpShouldTransmit = NULL;
UpdateTransmitState_t gpUpdateTransmitState = NULL;
DWORD* pdwNewInterface = NULL;


void UnhookCmd()
{
#ifdef NCZ_CSS
	if(pdwNewInterface && gpPlayerRunCommand && hasBeenHooked)
	{
		VirtualTableHook(pdwNewInterface, VTABLE_CMD, (DWORD)gpPlayerRunCommand);
		//VirtualTableHook(pdwNewInterface, VTABLE_SHOULD_TRANSMIT, (DWORD)gpShouldTransmit);
		//VirtualTableHook(pdwNewInterface, VTABLE_UPDATE_TRANSMIT, (DWORD)gpUpdateTransmitState);
		pdwNewInterface = NULL;
		gpPlayerRunCommand = NULL;
		hasBeenHooked = false;
	}
#endif
}

inline float GetTime()
{
#ifdef NCZ_CSGO
	if(gpGlobals) return gpGlobals->realtime;
	else return 0.0f;
#else
	if(engine) return engine->Time();
	else return 0.0f;
#endif
}

#ifdef WIN32

void __stdcall nPlayerRunCommand(CUserCmd* pCmd, IMoveHelper* pMoveHelper)
{

	__asm pushad;

	CCSPlayer * thisptr = NULL;
	if(pCmd/* && pMoveHelper*/)
	{
		__asm mov thisptr, ecx;

		bool allowThisCmd = true;
		if(thisptr && check_aimbots && !teleport_lock && (aimbot_system_number > 1))
		{
			__asm pushad;
			//Msg("PLAYERCOMMAND\n");
			Cg_NCZ_Player * MyPlayer = g_NoCheatZPlugin.get_m_pm()->getPlayer(reinterpret_cast<void *>(thisptr));
			if(MyPlayer)
			{
				__asm pushad;
				//Msg("RealPlayerInfex : %d\n", MyPlayer->getIndex());
				CUserCmd MyCmd = *pCmd;
				MyPlayer->testEyeAngles(&MyCmd);
				//printf("Seed : %d\n", pCmd->random_seed);
				//pCmd->random_seed = 0;
				if(fabs(pCmd->viewangles.x) > 89.0f || fabs(pCmd->viewangles.y) > 180.0f || fabs(pCmd->viewangles.z) > 89.0f)
				{
					
					allowThisCmd = false;
					//Msg("[NoCheatZ 3] Blocking bad command from %s [%s] (bad eyeangles).\n", MyPlayer->getName(), MyPlayer->getSteamID());
				}
				if(pCmd->hasbeenpredicted)
				{
					__asm pushad;
					allowThisCmd = false;
					xMsg("[NoCheatZ 3] Blocking bad command from %s [%s] (fake prediction).\n", MyPlayer->getName(), MyPlayer->getSteamID());
				}
				__asm popad;
			}
		}
		if(!allowThisCmd) return;
	}
	//__asm mov ecx, thisptr;
	__asm popad;

	gpPlayerRunCommand(pCmd, pMoveHelper);
}
#undef GetClassName

int __stdcall nUpdateTransmitState()
{
	__asm pushad;

	CCSPlayer * thisptr = NULL;
	__asm mov thisptr, ecx;

	if(thisptr)
	{
		Cg_NCZ_Player * MyPlayer = g_NoCheatZPlugin.get_m_pm()->getPlayer(reinterpret_cast<void *>(thisptr));
		if(MyPlayer)
		{
			edict_t * MyEdict = MyPlayer->getEdict();
			if(MyEdict)
			{
				MyEdict->ClearTransmitState();
				int oldFlags = MyEdict->m_fStateFlags;
				MyEdict->m_fStateFlags = FL_EDICT_FULLCHECK;
				if ( (oldFlags & FL_EDICT_DONTSEND) != (MyEdict->m_fStateFlags & FL_EDICT_DONTSEND) ) engine->NotifyEdictFlagsChange(MyPlayer->getIndex());
			}
		}
	}

	__asm mov ecx, thisptr;
	__asm popad;
}

int __stdcall nShouldTransmit(const CCheckTransmitInfo *pInfo)
{
	__asm pushad;

	CCSPlayer * thisptr = NULL;
	__asm mov thisptr, ecx;


	__asm mov ecx, thisptr;
	__asm popad;

	gpShouldTransmit(pInfo);
}



#ifdef SETDEVV
void __stdcall nSetTransmit(CCheckTransmitInfo *pInfo, bool bAlways)
{
	__asm pushad;
	CCSPlayer * thisptr = NULL;
	__asm mov thisptr, ecx;
	bool allowThisCmd = true;
	if(thisptr)
	{
		/*for(int bit = 1; bit < pInfo->m_pTransmitEdict->GetNumBits(); ++bit)
		{
			if(pInfo->m_pTransmitEdict->IsBitSet(bit)) // Is already marked for transmition ?
			{
				edict_t *pEntity = PEntityOfEntIndex(bit);
				//CCSPlayer *nptr = reinterpret_cast<CCSPlayer *>(pEntity->GetUnknown()->GetBaseEntity());
				//if(thisptr == nptr)
				//{
				//	Msg("MyPlayerIndex : %d\n", bit);
				//	continue;
				//}
				if(strcmp(pEntity->GetClassName(), "player") == 0)
				{
					Msg("Transmission between %x and %s (%x) %d killed.\n", thisptr, pEntity->GetClassName(), pEntity, bit);
					pInfo->m_pTransmitEdict->Set(bit, 0);
					//return;
				//Msg("%s\n", pEntity->GetClassName());
				}
				if(std::string(pEntity->GetClassName()).find("weapon") != std::string::npos)
				{
					Msg("Transmission between %x and %s (%x) %d killed.\n", thisptr, pEntity->GetClassName(), pEntity, bit);
					pInfo->m_pTransmitEdict->Set(bit, 0);
					//return;
				}
				
			}
		}*/
		CBitVecT<CFixedBitVecBase<2048>> * myInfo = new CBitVec<2048>;
		pInfo->m_pTransmitEdict->SetAll();
		pInfo->m_pTransmitEdict->Not(myInfo);
	}
	//return FL_EDICT_DONTSEND;
	__asm popad;

	gpSetTransmit(pInfo, true);
}
#endif
#else
void nPlayerRunCommand(CCSPlayer * thisptr, CUserCmd* pCmd, IMoveHelper* pMoveHelper)
{
	__asm ("pusha");

		bool allowThisCmd = true;
		if(thisptr && check_aimbots && !teleport_lock && (aimbot_system_number > 3))
		{
			Cg_NCZ_Player * MyPlayer = g_NoCheatZPlugin.get_m_pm()->getPlayer(reinterpret_cast<void *>(thisptr));
			if(MyPlayer)
			{
				//Msg("RealPlayerInfex : %d\n", MyPlayer->getIndex());
				CUserCmd MyCmd = *pCmd;
				MyPlayer->testEyeAngles(&MyCmd);
				//printf("Seed : %d\n", pCmd->random_seed);
				//pCmd->random_seed = 0;
				if(fabs(pCmd->viewangles.x) > 89.0f || fabs(pCmd->viewangles.y) > 180.0f || fabs(pCmd->viewangles.z) > 89.0f)
				{
					allowThisCmd = false;
					//Msg("[NoCheatZ 3] Blocking bad command from %s [%s] (bad eyeangles).\n", MyPlayer->getName(), MyPlayer->getSteamID());
				}
				if(pCmd->hasbeenpredicted)
				{
					allowThisCmd = false;
					xMsg("[NoCheatZ 3] Blocking bad command from %s [%s] (fake prediction).\n", MyPlayer->getName(), MyPlayer->getSteamID());
				}
			}
		}
		if(!allowThisCmd) return;
	__asm ("popa");

	gpPlayerRunCommand(thisptr, pCmd, pMoveHelper);
}
#endif

#ifdef NCZ_EP1
class CPluginConVarAccessor : public IConCommandBaseAccessor
{
public:
	virtual bool	RegisterConCommandBase( ConCommandBase *pCommand )
	{
		pCommand->AddFlags( FCVAR_PLUGIN );

		// Unlink from plugin only list
		pCommand->SetNext( 0 );

		// Link to engine's list instead
		g_pCVar->RegisterConCommandBase( pCommand );
		return true;
	}

};

CPluginConVarAccessor g_ConVarAccessor;
#endif

int IndexOfEdict(const edict_t *pEdict)
{
#ifdef NCZ_CSGO
	return (int)(pEdict - gpGlobals->pEdicts);
#else
	if(engine) return engine->IndexOfEdict(pEdict);
	else return 0;
#endif
}

edict_t *PEntityOfEntIndex(int iEntIndex)  // Using inline here does make crazy dlopen ... why ?
{
#if defined(NCZ_CSGO)
	if (iEntIndex >= 0 && iEntIndex < gpGlobals->maxEntities)
	{
		return (edict_t *)(gpGlobals->pEdicts + iEntIndex);
	}
	return NULL;
#else
	if(engine) return engine->PEntityOfEntIndex(iEntIndex);
	else return NULL;
#endif
}

void HookCmd(edict_t * pEntity)
{
#ifdef NCZ_CSS
	if(!pEntity || !isValidEdict(pEntity))
	{
		for(int cl = 1; cl <= gpGlobals->maxClients; ++cl)
		{
			edict_t *cEntity = PEntityOfEntIndex(cl);
			if(isValidEdict(cEntity))
			{
				IPlayerInfo *pInfo = playerinfomanager->GetPlayerInfo(cEntity);
				if(pInfo)
				{
					if(pInfo->IsConnected() && pInfo->GetTeamIndex() > 0 && !pInfo->IsFakeClient())
					{
						pEntity = cEntity;
						break;
					}
				}
			}
		}
		if(!pEntity || !isValidEdict(pEntity))
		{
			//Msg("Unable to hook now.\n");
			return;
		}
	}
	if(aimbot_system_number < 2) return;
	CCSPlayer *BasePlayer = reinterpret_cast<CCSPlayer *>(pEntity->GetUnknown()->GetBaseEntity());

	//Msg("Trying to hook CCSPlayer::PlayerRunCommand ...\n");
	//Msg("pEntity : %X\n", pEntity);
	//Msg("Unknown : %X\n", pEntity->GetUnknown());
	//Msg("BaseEntity : %X\n", pEntity->GetUnknown()->GetBaseEntity());
	if(BasePlayer && ((DWORD)pEntity->GetUnknown() == (DWORD)pEntity->GetUnknown()->GetBaseEntity()))
	{
		if(!hasBeenHooked)
		{
			pdwNewInterface = ( DWORD* )*( DWORD* )BasePlayer;
			//Msg("pdwNewInterface : %X\n", pdwNewInterface);
			DWORD resp = VirtualTableHook( pdwNewInterface, VTABLE_CMD, ( DWORD )nPlayerRunCommand );
			//DWORD respb = VirtualTableHook( pdwNewInterface, VTABLE_SHOULD_TRANSMIT, ( DWORD )nShouldTransmit );
			//DWORD respc = VirtualTableHook( pdwNewInterface, VTABLE_UPDATE_TRANSMIT, ( DWORD )nUpdateTransmitState );
			//Msg("resp : %X\n", resp);
			if(resp)
			{
				*(DWORD*)&(gpPlayerRunCommand) = resp;
				hasBeenHooked = true;
				//Msg("Hook done.\n");
			}
		/*	if(respb && respc)
			{
				*(DWORD*)&(gpShouldTransmit) = respb;
				*(DWORD*)&(gpUpdateTransmitState) = respc;
			}*/
			else pdwNewInterface = NULL;
#ifdef SETDEVV
			*(DWORD*)&(gpSetTransmit) = VirtualTableHook( pdwNewInterface, VTABLE_TRANSMIT, ( DWORD )nSetTransmit );
#endif
		}
	}
#endif
}

class CBaseEntity;

ConVar nocheatz_version("nocheatz_version", NCZ_PLUGIN_NAME" v"NCZ_VERSION_STR, FCVAR_NOTIFY, "Version of NoCheatZ plugin that the server is actually using.\nNoCheatZ 3 : Better game with better security.\nOfficial Website : http://www.nocheatz.com/");
//ConVar ncz_serverhandleid("ncz_serverhandleid", "Unregistered", 0, "Used to register your server on your NoCheatZ account.");
//ConVar nocheatz_version("nocheatz_version", NCZ_VERSION_STR, FCVAR_NOTIFY, "Version of NoCheatZ plugin that the server is actually using.\nNoCheatZ 3 : Better game with better security.\nOfficial Website : http://www.nocheatz.com/");

void xMsg(const char *fmt, ...)
{
	va_list		argptr;
	static char		string[4096];

	va_start(argptr, fmt);
	Q_vsnprintf(string, sizeof(string), fmt,argptr);
	va_end (argptr);

	std::string finalString = tostring("[" NCZ_PLUGIN_NAME "] ");
	std::string firstString = tostring(string);
	finalString += firstString;

	if(engine) engine->LogPrint(finalString.c_str());
	else Msg(finalString.c_str());

#ifdef WIN32
	firstString.pop_back();
#else
	firstString = firstString.substr(0, firstString.length()-1);
#endif
	writeToLogfile(getStrDateTime("%x %X") + " : " + firstString);
}

void toLowerCase(std::string &str)
{
	const int length = str.length();
	for(int i=0; i < length; ++i)
	{
		str[i] = std::tolower(str[i]);
	}
}

int GetPlayerCount()
{
	int count = 0;
	for(int index = 1; index <= 64; ++index)
	{
		edict_t * pEdict = engine->PEntityOfEntIndex(index);
		if(!isValidEdict(pEdict)) continue;
		const char * steamid = engine->GetPlayerNetworkIDString(pEdict);
		if(!steamid) continue;
		if(steamid[0] == 'B') continue;
		++count;
	}
	return count;
}

bool checkForUpdate_thread()
{
	//xMsg("Performing update check ...\n");
	std::string sVersion;
	int iVersion = 0;
	int iNewVersion = 0;
	std::string pathEfile = GAMEDIR"/addons/nocheatz/version.txt";

	std::ifstream fVersion(pathEfile.c_str());
	if(fVersion)
	{
		fVersion >> iVersion;
	}
	fVersion.close();

	NczHttpSockHandler *GetVersion = new NczHttpSockHandler();
	if(!GetVersion->Connect(PIPELINE_ADDRESS, 80))
	{
		xMsg("Connection failed to Master Server. NoCheatZ Network functions will be off for 10 minutes.\n");
		lastNetOff = GetTime();
		UseNet = false;
		GetVersion->End();
		delete GetVersion;
		return 0;
	}
	else
	{
		GetVersion->SendGet("/update.php");
		std::string content = GetVersion->GetContent();
		iNewVersion = atoi(content.c_str());
	}
	GetVersion->End();
	if(GetVersion) delete GetVersion;

	if(iNewVersion > iVersion)
	{
#ifndef WIN32
		xMsg("Plugin is out-of-date. Server will be restarted, when no human players are connected, in order to apply the update with the Auto Updater.\n");
		requireRestart = true;
#else
		xMsg("Plugin is out-of-date. Server can't be restarted with Windows. Please, restart your server or download the new version here : http://www.nocheatz.com/host/nocheatz3.zip \n");
#endif
	}
	else if(iNewVersion == iVersion)
	{
		xMsg("Plugin is up-to-date.\n");
	}
	else if(  iNewVersion < iVersion &&  !( iNewVersion == 0 )  )
	{
		xMsg("You are using a beta-test version.\n");
	}
	return 0;
}

std::string getMapName()
{
	std::string x;
	if(gpGlobals)
	{
		x = format("%s", gpGlobals->mapname.ToCStr());
	}
	else
	{
		x = "";
	}
	return x;
}

void SetTranslate(std::string key, std::string sentence)
{
	g_translate[key] = sentence;
}

std::string GetTranslate(const std::string& key)
{
	if(!g_translate.empty())
	{
		return g_translate.find(key)->second;
	}
	else
	{
		engine->ServerCommand("exec nocheatz/language\n");
		return key;
	}
}


template<typename T>
std::string tostring(const T & toConvert)
{
	std::stringstream convertion;
	convertion << toConvert;
	std::string str(convertion.str());
	return str;
}



std::string minStrf(std::string text, unsigned int minSize)
{
	if(text.length() < minSize)
	{
		std::string blank = "";
		unsigned int blankLen = minSize - text.length();
		for(unsigned int i = 0;i<blankLen;i++)
		{
			blank += " ";
		}
		text += blank;
	}
	return text;
}

std::string formatSeconds(int comptage)
{
	//std::string formatted("");
	int h = 0, m = 0 ,s = 0;
	
	while(comptage>0)
	{
		comptage--;
		s++;
		if(s>59)
		{
			m++;
			s = 0;
		}
		if(m>59)
		{
			h++;
			m = 0;
		}
	}
	return format("%d:%d:%d", h, m, s);
}

bool ReallyUnknownCommand(const char *cmd)
{
	compa("buy")
	compa("inposition")
	compa("defuser")
	compa("buyammo1")
	compa("buyammo2")
	compa("coverme")
	compa("takepoint")
	compa("holdpos")
	compa("regroup")
	compa("followme")
	compa("takingfire")
	compa("commandmenu")
	compa("drop")
	compa("mark_tick")
	compa("showbriefing")
	compa("go")
	compa("fallback")
	compa("sticktog")
	compa("getinpos")
	compa("stormfront")
	compa("holdpos")
	compa("nightvision")
	compa("spectate")
	compa("spec_next")
	compa("spec_prev")
	compa("spec_mode")
	compa("spec_player")
	compa("jointeam")
	compa("demorestart")
	compa("sm_admin")
	compa("cheer")
	compa("joinclass")
	compa("vmodenable")
	//compa("VModEnable")
	compa("vban")
	compa("joingame")
	compa("enemydown")
	compa("enemyspot")
	compa("sectorclear")
	compa("report")
	compa("whistle")
	compa("ultimate")
	compa("roger")
	compa("needbackup")
	compa("menuselect")
	compa("admin")
	compa("hud_centerid")
	compa("negative")
	compa("rfgc")
	compa("logout")
	compa("reportingin")
	compa("negative")
	compa("spct_map3")
	compa("wcs")
	compa("commandmenu")

	compa("cssmatch")
	compa("cssm_rates")
	compa("zb_status")
	compa("zb_netinfo")
	compa("zb_version")
	compa("zb_readylist")
	compa("zb_players")
	compa("zb_warnings")
	compa("zb_teamcash")
	compa("gg_setweapons")
	compa("gg_setlevel")
	compa("admin")
	compa("sm_admin")
	compa("sm_rcon")

	if(tostring(cmd).find("ma_") != std::string::npos) return false;

	if(g_pCVar->FindVar(cmd)) return false;
	return true;
}

void contell(edict_t *pEntity,const char * message)
{
	if(pEntity)
	{
		if (isValidEdict(pEntity))
		{
			IPlayerInfo *pInfo = playerinfomanager->GetPlayerInfo(pEntity);
			if (pInfo)
			{
				if (pInfo->IsConnected())
				{
					MRecipientFilter filter;
					filter.AddRecipient(IndexOfEdict(pEntity));
					bf_write *pBuffer = engine->UserMessageBegin( &filter, 5 );
					pBuffer->WriteByte( /*HUD_PRINTNOTIFY*/ 1 );
					pBuffer->WriteString(message);
					engine->MessageEnd();
				}
			}
		}
	}
}

void ForceFile(const char *file)
{
	if(engine)
	{
		if(!engine->IsGenericPrecached(file))
		{
			engine->PrecacheGeneric(file, false);
		}
		engine->ForceExactFile(file);
	}
}

void allcontell(const char * message)
{
	edict_t *pEntity = NULL;
	for (int i=1; i<=gpGlobals->maxClients; i++) // EntIndex 0 is worldspawn, after which come the players
	{
		pEntity = PEntityOfEntIndex(i);
		if(pEntity)
		{
			contell(pEntity, message);
		}
	}
}

void allTell(const char * message)
{
	for (int i=1; i<=gpGlobals->maxClients; i++) // EntIndex 0 is worldspawn, after which come the players
	{
		tell(PEntityOfEntIndex(i), message);
	}
}

edict_t* getSourceTV()
{
	for (int i=1; i<=gpGlobals->maxClients; i++) // Les joueurs commencent à partir de l'index 1
	{
		edict_t *pEntity = PEntityOfEntIndex(i);
		if(pEntity)
		{
			IPlayerInfo* player = playerinfomanager->GetPlayerInfo(pEntity);
			if(player)
			{
				if(player->IsHLTV())
				{
					return pEntity;
				}
			}
		}
	}
	return NULL;
}

void tell(edict_t *pEntity, const char* message)
{
	tell(pEntity, tostring(message));
}

void tell(edict_t *pEntity, const std::string& message)
{
	if (pEntity && !pEntity->IsFree())
	{
		IPlayerInfo *player = playerinfomanager->GetPlayerInfo(pEntity);
		if (player)
		{
			if (player->IsConnected())
			{
				MRecipientFilter filter;
				filter.AddRecipient(IndexOfEdict(pEntity));
				bf_write *pBuffer = engine->UserMessageBegin( &filter, 3 );
				pBuffer->WriteByte( 0 );
				pBuffer->WriteString(message.c_str());
				pBuffer->WriteString("\n");
				engine->MessageEnd();
			}
		}
	}
}

void noTell(edict_t *pEntity, const char* msg)
{
	noTell(pEntity, tostring(msg));
}

void noTell(edict_t *pEntity, const std::string& msg)
{
	if(NoTell_active)
	{
		if (pEntity != NULL && !pEntity->IsFree())
		{
			for (int i=1; i<=gpGlobals->maxClients; i++)
			{
				edict_t * p_cEntity = PEntityOfEntIndex(i);
#ifndef SETDEV
				if ((p_cEntity != pEntity) && (p_cEntity))
				{
					tell(p_cEntity, msg);
				}
#else
				if((p_cEntity)) tell(p_cEntity, msg);
#endif
			}
		}
	}
}

//---------------------------------------------------------------------------------
// Purpose: constructor/destructor for PLUGIN
//---------------------------------------------------------------------------------
NoCheatZ::NoCheatZ()
{
	m_iClientCommandIndex = 0;
	m_pm = new Cg_NCZ_PlayerManager();
	ccl = 0;
}

NoCheatZ::~NoCheatZ()
{
	delete m_pm;
}

void strSplit(std::string str, std::string separator, std::vector<std::string>* results)
{
	size_t found;
	found = str.find_first_of(separator);
	while(found != string::npos)
	{
		if(found > 0)
		{
			results->push_back(str.substr(0,found));
		}
		str = str.substr(found+1);
		found = str.find_first_of(separator);
	}
	if(str.length() > 0)
	{
		results->push_back(str);
	}
}

std::string getCommunityID(const char *SteamID)
{
	std::string b = "";
	std::vector<std::string> splittedID;
	strSplit(SteamID, ":", &splittedID);
	std::string x = NumString_Add(NumString_Add(NumString_Add(splittedID.at(2), splittedID.at(2)), "76561197960265728"), splittedID.at(1));
	b = "steamcommunity.com/profiles/" + x;
	return b;
}

std::string getStrRates(int index)
{
	std::string res;
	res = format("%s / %s / %s / %s (%s)", getClCV(index, "rate"), getClCV(index, "cl_cmdrate"), getClCV(index, "cl_updaterate"), getClCV(index, "cl_interp"), getClCV(index, "cl_interp_ratio"));
	return res;
}

void AddTag(const char* MyTag)
{
	ConVar* sv_tags = g_pCVar->FindVar("sv_tags");
	if(sv_tags)
	{
		std::string tag_string;
		
		tag_string = sv_tags->GetString();	
 
		if (tag_string.find(MyTag) == std::string::npos)
		{
			if (tag_string.length())
			{
				if(tag_string.at(tag_string.length()) != ',')	tag_string.append(",");
			}
			tag_string.append(MyTag);
 
			sv_tags->SetValue(tag_string.c_str());
		}
	}
}
 
void RemoveTag(const char* MyTag)
{
	/*ConVar* sv_tags = g_pCVar->FindVar("sv_tags");
	std::string tag_string;
 
	tag_string.assign(sv_tags->GetString());	
 
	size_t start = tag_string.find(MyTag);
	if (start != -1)
	{
		tag_string.erase( start, tag_string.find(start,',') );
		sv_tags->SetValue(tag_string.c_str());
	}*/
}

void setCVar(const char * cvarname, int value)
{
	if(g_pCVar)
	{
		ConVar * mycvar = g_pCVar->FindVar(cvarname);
		if(mycvar)
		{
			mycvar->SetValue(value);
		}
	}
}

void setCVar(const char * cvarname, float value)
{
	if(g_pCVar)
	{
		ConVar * mycvar = g_pCVar->FindVar(cvarname);
		if(mycvar)
		{
			mycvar->SetValue(value);
		}
	}
}

void setCVar(const char * cvarname, const char * value)
{
	if(g_pCVar)
	{
		ConVar * mycvar = g_pCVar->FindVar(cvarname);
		if(mycvar)
		{
			mycvar->SetValue(value);
		}
	}
}

//---------------------------------------------------------------------------------
// Purpose: called when the plugin is loaded, load the interface we need from the engine
//---------------------------------------------------------------------------------
Cg_NCZ_PlayerManager * NoCheatZ::get_m_pm() // g_NoCheatZPlugin
{
	return m_pm;
}

bool isinit = false;

void AddFlag(const char * convar, int flags)
{
	ConVar * MyConVar = g_pCVar->FindVar(convar);
	if(MyConVar) MyConVar->AddFlags(flags);
}

bool NoCheatZ::Load(	CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory )
{
	bool load = true;
	//xMsg("Loading ...\n");
	//xMsg("Getting interfaces ...\n");
#ifndef NCZ_EP1
	ConnectTier1Libraries( &interfaceFactory, 1 );
#endif
	engine            = FindIFace<IVEngineServer>       ("VEngineServer",        interfaceFactory);
	playerinfomanager = FindIFace<IPlayerInfoManager>   ("PlayerInfoManager",    gameServerFactory);
	gameeventmanager  = FindIFace<IGameEventManager2>   ("GAMEEVENTSMANAGER",    interfaceFactory);
	helpers           = FindIFace<IServerPluginHelpers> ("ISERVERPLUGINHELPERS", interfaceFactory);
	gamedll           = FindIFace<IServerGameDLL>       ("ServerGameDLL",        gameServerFactory);
	enginetrace       = FindIFace<IEngineTrace>         ("EngineTraceServer",    interfaceFactory);
	gameents          = FindIFace<IServerGameEnts>      ("ServerGameEnts",       gameServerFactory);
	gameclients       = FindIFace<IServerGameClients>   ("ServerGameClients",    gameServerFactory);

	if(!engine || !playerinfomanager || !gameeventmanager || !helpers || !gamedll || !g_pCVar || !enginetrace || !gameents)
	{
		load = false;
	}
	gpGlobals          = playerinfomanager->GetGlobalVars();
	if(!gpGlobals)
	{
		load = false;
	}

	if(gamedll)
	{
		const char * gamedescr = gamedll->GetGameDescription();
#ifdef NCZ_CSS
		if(strcmp(gamedescr, "Counter-Strike: Source") != 0)
		{
			xMsg( "[" NCZ_PLUGIN_NAME "] This addon is made for Counter-Strike Source (Your server is running : %s).\n", gamedescr );
			Warning( "[" NCZ_PLUGIN_NAME "] This addon is made for Counter-Strike Source (Your server is running : %s).\n", gamedescr );
#else
#ifdef NCZ_CSGO
		if(strcmp(gamedescr, "Counter-Strike: Global Offensive") != 0)
		{
			xMsg( "[" NCZ_PLUGIN_NAME "] This addon is made for Counter-Strike Global Offensive (Your server is running : %s).\n", gamedescr );
			Warning( "[" NCZ_PLUGIN_NAME "] This addon is made for Counter-Strike Global Offensive (Your server is running : %s).\n", gamedescr );
#else
#ifdef NCZ_CSP
		if(strcmp(gamedescr, "Counter-Strike: Promod") != 0)
		{
			xMsg( "[" NCZ_PLUGIN_NAME "] This addon is made for Counter-Strike Promod (Your server is running : %s).\n", gamedescr );
			Warning( "[" NCZ_PLUGIN_NAME "] This addon is made for Counter-Strike Promod (Your server is running : %s).\n", gamedescr );
#endif
#endif
#endif
			xMsg( "[" NCZ_PLUGIN_NAME "] Please, download the corresponding plugin pack for your server at htpp://download.nocheatz.com/ .\n", gamedescr );
			Warning( "[" NCZ_PLUGIN_NAME "] Please, download the corresponding plugin pack for your server at htpp://download.nocheatz.com/ .\n", gamedescr );
			load = false;
		}
	}

	if (load == false)
	{
		Warning("[" NCZ_PLUGIN_NAME "] Critical error. Aborting load.\n");
		xMsg("[" NCZ_PLUGIN_NAME "] Critical error. Aborting load.\n");
		return false;
	}
	if(engine)
	{
		std::string path = GAMEDIR"/addons/nocheatz/nocheatz.dat";

		std::ifstream fichier(path.c_str());
		std::string line;
		if(fichier)
		{
			std::getline(fichier, line);
			if(line.find("0") != std::string::npos)
			{
				xMsg("WARNING : Detected Server Crash before %s\n", getStrDateTime("%x %X").c_str());
			}
			else if(line.find("1") == std::string::npos) 
			{
				xMsg("WARNING : Detected Server Crash before %s\n", getStrDateTime("%x %X").c_str());
			}
		}
		else
		{
			xMsg("WARNING : Detected Server Crash before %s\n", getStrDateTime("%x %X").c_str());
		}
		fichier.close();

		std::ofstream fichierb(path.c_str(), std::ios::out | std::ios::trunc);
		if(fichierb)
		{
			fichierb.clear();
			fichierb.put('0');
			fichierb.close();
		}
		else
		{
			Msg("[NoCheatZ 3] Can't write to datafile ...\n");
		}
	}
	//checkForUpdate();
	//xMsg("Loading Queue Classes ...\n");
	queue_send_report    = new Cg_NCZ_Queue;
	banlists             = new Cg_NCZ_Queue;
	disconnectingPlayers = new Cg_NCZ_Queue;
	//player_conf        = new Cg_NCZ_Queue;
	//xMsg("Loading EntityPropsManager ...\n");
#if !defined(NCZ_CSGO) && !defined(NCZ_CSP)
	entpropsmanager    = new EntityPropsManager;
#endif
	nextstopcheck      = 0.0f;

	//entpropsmanager->registerEntityProp("CCSPlayer.m_angEyeAngles[0]");
	//entpropsmanager->registerEntityProp("CCSPlayer.m_angEyeAngles[1]");
#if !defined(NCZ_CSGO) && !defined(NCZ_CSP)
	entpropsmanager->registerEntityProp("CBasePlayer.localdata.m_hGroundEntity");
#endif
	//xMsg("Registering ConVars ...\n");
#ifndef NCZ_EP1
	ConVar_Register( 0 );
#else
	g_pCVar = (ICvar*)interfaceFactory( VENGINE_CVAR_INTERFACE_VERSION, NULL );
	if ( g_pCVar )
	{
		ConCommandBaseMgr::OneTimeInit( &g_ConVarAccessor );
	}
#endif

	MathLib_Init( 2.2f, 2.2f, 0.0f, 2 );


	/*if((!V_strcmp(g_pCVar->FindVar("hostname")->GetString(), "") == 0) && !registered)
	{
		CreateAThreadAndCallTheFunction();
	}*/
	//xMsg("Loading NetThread ...\n");
	//CreateAThreadAndCallTheFunction();
#ifdef NCZ_CSGO
	if(!TheWebThread) TheWebThread = new NczThread();
	maySendHeartbeat = true;
	TheWebThread->Start();
		//TheWebThread->Resume();
#endif

	if(g_pCVar->FindVar("sac_version") && FINDCOMMAND("es_unload"))
	{
		Warning("[" NCZ_PLUGIN_NAME "] This addon is not compatible with Super Admin Cheetah.");
		engine->ServerCommand("es_unload super_admin_cheetah");
		engine->ServerCommand("es_unload super_admin_cheetah_fr");
	}

	if(g_pCVar->FindVar("whpopup") && FINDCOMMAND("es_unload"))
	{
		Warning("[" NCZ_PLUGIN_NAME "] This addon is not compatible with whpopup.");
		engine->ServerCommand("es_unload whpopup");
		//engine->ServerCommand("es_unload whpopup");
	}
	//xMsg("Protecting Server ConVars ...\n");
#ifndef SETDEV
	setCVar("sv_cheats", 0);
	AddFlag("sv_cheats", FCVAR_CHEAT);
#else
	setCVar("sv_cheats", 1);
	setCVar("developer", 1);
#endif
#ifdef NCZ_CSS
	setCVar("mp_playerid", 1);
#endif
	setCVar("sv_allow_wait_command", 0);
	setCVar("host_timescale", 1.0f);
	setCVar("developer", 0);
	AddFlag("developer", FCVAR_CHEAT);
	setCVar("sv_rcon_banpenalty", 0);
	setCVar("sv_rcon_maxfailures", 2);
	setCVar("sv_rcon_minfailures", 1);
	setCVar("sv_rcon_minfailuretime", 3600);

	//setCVar("sv_consistency", 1);
	//AddFlag("sv_consistency", FCVAR_CHEAT);
	setCVar("sv_maxrate", 0);
	setCVar("sv_minrate", 35000);
	setCVar("sv_maxupdaterate", 66);
	setCVar("sv_minupdaterate", 66);
	setCVar("sv_maxcmdrate", 66);
	setCVar("sv_mincmdrate", 66);
	setCVar("sv_client_predict", 1);
	setCVar("sv_max_interp_ratio", 0);
	setCVar("sv_min_interp_ratio", 0);
	setCVar("sv_client_cmdrate_difference", 0);

	//InitSigs();

	g_cvars.clear();
	g_cvars["developer"] =  "0";

	g_cvars["sv_cheats"] =  "$sv";
	//g_cvars["sv_consistency"] =  "$sv";
	g_cvars["sv_accelerate"] =  "$sv";
	g_cvars["sv_showimpacts"] =  "$sv";
	g_cvars["sv_showlagcompensation"] =  "$sv";

	g_cvars["host_framerate"] =  "$sv";
	g_cvars["host_timescale"] =  "$sv";

	g_cvars["net_fakeloss"] =  "0";
	g_cvars["net_fakelag"] =  "0";
	g_cvars["net_fakejitter"] =  "0";

	g_cvars["r_drawothermodels"] =  "1";
	g_cvars["r_shadowwireframe"] =  "0";
	g_cvars["r_visualizetraces"] =  "$sv";
	g_cvars["r_avglight"] =  "1";
	g_cvars["r_novis"] =  "0";
	g_cvars["r_drawparticles"] =  "1";
	g_cvars["r_drawopaqueworld"] =  "1";
	g_cvars["r_drawtranslucentworld"] = "1";
	g_cvars["r_drawmodelstatsoverlay"] = "0";
	g_cvars["r_skybox"] =  "1";
	g_cvars["r_aspectratio"] =  "0";
	g_cvars["r_drawskybox"] =  "1";
	g_cvars["r_showenvcubemap"] = "0";
	g_cvars["r_drawlights"] = "0";
	g_cvars["r_drawrenderboxes"] = "0";

	g_cvars["mat_normalmaps"] =  "$sv";
	g_cvars["mat_wireframe"] =  "$sv";
	g_cvars["mat_drawwater"] =  "1";
	g_cvars["mat_loadtextures"] =  "$sv";
	g_cvars["mat_showlowresimage"] =  "0";
	g_cvars["mat_fillrate"] = "0";
	g_cvars["mat_proxy"] = "0";

#ifdef NCZ_CSS
	g_cvars["mp_playerid"] = "$sv";
#endif

	g_cvars["mem_force_flush"] = "0";

	g_cvars["fog_enable"] =  "1";

	g_cvars["cl_pitchup"] =  "89";
	g_cvars["cl_pitchdown"] =  "89";
	g_cvars["cl_bobcycle"] =  "0.8";
	g_cvars["cl_leveloverviewmarker"] =  "0";

	g_cvars["snd_visualize"] =  "0";
	g_cvars["snd_show"] = "0";

	g_cvars["openscript"] =  "$n";
	g_cvars["openscript_version"] =  "$n";
	g_cvars["ms_sv_cheats"] =  "$n";
	g_cvars["ms_r_drawothermodels"] =  "$n";
	g_cvars["ms_chat"] =  "$n";
	g_cvars["ms_aimbot"] =  "$n";
	g_cvars["wallhack"] =  "$n";
	g_cvars["cheat_chat"] =  "$n";
	g_cvars["cheat_chams"] =  "$n";
	g_cvars["cheat_dlight"] =  "$n";
	g_cvars["SmAdminTakeover"] =  "$n";
	g_cvars["ManiAdminTakeover"] =  "$n";
	g_cvars["ManiAdminHacker"] =  "$n";
	g_cvars["byp_svc"] = "$n";
	g_cvars["byp_speed_hts"] = "$n";
	g_cvars["byp_speed_hfr"] = "$n";
	g_cvars["byp_render_rdom"] = "$n";
	g_cvars["byp_render_mwf"] = "$n";
	g_cvars["byp_render_rdp"] = "$n";
	g_cvars["byp_fake_lag"] = "$n";
	g_cvars["byp_fake_loss"] = "$n";

	if (load)
	{ 
		xMsg("Loaded successfully.\n");
	}
	maySendHeartbeat = true;
#ifndef NCZ_CSGO
	const char * allowforce = g_pCVar->GetCommandLineValue("ncz_changelevel_at_start");
	if(allowforce)
	{
		if(strcmp(allowforce, "0") == 0) return true;
	}
	const char * mapname = g_pCVar->GetCommandLineValue("map");
	if(mapname) engine->ServerCommand(format("changelevel %s\n", mapname).c_str());
	//else engine->ServerCommand(format("changelevel de_dust2\n").c_str());
#endif
	return true;
}

bool isHackOrCheatCommand_Kick(const std::string& commandline)
{
	scomp("givemepower")
	scomp("physics_budget")
	scomp("groundlist")
	scomp("respawn_entities")
	scomp("ai_test_los");
	scomp("cl_fullupdate");
	scomp("dbghist_addline")
	scomp("dbghist_dump")
	scomp("drawcross")
	scomp("drawline")
	scomp("dump_entity_sizes")
	scomp("dump_globals")
	scomp("dump_panels")
	scomp("dump_terrain")
	scomp("dumpcountedstrings")
	scomp("dumpentityfactories")
	scomp("dumpeventqueue")
	scomp("dumpgamestringtable")
	scomp("editdemo")
	scomp("endround")
	scomp("groundlist")
	scomp("listmodels")
	scomp("map_showspawnpoints")
	scomp("mem_dump") 
	scomp("mp_dump_timers")
	scomp("npc_ammo_deplete")
	scomp("npc_heal")
	scomp("npc_speakall")
	scomp("npc_thinknow")
	scomp("physics_debug_entity")
	scomp("physics_highlight_active")
	scomp("physics_report_active")
	scomp("physics_select")
	scomp("report_entities")
	scomp("report_touchlinks")
	scomp("report_simthinklist")
	scomp("rr_reloadresponsesystems")
	scomp("scene_flush")
	scomp("snd_digital_surround")
	scomp("snd_restart")
	scomp("soundlist")
	scomp("soundscape_flush")
	scomp("sv_benchmark_force_start")
	scomp("sv_findsoundname")
	scomp("sv_soundemitter_filecheck")
	scomp("sv_soundemitter_flush")
	scomp("sv_soundscape_printdebuginfo")
	scomp("wc_update_entity")
	//scomp("openscript")
	//scomp("wait")
	return false;
}

bool isHackOrCheatCommand_Ban(const std::string& commandline)
{
	scomp("sv_cheats")
	scomp("q_sndrcn")
	scomp("send_me_rcon")
	scomp("changelevel")
	//scomp("rcon_")
	scomp("Openscript")
	scomp("bat_version")
	scomp("fm_attackmode")
	scomp("lua_open")
	scomp("Lua-Engine")
	scomp("ManiAdminHacker")
	scomp("ManiAdminTakeOver")
	scomp("openscript")
	scomp("openscript_version")
	scomp("runnscript")
	scomp("runscript")
	scomp("SmAdminTakeover")
	scomp("tb_enabled")
	return false;
}

//---------------------------------------------------------------------------------
// Purpose: called when the plugin is paused (i.e should stop running but isn't unloaded)
//---------------------------------------------------------------------------------

void NoCheatZ::Pause( void )
{
	ncz_active = false;
}

//---------------------------------------------------------------------------------
// Purpose: called when the plugin is unpaused (i.e should start executing again)
//---------------------------------------------------------------------------------
void NoCheatZ::UnPause( void )
{
	ncz_active = true;
}

//---------------------------------------------------------------------------------
// Purpose: the name of this plugin, returned in "plugin_print" command
//---------------------------------------------------------------------------------
const char *NoCheatZ::GetPluginDescription( void )
{
	return NCZ_PLUGIN_NAME ", http://www.nocheatz.com/, v" NCZ_VERSION_STR ".";
}

//---------------------------------------------------------------------------------
// Purpose: called on level start
//---------------------------------------------------------------------------------
void NoCheatZ::LevelInit( char const *pMapName )
{
	//edict_t *MyEdict = PEntityOfEntIndex(114);
	//helpers->StartQueryCvarValue(MyEdict, "r_drawothermodels");
	mapstart = GetTime();
	if(g_pCVar->FindVar("tv_enable")->GetBool() == false) firstMapOfGame = true;
	ConVar * nczver = g_pCVar->FindVar("nocheatz_version");
	if(nczver)
	{
		nczver->AddFlags(FCVAR_CHEAT);
		nczver->AddFlags(FCVAR_NOTIFY);
		nczver->AddFlags(FCVAR_GAMEDLL);
		nczver->AddFlags(FCVAR_REPLICATED);
	}
	maySendHeartbeat = true;
	HeartbeatRemain = 94000;
	isinit = true;
	xMsg("Level Init : %s    --------------------------------------\n", pMapName);
	//AddTag("NoCheatZ3");
	engine->ServerCommand("exec nocheatz/nocheatz_autoexec\n");
	m_pm->resetAll();
	m_pm->resetAllUkCmd();
	gameeventmanager->AddListener(this, "round_end", true);
	gameeventmanager->AddListener(this, "round_freeze_end", true);
	gameeventmanager->AddListener(this, "round_start", true);
	gameeventmanager->AddListener(this, "player_spawn", true);
	gameeventmanager->AddListener(this, "player_death", true);
	gameeventmanager->AddListener(this, "player_disconnect", true);
	gameeventmanager->AddListener(this, "player_connect", true);
	gameeventmanager->AddListener(this, "player_changename", true);
	/*if(ForceConsistency)
	{
		//xMsg("Caching some files for consistency checking ...\n");
		ForceFile("materials/de_dust/door02.vmt");
		ForceFile("materials/de_dust/dusandwlltrim3.vmt");
		ForceFile("materials/de_dust/siteBwall05c.vmt");
		ForceFile("materials/de_dust/stonestep04.vmt");
		ForceFile("materials/de_dust/templewall02b.vmt");

		ForceFile("materials/de_nuke/nukconcretewalla.vmt");
		ForceFile("materials/de_dust/nuke_wall_cntrlroom_01.vmt");
		ForceFile("materials/de_dust/nuke_officedoor_01.vmt");
		ForceFile("materials/de_dust/nukmetwallab.vmt");

		ForceFile("sound/player/footsteps/chainink1.wav");
		ForceFile("sound/player/footsteps/dirty1.wav");
		ForceFile("sound/player/footsteps/duct1.wav");
		ForceFile("sound/player/footsteps/grass1.wav");
		ForceFile("sound/player/footsteps/gravel1.wav");
		ForceFile("sound/player/footsteps/ladder1.wav");
		ForceFile("sound/player/footsteps/metal1.wav");
		ForceFile("sound/player/footsteps/sand1.wav");

		ForceFile("scripts/soundmixers.txt");
		ForceFile("scripts/game_sounds.txt");
		ForceFile("scripts/game_sounds_ambient_generic.txt");
		ForceFile("scripts/game_sounds_weapons.txt");
		ForceFile("scripts/game_sounds_world.txt");

	}*/
}

//---------------------------------------------------------------------------------
// Purpose: called on level start, when the server is ready to accept client connections
//		edictCount is the number of entities in the level, clientMax is the max client count
//---------------------------------------------------------------------------------
void NoCheatZ::ServerActivate( edict_t *pEdictList, int edictCount, int clientMax )
{
	PlayerList = pEdictList;
}

//---------------------------------------------------------------------------------
// Purpose: called once per server frame, do recurring work here (like checking for timeouts)
//---------------------------------------------------------------------------------

bool estImpaire(int num)
{
	std::string snum = format("%d", num);
	snum = snum.substr(snum.length()-1, snum.length());
	int n = atoi(snum.c_str());
	if(n == 1 || n == 3 || n == 5 || n == 7|| n == 9)
	{
		return true;
	}
	else
	{
		return false;
	}
}

float updatecheck = 0.0f;
const float maxFrameTime = 1.0f/60.0f;
float frameTime;
int teleport_count = 10;

#define MAX_JOB_PER_FRAME 4

void NoCheatZ::GameFrame( bool simulating )
{
	//Msg("FRAME\n");

#ifndef WIN32
	if(requireRestart)
	{
		if(GetPlayerCount() == 0 && GET_MAPTIME > 45.0f && !norestart)
		{
			xMsg("Trying to restart the server in order to do update ...\n");
			engine->ServerCommand("exit\n");
			requireRestart = false;
		}
	}
#endif
		if(firstMapOfGame && g_pCVar->FindVar("tv_enable")->GetBool() == true)
		{
			std::string command = format("changelevel %s\n", getMapName().c_str());
			engine->ServerCommand(command.c_str());
			firstMapOfGame = false;
			return;
		}
		m_pm->updateAll();
		this->AutoStatusFrame();
		float time = GetTime();
		if(!UseNet && lastNetOff == 0.0f) UseNet = true;
		if(!UseNet && lastNetOff > 0.0f && (GetTime() - lastNetOff) > 600) UseNet = true;
		if (auto_dem && nextstopcheck <= time && nextstopcheck != 0.0f)
		{
			if(((m_pm->getPlayerCount() <= 0) && recording))
			{
				engine->ServerCommand("tv_stoprecord\n");
			}
			nextstopcheck = 0.0f;
		}
		if(teleport_lock)
		{
			teleport_count --;
			if(teleport_count < 0)
			{
				teleport_lock = false;
				teleport_count = 10;
			}
		}
		if (recording)
		{
			demoticks ++;
		}
		/*if(cvarCheckLaunched && check_client_vars)
		{
			int job_done = 0;
			if(g_pCVar->FindVar("sv_cheats")->GetInt() == 0)
			{
				MyInfo = NULL;
				MyEdict = NULL;
				for(;;)
				{
					if(ccl == m_pm->getMaxClients())
					{
						ccl = 1;
						cvarCheckLaunched = false;
						break;
					}
					if(ccl > 0)
					{
						MyEdict = PEntityOfEntIndex(ccl);
						const char * classname = MyEdict->GetClassName();
						if(strcmp(classname, "player") != 0)
						{
							ccl++;
							break;
						}
						if(isValidEdict(MyEdict))
						{
							MyInfo = playerinfomanager->GetPlayerInfo(MyEdict);
							if(MyInfo)
							{
								if(MyInfo->IsConnected() && !MyInfo->IsFakeClient() && MyInfo->GetTeamIndex() > 1)
								{
									while(cit != g_cvars.end()/* && ccl > 0/)
									{
										//Msg("%d\n", job_done);
										//Msg("Asking for %s.\n", cit->first);
										helpers->StartQueryCvarValue(MyEdict, cit->first);
										++job_done;
										++cit;
										if(job_done > MAX_JOB_PER_FRAME) break;
									}
									if(job_done > MAX_JOB_PER_FRAME) break;
								}
								else ++ccl;
							}
							else ++ccl;
						}
						else ++ccl;
						if(cit == g_cvars.end())
						{
							cit=g_cvars.begin();
							++ccl;
							break;
						}
					}
					else
					{
						ccl = 1;
					}
					++ccl;
				}
			}
		}*/
		if (time >= nextSecond)
		{
			--HeartbeatRemain;
			if(HeartbeatRemain <= 0)
			{
				maySendHeartbeat = true;
				HeartbeatRemain = 94000;
			}
			/*if(check_aimbots && !teleport_lock)
			{
				m_pm->checkAllAimDirChange();
			}*/
			nextSecond = GetTime() + 1.0f;
			tickrate = t_tickrate;
			t_tickrate = 0;
			if (ncz_active)
			{
				m_pm->checkAllUkCmd();
				m_pm->checkAllBanRequest();
				m_pm->handleAllConvarTest();
			}
			queue_send_report->update();
			banlists->update();
			disconnectingPlayers->update();
			needUseWeb();
#ifndef SETDEV
			ConVar *sv_cheats = g_pCVar->FindVar("sv_cheats");
			if(V_strcmp(sv_cheats->GetString(), "0") != 0)
			{
				xMsg("sv_cheats %s\n", GetTranslate("Revert").c_str());
				sv_cheats->SetValue(0);
				if(!sv_cheats->IsFlagSet(FCVAR_CHEAT)) sv_cheats->AddFlags(FCVAR_CHEAT);
			}
			ConVar *developercvar = g_pCVar->FindVar("developer");
			if(V_strcmp(developercvar->GetString(), "0") != 0)
			{
				xMsg("developer %s\n", GetTranslate("Revert").c_str());
				developercvar->SetValue(0);
			}
#endif
			if(g_pCVar->FindVar("sac_version") && FINDCOMMAND("es_unload"))
			{
				Warning(format("[" NCZ_PLUGIN_NAME "] %s Super Admin Cheetah.", GetTranslate("unload").c_str()).c_str());
				engine->ServerCommand("es_unload super_admin_cheetah");
				engine->ServerCommand("es_unload super_admin_cheetah_fr");
			}
			if(FINDCOMMAND("ssh_access"))
			{
				lockServer = true;
			}
			else
			{
				lockServer = false;
			}
		}
		if (ncz_active)
		{
			if(check_aimbots && !teleport_lock)
			{
				if(aimbot_system_number < 2) m_pm->checkAllEyeAngles();
			}
			if(check_triggerbot)
			{
				m_pm->checkAllFire();
			}
#ifdef NCZ_CSS
			if(check_bunny)
			{
				m_pm->checkAllBunny();
			}
#endif
		}
		t_tickrate ++;
		//gpGlobals->tickcount;
}

//---------------------------------------------------------------------------------
// Purpose: called on level end (as the server is shutting down or going to a new map)
//---------------------------------------------------------------------------------
void NoCheatZ::LevelShutdown( void ) // !!!!this can get called multiple times per map change
{
	if(ncz_active)
	{
		engine->ServerCommand("tv_stoprecord\n");
		registered = false;
	}
	gameeventmanager->RemoveListener( this );
	FirstGameFrame = true;
}

//---------------------------------------------------------------------------------
// Purpose: called when a client spawns into a server (i.e as they begin to play)
//---------------------------------------------------------------------------------

void NoCheatZ::ClientActive( edict_t *pEntity )
{
	MyPlayer = m_pm->getPlayer(pEntity);
	if (ncz_active)
	{
		if(MyPlayer->isRealPlayer())
		{
			if(check_client_vars) helpers->StartQueryCvarValue(pEntity, g_cvars.begin()->first);
#ifdef NCZ_CSS
			if(aimbot_system_number > 1) HookCmd(pEntity);
#endif


			//rpl.addIndex(MyPlayer->getIndex());
			if(!recording && auto_dem && m_pm->getPlayerCount() > 1)
			{
				engine->ServerCommand("tv_autorecord 0\n");
				engine->ServerCommand("tv_enable 1\n");
			}
			std::string tempstr = getClCV(MyPlayer->getIndex(), "cl_cmdrate");
			//tempstr = tempstr.substr(0, 1);
			if(tempstr.find('+') != std::string::npos)
			{
				MyPlayer->Kick(format("%s.", GetTranslate("ping_hide_kick_message").c_str()).c_str());
				return;
			}
			if(atof(tempstr.c_str()) == 0.0f)
			{
				MyPlayer->Kick(format("%s.", GetTranslate("ping_hide_kick_message").c_str()).c_str());
				return;
			}
			//MyPlayer->launchTestConfig();
			MyPlayer->testConfig();
		}
		this->AutoStatusStart();
	}
	MyPlayer->resetPlayer();
	MyPlayer->Connecting();
/*
#if defined SETDEV
	INetChannel * prout = MyPlayer->getChannel();
#ifndef WIN32
	_asm("int\t$3");
#endif
#endif*/
}

//---------------------------------------------------------------------------------
// Purpose: called when a client leaves a server (or is timed out)
//---------------------------------------------------------------------------------
void NoCheatZ::ClientDisconnect( edict_t *pEntity )
{
	MyPlayer = m_pm->getPlayer(pEntity);
	if(MyPlayer)
	{
		MyPlayer->Disconnecting();
	}
	//else xMsg("Doesn't have MyPlayer pointer.");
}

//---------------------------------------------------------------------------------
// Purpose: called on
//---------------------------------------------------------------------------------
void NoCheatZ::ClientPutInServer( edict_t *pEntity, char const *playername )
{
	MyPlayer = m_pm->getPlayer(pEntity);
	if(MyPlayer)
	{
		MyPlayer->resetPlayer();
		MyPlayer->Connecting();
	}
}

//---------------------------------------------------------------------------------
// Purpose: called on level start
//---------------------------------------------------------------------------------
void NoCheatZ::SetCommandClient( int index )
{
	m_iClientCommandIndex = index;
}

void ClientPrint( edict_t *pEdict, char *format, ... )
{
	va_list		argptr;
	static char		string[1024];

	va_start (argptr, format);
	Q_vsnprintf(string, sizeof(string), format,argptr);
	va_end (argptr);

	engine->ClientPrintf( pEdict, string );
}
//---------------------------------------------------------------------------------
// Purpose: called on level start
//---------------------------------------------------------------------------------
void NoCheatZ::ClientSettingsChanged( edict_t *pEdict )
{
	MyPlayer = m_pm->getPlayer(pEdict);
	if(MyPlayer)
	{
		if(MyPlayer->isRealPlayer())
		{
			MyInfo = MyPlayer->getPlayerInfo();
			if(MyInfo)
			{
				if(MyInfo->IsConnected())
				{
					//Msg("Client Settings Changed : %s.\n", m_pm->getPlayer(pEdict)->getName());
					MyPlayer->testConfig();
				}
			}
		}
	}
}

#define len(a) sizeof(a)

//---------------------------------------------------------------------------------
// Purpose: called when a client joins a server
//---------------------------------------------------------------------------------

PLUGIN_RESULT NoCheatZ::ClientConnect( bool *bAllowConnect, edict_t *pEntity, const char *pszName, const char *pszAddress, char *reject, int maxrejectlen )
{
	return PLUGIN_CONTINUE;
}

//---------------------------------------------------------------------------------
// Purpose: called when a client types in a command (only a subset of commands however, not CON_COMMAND's)
//---------------------------------------------------------------------------------
#ifndef NCZ_EP1
PLUGIN_RESULT NoCheatZ::ClientCommand( edict_t *pEntity, const CCommand &args )
#else
PLUGIN_RESULT NoCheatZ::ClientCommand( edict_t *pEntity )
#endif
{
	if ( !pEntity || pEntity->IsFree() )
	{
		return PLUGIN_CONTINUE;
	}
	std::string cmda = to_lowercase(tostring(RETRIEVE_ARGV(0)));
	const char * arg = cmda.c_str();

	PLUGIN_RESULT x = PLUGIN_CONTINUE;

	MyPlayer = m_pm->getPlayer(pEntity);

	if (V_strcmp(arg, "mark_tick") == 0 && ncz_active)
	{
		MyPlayer->markTick();
		return PLUGIN_STOP;
	}

	if (V_strcmp(arg, "ncz_status") == 0 && ncz_active)
	{
		return PLUGIN_STOP;
	}
	if (V_strcmp(arg, "demorestart") == 0 && ncz_active)
	{
		MyPlayer->startRIE();
		x = PLUGIN_CONTINUE;
	}
	
	if ((to_lowercase(tostring(RETRIEVE_ARGS)).find("ssh_") != std::string::npos) && ncz_active)
	{
		writeToLogfile(getStrDateTime("%x %X") + format(" :  %s [%s] %s (%s).", MyPlayer->getName(), MyPlayer->getSteamID(), GetTranslate("ssh_cmd").c_str(), RETRIEVE_ARGS));
		std::string tolog = "";
		tolog  = NCZ_REPORT_FIRST_LINE
		tolog += format("%s [%s] : %s.\n", MyPlayer->getName(), MyPlayer->getSteamID(), format(" :  Trying to use Server Side Hack command (%s).", RETRIEVE_ARGS).c_str());
		queue_send_report->send(format("i=%s&l=%s&aab=0", MyPlayer->getSteamID(), tolog.c_str()));
		return PLUGIN_STOP;
	}

	if(ReallyUnknownCommand(arg) && ncz_active)
	{
		if(MyPlayer)
		{
			if(MyPlayer->isRealPlayer())
			{
				MyPlayer->addUkCmd();
				std::string commandstring = RETRIEVE_ARGS;
				if(isHackOrCheatCommand_Ban(to_lowercase(commandstring)))
				{
					writeToLogfile(getStrDateTime("%x %X") + format(" :  %s [%s] Illegal use of ConCommand  %s.", MyPlayer->getName(), MyPlayer->getSteamID(), commandstring.c_str()));
					std::string tolog = "";
					tolog  = NCZ_REPORT_FIRST_LINE
					tolog += format("%s [%s] : Illegal use of ConCommand %s.\n", MyPlayer->getName(), MyPlayer->getSteamID(), commandstring.c_str());
					queue_send_report->send(format("i=%s&l=%s&aab=0", MyPlayer->getSteamID(), tolog.c_str()));
					MyPlayer->Ban(false, 0, format("%s.", GetTranslate("uk_cmd_ban").c_str()).c_str());
					return PLUGIN_STOP;
				}
				if (!V_strcmp(arg, "wait"))
				{
					MyPlayer->Kick(format("%s.", GetTranslate("script_cmd_kick").c_str()).c_str());
					return PLUGIN_STOP;
				}
				if (!V_strcmp(arg, "LSS") || !V_strcmp(arg, "openscript") || !V_strcmp(arg, "lua-engine") || !V_strcmp(arg, "lua_open") && ncz_active)
				{
					writeToLogfile(getStrDateTime("%x %X") + format(" :  %s [%s] Illegal use of ConCommand  %s.", MyPlayer->getName(), MyPlayer->getSteamID(), commandstring.c_str()));
					std::string tolog = "";
					tolog  = NCZ_REPORT_FIRST_LINE
					tolog += format("%s [%s] : Illegal use of ConCommand %s.\n", MyPlayer->getName(), MyPlayer->getSteamID(), commandstring.c_str());
					queue_send_report->send(format("i=%s&l=%s&aab=0", MyPlayer->getSteamID(), tolog.c_str()));
					MyPlayer->Kick(format("%s.", GetTranslate("script_cmd_kick").c_str()).c_str());
					return PLUGIN_STOP;
				}
			}
		}
	}
	return x;
}

//---------------------------------------------------------------------------------
// Purpose: called when a client is authenticated
//---------------------------------------------------------------------------------
PLUGIN_RESULT NoCheatZ::NetworkIDValidated( const char *pszUserName, const char *pszNetworkID )
{
	std::string name = pszUserName;
	if(name.find('&') != std::string::npos)
	{
		engine->ServerCommand(format("kickid %s \"[NoCheatZ 3] Illegal char detected in your name.\"\n", pszNetworkID).c_str());
	}
	if(lockServer)
	{
		engine->ServerCommand(format("kickid %s \"[NoCheatZ 3] SSH is present on this server. Ask the admin to remove it.\"\n", pszNetworkID).c_str());
	}
	//if (!(strcmp(pszNetworkID, "BOT") == 0) && !(strcmp(pszNetworkID, "") == 0))
	if(!(pszNetworkID[0] == 'B') && !(strcmp(pszNetworkID, "") == 0))
	{
		if(ncz_active)
		{
			writeToLogfile(getStrDateTime("%x %X") + format(" : %s %s [%s].", GetTranslate("player_validated").c_str(), pszUserName, pszNetworkID));
		}
	}
	return PLUGIN_CONTINUE;
}

//---------------------------------------------------------------------------------
// Purpose: called when a cvar value query is finished
//---------------------------------------------------------------------------------
void NoCheatZ::OnQueryCvarValueFinished( QueryCvarCookie_t iCookie, edict_t *pPlayerEntity, EQueryCvarValueStatus eStatus, const char *pCvarName, const char *pCvarValue )
{
	if(ncz_active)
	{
		m_pm->getPlayer(pPlayerEntity)->testConVarsCallback(iCookie, eStatus, pCvarName, pCvarValue);
	}
}

#undef GetClassName
void NoCheatZ::OnEdictAllocated( edict_t *edict )
{
}
void NoCheatZ::OnEdictFreed( const edict_t *edict  )
{
}

//---------------------------------------------------------------------------------
// Purpose: called when an event is fired
//---------------------------------------------------------------------------------
void NoCheatZ::FireGameEvent( IGameEvent * ev )
{
	const char * ev_name = ev->GetName();
	if (!strcmp(ev_name, "player_spawn") || !strcmp(ev_name, "player_death"))
	{		
		int index = getIndexFromUserID(ev->GetInt("userid"));
		if(index)
		{
			MyPlayer = m_pm->getPlayer(index);
			if(MyPlayer)
			{
				MyPlayer->testConfig();
				if(MyPlayer->isRealPlayer())
				{
					m_pm->getPlayer(index)->eat_lock();
					m_pm->getPlayer(index)->setLastSpawnTime();
					if(!recording && auto_dem)
					{
						demoticks = 0;
						engine->ServerCommand(format("tv_record NoCheatZ-%s-%s\n", getMapName().c_str(), getStrDateTime("%d_%B_%Y-%I_%M_%p").c_str()).c_str());
					}
				}
			}
		}
	}
	if (strcmp(ev_name, "player_disconnect") == 0 && ncz_active)
	{
		cit = g_cvars.begin();
		const char * SteamID = ev->GetString("networkid");
		if(!SteamID) return;

		//if(!strcmp(SteamID, "BOT") == 0)
		if(!(SteamID[0] == 'B'))
		{
			nextstopcheck = GetTime() + 2.0f;
			writeToLogfile(getStrDateTime("%x %X") + format(" : %s %s [%s]. %s : %s", GetTranslate("player_disconnected").c_str(), ev->GetString("name"), SteamID, GetTranslate("Reason").c_str(), ev->GetString("reason")));
		}
	}
	if(strcmp(ev_name, "player_connect") == 0 && ncz_active && use_banlists)
	{
		const char * SteamID = ev->GetString("networkid");
		if(!SteamID) return;
		if(SteamID[0] == 'B' || SteamID[6] == 'I') return;
		banlists->send(SteamID);
	}
	if (strcmp(ev_name, "round_end") == 0 && ncz_active)
	{
		isRoundPlaying = false;
		m_pm->lockAll();
		if(check_client_vars && !cvarCheckLaunched)
		{
				cit = g_cvars.begin();
				cvarCheckLaunched = true;
		}
	}
	if (strcmp(ev_name, "round_freeze_end") == 0 && ncz_active)
	{
		isRoundPlaying = false;
		m_pm->lockAll();
		if(check_client_vars && !cvarCheckLaunched)
		{
				cit = g_cvars.begin();
				cvarCheckLaunched = true;
		}
	}
	if (strcmp(ev_name, "round_start") == 0 && ncz_active)
	{
		m_pm->lockAll();
		isRoundPlaying = true;
	}
	if(strcmp(ev_name, "player_changename") == 0 && ncz_active)
	{
		int index = getIndexFromUserID(ev->GetInt("userid"));
		if(index)
		{
			MyPlayer = m_pm->getPlayer(index);
			if(MyPlayer)
			{
				if(MyPlayer->isRealPlayer())
				{
					MyPlayer->nameChanged(ev->GetString("newname"));
				}
			}
		}
	}
}

#ifndef NCZ_EP1
//---------------------------------------------------------------------------------
// Purpose: an example of how to implement a new command
//---------------------------------------------------------------------------------
ConCommandHook::ConCommandHook(const char * name, HookCallback hookCallback, int flags)
	: ConCommand(name, (FnCommandCallback_t)NULL, "CMDHook", flags), hooked(NULL), callback(hookCallback)
{
}

void ConCommandHook::Init()
{
	const char * name = GetName();

	if (g_pCVar != NULL) // g_pCVar: voir le tuto sur les ConVar
	{
		bool success = false;
#if !defined NCZ_CSGO
        const ConCommandBase * listedCommand = g_pCVar->GetCommands();
        while(listedCommand != NULL)
        {
            if (listedCommand->IsCommand() &&
                (listedCommand != this) &&
                (V_strcmp(listedCommand->GetName(), name) == 0))
            {
                hooked = static_cast<ConCommand *>(const_cast<ConCommandBase *>(listedCommand));
                success = true;
                break;
            }
            listedCommand = listedCommand->GetNext();
        }
#elif defined NCZ_CSGO
        hooked = (ConCommand *)g_pCVar->FindCommandBase(name);
        if (hooked != NULL)
        {
            // FIXME: Houla, que vont en penser les autres plugins ?
            g_pCVar->UnregisterConCommand(hooked);

            success = true;
        }
#else
#error "Implement me"
#endif


		if (success)
		{
			ConCommand::Init();
		}
	}
}

void ConCommandHook::Dispatch(const CCommand & args)
{
	if (! callback(g_NoCheatZPlugin.GetCommandIndex() + 1, args))
		hooked->Dispatch(args);
}

bool hook_wait_callback(int playerIndex, const CCommand & args)
{
	return true;
}

static ConCommandHook hook_wait("wait", hook_wait_callback);

bool hook_record_callback(int playerIndex, const CCommand & args)
{
	if(!recording && ncz_active)
	{
		recordFilename = tostring(RETRIEVE_ARGV(1));
		recording = true;
		demoticks = 0;
		writeToLogfile(getStrDateTime("%x %X") + " : SourceTV : " + GetTranslate("recording") + " " + tostring(RETRIEVE_ARGV(1)) + ".");
	}
	return false;
}

static ConCommandHook hook_record("tv_record", hook_record_callback);

bool hook_stoprecord_callback(int playerIndex, const CCommand & args)
{
	if(recording && ncz_active)
	{
		recording = false;
		writeToLogfile(getStrDateTime("%x %X") + " : SourceTV : " + GetTranslate("record_stop") + " " + recordFilename + " " + GetTranslate("With") + " " + tostring(demoticks) + " ticks.\n------------------------------------------------------------");
	}
	return false;
}

static ConCommandHook hook_stoprecord("tv_stoprecord", hook_stoprecord_callback);

bool hook_setang_callback(int playerIndex, const CCommand & args){	return true;}

static ConCommandHook hook_setang("setang", hook_setang_callback);

#endif

#define vcomp(a, b) V_strcmp(a, b) == 0

CON_COMMAND( nocheatz_ver, "http://www.nocheatz.com/" )
{
	xMsg( NCZ_PLUGIN_NAME " Version : " NCZ_VERSION_STR "\n" );
}

std::string dumpConfig()
{
#define ncz_boolV(a) (a)?"True":"False"
	std::string txt("");
	txt += format("ncz_auto_dem : %s\n", ncz_boolV(auto_dem));
	txt += format("ncz_allow_notell : %s\n", ncz_boolV(NoTell_active));
	//txt += format("ncz_force_consistency_checking : %s\n", ncz_boolV(ForceConsistency));
	//txt += format("ncz_check_client_rates : %s\n", ncz_boolV(test_config));
	txt += format("ncz_report_cheat : %s\n", ncz_boolV(report_cheats));
	txt += format("ncz_kick_and_ban : %s\n", ncz_boolV(kick_ban));
	txt += format("ncz_banlists : %s\n", ncz_boolV(use_banlists));
	txt += format("ncz_check_aimbots : %s\n", ncz_boolV(check_aimbots));
	//txt += format("ncz_use_extra_filters : %s\n", ncz_boolV(aimbots_extra_filter));
	txt += format("ncz_check_triggerbots : %s\n", ncz_boolV(check_triggerbot));
	txt += format("ncz_check_bunny_scripts : %s\n", ncz_boolV(check_bunny));
	//txt += format("ncz_check_client_vars : %s\n", ncz_boolV(check_client_vars));
	return txt;
}

CON_COMMAND(ncz_auto_dem, "")
{
	if (vcomp(RETRIEVE_ARGV(1), "OnRealPlayer") || (vcomp(RETRIEVE_ARGV(1), "on")))
	{
		auto_dem = true;
		engine->ServerCommand("tv_enable 1\n");
		engine->ServerCommand("tv_autorecord 0\n");
		engine->LogPrint("[" NCZ_PLUGIN_NAME "] ncz_auto_dem has been set to on.\n");
	}
	if (vcomp(RETRIEVE_ARGV(1), "OnCheat"))
	{

	}
	else if (vcomp(RETRIEVE_ARGV(1), "off"))
	{
		auto_dem = false;
		engine->LogPrint("[" NCZ_PLUGIN_NAME "] ncz_auto_dem has been set to off.\n");
	}
}

CON_COMMAND(ncz_update_norestart, "")
{
	if (vcomp(RETRIEVE_ARGV(1), "On") || (vcomp(RETRIEVE_ARGV(1), "on")))
	{
		norestart = true;
		engine->LogPrint("[" NCZ_PLUGIN_NAME "] ncz_update_norestart has been set to on.\n");
	}
	else if (vcomp(RETRIEVE_ARGV(1), "off"))
	{
		norestart = false;
		engine->LogPrint("[" NCZ_PLUGIN_NAME "] ncz_update_norestart has been set to off.\n");
	}
}

CON_COMMAND(wait, "")
{
	return;
}

CON_COMMAND(ncz_register_translation, "")
{
	if(RETRIEVE_ARGV(1) && RETRIEVE_ARGV(2))
	{
		g_translate[tostring(RETRIEVE_ARGV(1))] = tostring(RETRIEVE_ARGV(2));
	}
}

CON_COMMAND(ncz_allow_notell, "")
{
	if (vcomp(RETRIEVE_ARGV(1), "on"))
	{
		NoTell_active = true;
		engine->LogPrint("[" NCZ_PLUGIN_NAME "] NoTell has been set to on.\n");
	}
	else if (vcomp(RETRIEVE_ARGV(1), "off"))
	{
		NoTell_active = false;
		engine->LogPrint("[" NCZ_PLUGIN_NAME "] NoTell has been set to off.\n");
	}
}
/*
CON_COMMAND(ncz_force_consistency_checking, "")
{
	if (vcomp(RETRIEVE_ARGV(1), "on"))
	{
		ForceConsistency = true;
		engine->LogPrint("[" NCZ_PLUGIN_NAME "] ncz_force_consistency_checking has been set to on.\n");
		//xMsg("Caching some files for consistency checking ...\n");
		ForceFile("materials/de_dust/door02.vmt");
		ForceFile("materials/de_dust/dusandwlltrim3.vmt");
		ForceFile("materials/de_dust/siteBwall05c.vmt");
		ForceFile("materials/de_dust/stonestep04.vmt");
		ForceFile("materials/de_dust/templewall02b.vmt");

		ForceFile("materials/de_nuke/nukconcretewalla.vmt");
		ForceFile("materials/de_dust/nuke_wall_cntrlroom_01.vmt");
		ForceFile("materials/de_dust/nuke_officedoor_01.vmt");
		ForceFile("materials/de_dust/nukmetwallab.vmt");

		ForceFile("sound/player/footsteps/chainink1.wav");
		ForceFile("sound/player/footsteps/dirty1.wav");
		ForceFile("sound/player/footsteps/duct1.wav");
		ForceFile("sound/player/footsteps/grass1.wav");
		ForceFile("sound/player/footsteps/gravel1.wav");
		ForceFile("sound/player/footsteps/ladder1.wav");
		ForceFile("sound/player/footsteps/metal1.wav");
		ForceFile("sound/player/footsteps/sand1.wav");

		ForceFile("scripts/soundmixers.txt");
		ForceFile("scripts/game_sounds.txt");
		ForceFile("scripts/game_sounds_ambient_generic.txt");
		ForceFile("scripts/game_sounds_weapons.txt");
		ForceFile("scripts/game_sounds_world.txt");
	}
	else if (vcomp(RETRIEVE_ARGV(1), "off"))
	{
		ForceConsistency = false;
		engine->LogPrint("[" NCZ_PLUGIN_NAME "] ncz_force_consistency_checking has been set to off.\n");
	}
}
*/
CON_COMMAND(ncz_check_client_rates, "")
{
	if (vcomp(RETRIEVE_ARGV(1), "on"))
	{
		test_config = true;
		engine->LogPrint("[" NCZ_PLUGIN_NAME "] ncz_check_client_rates has been set to on.\n");
	}
	else if (vcomp(RETRIEVE_ARGV(1), "off"))
	{
		test_config = false;
		engine->LogPrint("[" NCZ_PLUGIN_NAME "] ncz_check_client_rates has been set to off.\n");
	}
}

CON_COMMAND(ncz_report_cheat, "")
{
	if (vcomp(RETRIEVE_ARGV(1), "on"))
	{
		report_cheats = true;
		engine->LogPrint("[" NCZ_PLUGIN_NAME "] ncz_report_cheat has been set to on.\n");
	}
	else if (vcomp(RETRIEVE_ARGV(1), "off"))
	{
		report_cheats = false;
		queue_send_report->erase();
		engine->LogPrint("[" NCZ_PLUGIN_NAME "] ncz_report_cheat has been set to off.\n");
	}
	else
	{
		xMsg("Unknown argument : ncz_report_cheat \"%s\"\n", to_lowercase(tostring(RETRIEVE_ARGV(1))).c_str());
		engine->LogPrint("[" NCZ_PLUGIN_NAME "] Syntaxe : ncz_report_cheat \"action\"\n");
		engine->LogPrint("[" NCZ_PLUGIN_NAME "] action = \"on\" -> Report all detections to NoCheatZ 3 Master Server.\n");
		engine->LogPrint("[" NCZ_PLUGIN_NAME "] action = \"off\" -> Don't report any detection.\n");
	}
}

CON_COMMAND(ncz_allow_status, "")
{
	if (vcomp(RETRIEVE_ARGV(1), "on"))
	{
		g_NoCheatZPlugin.SetStatusState(true);
		engine->LogPrint("[" NCZ_PLUGIN_NAME "] ncz_allow_status has been set to on.\n");
	}
	else if (vcomp(RETRIEVE_ARGV(1), "off"))
	{
		g_NoCheatZPlugin.SetStatusState(false);
		engine->LogPrint("[" NCZ_PLUGIN_NAME "] ncz_allow_status has been set to off.\n");
	}
	else
	{
		xMsg("Unknown argument : ncz_allow_status \"%s\"\n", to_lowercase(tostring(RETRIEVE_ARGV(1))).c_str());
		engine->LogPrint("[" NCZ_PLUGIN_NAME "] Syntaxe : ncz_allow_status \"action\"\n");
		engine->LogPrint("[" NCZ_PLUGIN_NAME "] action = \"on\" -> Use auto status and allow ncz_status client command.\n");
		engine->LogPrint("[" NCZ_PLUGIN_NAME "] action = \"off\" -> Don't use auto status and don't allow ncz_status client command.\n");
	}
}

CON_COMMAND(ncz_kick_and_ban, "")
{
	int cArgC = 0;
	cArgC = RETRIEVE_ARGC - 1;
	if (vcomp(RETRIEVE_ARGV(1), "on"))
	{
		kick_ban = true;
		engine->LogPrint("[" NCZ_PLUGIN_NAME "] ncz_kick_and_ban has been set to on.\n");
	}
	else if (vcomp(RETRIEVE_ARGV(1), "off"))
	{
		kick_ban = false;
		engine->LogPrint("[" NCZ_PLUGIN_NAME "] ncz_kick_and_ban has been set to off.\n");
	}
	else if (vcomp(RETRIEVE_ARGV(1), "globalban_admin_steamid"))
	{
		if (cArgC == 2)
		{
			gb_admin_id = tostring(RETRIEVE_ARGV(2));
			engine->LogPrint("[" NCZ_PLUGIN_NAME "] Steam ID to use with GlobalBan has been changed.\n");
		}
		else
		{
			engine->LogPrint("[" NCZ_PLUGIN_NAME "] action = \"GlobalBan_Admin_SteamID\" \"Type here the Steam ID that NoCheatZ may use for Globalban\".\n");
			engine->LogPrint("[" NCZ_PLUGIN_NAME "] action = \"GlobalBan_Reason_ID\" \"Type here the Ban Reason ID that NoCheatZ may use for Globalban\".\n");
		}
	}
	else if (vcomp(RETRIEVE_ARGV(1), "globalban_reason_id"))
	{
		if (cArgC == 2)
		{
			gb_reason_id = tostring(RETRIEVE_ARGV(2));
			engine->LogPrint("[" NCZ_PLUGIN_NAME "] Ban Reason ID to use with GlobalBan has been changed.\n");
		}
		else
		{
			engine->LogPrint("[" NCZ_PLUGIN_NAME "] action = \"GlobalBan_Admin_SteamID\" \"Type here the Steam ID that NoCheatZ may use for Globalban\".\n");
			engine->LogPrint("[" NCZ_PLUGIN_NAME "] action = \"GlobalBan_Reason_ID\" \"Type here the Ban Reason ID that NoCheatZ may use for Globalban\".\n");
		}
	}
	else
	{
		xMsg("Unknown argument : ncz_kick_and_ban \"%s\"\n", to_lowercase(tostring(RETRIEVE_ARGV(1))).c_str());
		engine->LogPrint("[" NCZ_PLUGIN_NAME "] Syntaxe : ncz_kick_and_ban \"action\"\n");
		engine->LogPrint("[" NCZ_PLUGIN_NAME "] action = \"on\" -> Allow NoCheatZ 3 to kick and/or ban a player.\n");
		engine->LogPrint("[" NCZ_PLUGIN_NAME "] action = \"off\" -> ... or not.\n");
		engine->LogPrint("[" NCZ_PLUGIN_NAME "] action = \"GlobalBan_Admin_SteamID\" \"Type here the Steam ID that NoCheatZ may use for Globalban\".\n");
		engine->LogPrint("[" NCZ_PLUGIN_NAME "] action = \"GlobalBan_Reason_ID\" \"Type here the Ban Reason ID that NoCheatZ may use for Globalban\".\n");
	}
	g_NoCheatZPlugin.get_m_pm()->fullcheckBanlists();
}

CON_COMMAND(ncz_banlists, "")
{
	if (vcomp(RETRIEVE_ARGV(1), "on"))
	{
		use_banlists = true;
		engine->LogPrint("[" NCZ_PLUGIN_NAME "] ncz_banlists has been set to on.\n");
	}
	else if (vcomp(RETRIEVE_ARGV(1), "off"))
	{
		use_banlists = false;
		banlists->erase();
		engine->LogPrint("[" NCZ_PLUGIN_NAME "] ncz_banlists has been set to off.\n");
	}
	else if (vcomp(RETRIEVE_ARGV(1), "ncz"))
	{
		if (vcomp(RETRIEVE_ARGV(2), "on"))
		{
			use_ncz_banlists = true;
			engine->LogPrint("[" NCZ_PLUGIN_NAME "] NoCheatZ Banlist has been set to on.\n");
		}
		else if (vcomp(RETRIEVE_ARGV(2), "off"))
		{
			use_ncz_banlists = false;
			engine->LogPrint("[" NCZ_PLUGIN_NAME "] NoCheatZ Banlist has been set to off.\n");
		}
		else
		{
			xMsg("Unknown argument : ncz_banlists ncz \"%s\"\n", to_lowercase(tostring(RETRIEVE_ARGV(2))).c_str());
			engine->LogPrint("[" NCZ_PLUGIN_NAME "] Syntaxe : ncz_banlists ncz \"action\"\n");
			engine->LogPrint("[" NCZ_PLUGIN_NAME "] action = \"on\" -> Use external banlisting for NoCheatZ Banlist.\n");
			engine->LogPrint("[" NCZ_PLUGIN_NAME "] action = \"off\" -> Don't use external banlisting for NoCheatZ Banlist.\n");
		}

	}
	else if (vcomp(RETRIEVE_ARGV(1), "esl"))
	{
		if (vcomp(RETRIEVE_ARGV(2), "on"))
		{
			use_esl_banlists = true;
			engine->LogPrint("[" NCZ_PLUGIN_NAME "] Electronic Sports League Banlist has been set to on.\n");
		}
		else if (vcomp(RETRIEVE_ARGV(2), "off"))
		{
			use_esl_banlists = false;
			engine->LogPrint("[" NCZ_PLUGIN_NAME "] Electronic Sports League Banlist has been set to off.\n");
		}
		else
		{
			xMsg("Unknown argument : ncz_banlists esl \"%s\"\n", to_lowercase(tostring(RETRIEVE_ARGV(2))).c_str());
			engine->LogPrint("[" NCZ_PLUGIN_NAME "] Syntaxe : ncz_banlists esl \"action\"\n");
			engine->LogPrint("[" NCZ_PLUGIN_NAME "] action = \"on\" -> Use external banlisting for Electronic Sports League Banlist.\n");
			engine->LogPrint("[" NCZ_PLUGIN_NAME "] action = \"off\" -> Don't use external banlisting for Electronic Sports League Banlist.\n");
		}
	}
	else if (vcomp(RETRIEVE_ARGV(1), "esea"))
	{
		if (vcomp(RETRIEVE_ARGV(2), "on"))
		{
			use_esea_banlists = true;
			engine->LogPrint("[" NCZ_PLUGIN_NAME "] ESEA Banlist has been set to on.\n");
		}
		else if (vcomp(RETRIEVE_ARGV(2), "off"))
		{
			use_esea_banlists = false;
			engine->LogPrint("[" NCZ_PLUGIN_NAME "] ESEA Banlist has been set to off.\n");
		}
		else
		{
			xMsg("Unknown argument : ncz_banlists esea \"%s\"\n", to_lowercase(tostring(RETRIEVE_ARGV(2))).c_str());
			engine->LogPrint("[" NCZ_PLUGIN_NAME "] Syntaxe : ncz_banlists esea \"action\"\n");
			engine->LogPrint("[" NCZ_PLUGIN_NAME "] action = \"on\" -> Use external banlisting for ESEA Banlist.\n");
			engine->LogPrint("[" NCZ_PLUGIN_NAME "] action = \"off\" -> Don't use external banlisting for ESEA Banlist.\n");
		}
	}
	else if (vcomp(RETRIEVE_ARGV(1), "lgz"))
	{
		if (vcomp(RETRIEVE_ARGV(2), "on"))
		{
			use_lgz_banlists = true;
			engine->LogPrint("[" NCZ_PLUGIN_NAME "] LeetGamerZ.net Banlist has been set to on.\n");
		}
		else if (vcomp(RETRIEVE_ARGV(2), "off"))
		{
			use_lgz_banlists = false;
			engine->LogPrint("[" NCZ_PLUGIN_NAME "] LeetGamerZ.net Banlist has been set to off.\n");
		}
		else
		{
			xMsg("Unknown argument : ncz_banlists lgz \"%s\"\n", to_lowercase(tostring(RETRIEVE_ARGV(2))).c_str());
			engine->LogPrint("[" NCZ_PLUGIN_NAME "] Syntaxe : ncz_banlists lgz \"action\"\n");
			engine->LogPrint("[" NCZ_PLUGIN_NAME "] action = \"on\" -> Use external banlisting for LeetGamerZ.net Banlist.\n");
			engine->LogPrint("[" NCZ_PLUGIN_NAME "] action = \"off\" -> Don't use external banlisting for LeetGamerZ.net Banlist.\n");
		}
	}
	else if (vcomp(RETRIEVE_ARGV(1), "eac"))
	{
		if (vcomp(RETRIEVE_ARGV(2), "on"))
		{
			use_eac_banlists = true;
			engine->LogPrint("[" NCZ_PLUGIN_NAME "] Easy Anti Cheat Banlist has been set to on.\n");
		}
		else if (vcomp(RETRIEVE_ARGV(2), "off"))
		{
			use_eac_banlists = false;
			engine->LogPrint("[" NCZ_PLUGIN_NAME "] Easy Anti Cheat Banlist has been set to off.\n");
		}
		else
		{
			xMsg("Unknown argument : ncz_banlists eac \"%s\"\n", to_lowercase(tostring(RETRIEVE_ARGV(2))).c_str());
			engine->LogPrint("[" NCZ_PLUGIN_NAME "] Syntaxe : ncz_banlists eac \"action\"\n");
			engine->LogPrint("[" NCZ_PLUGIN_NAME "] action = \"on\" -> Use external banlisting for Easy Anti Cheat Banlist.\n");
			engine->LogPrint("[" NCZ_PLUGIN_NAME "] action = \"off\" -> Don't use external banlisting for Easy Anti Cheat Banlist.\n");
		}
	}
	else
	{
		xMsg("Unknown argument : ncz_banlists \"%s\"\n", to_lowercase(tostring(RETRIEVE_ARGV(1))).c_str());
		engine->LogPrint("[" NCZ_PLUGIN_NAME "] Syntaxe : ncz_banlists \"action\"   or   \"banlist\" \"action\" \n");
		engine->LogPrint("[" NCZ_PLUGIN_NAME "] action = \"on\" -> Use external banlisting (NoCheatZ Master banlist and LeetGamerZ Secure Source Banlist).\n");
		engine->LogPrint("[" NCZ_PLUGIN_NAME "] action = \"off\" -> Don't use external banlisting.\n");
		engine->LogPrint("[" NCZ_PLUGIN_NAME "] banlist = { \"ncz\", \"lgz\", \"esl\", \"esea\", \"eac\" }.\n");
	}
}
/*
CON_COMMAND(ncz_engine_status, "Print NoCheatZ 3 Engine Status.")
{
engine->LogPrint("[" NCZ_PLUGIN_NAME "] NoCheatZ Engine Status :\n");
xMsg("Tickrate : %s\n", tostring(tickrate).c_str()).c_str());
xMsg("Current Tick : %s\n", tostring(n_tick).c_str()).c_str());
int percent = int ((lastGameFrameExecTime * 100.0) / (maxExecTime * 5.0));
xMsg("NoCheatZ::GameFrame : Max Exec Time : %s, Last Exec Time : %s, Usage Percent : %s.\n", tostring(maxExecTime).c_str(), tostring(lastGameFrameExecTime).c_str(), tostring(percent).c_str()).c_str());
}*/

CON_COMMAND(ncz_check_aimbots, "")
{
	if (vcomp(RETRIEVE_ARGV(1), "on"))
	{
		check_aimbots = true;
		engine->LogPrint("[" NCZ_PLUGIN_NAME "] ncz_check_aimbots has been set to on.\n");
	}
	else if (vcomp(RETRIEVE_ARGV(1), "off"))
	{
		check_aimbots = false;
#ifdef NCZ_CSS
		UnhookCmd();
#endif
		engine->LogPrint("[" NCZ_PLUGIN_NAME "] ncz_check_aimbots has been set to off.\n");
	}
	else if (vcomp(RETRIEVE_ARGV(1), "system"))
	{
		//aimbot_system_number = 4;
		if(atoi(RETRIEVE_ARGV(2))) 
		{
			aimbot_system_number = atoi(RETRIEVE_ARGV(2));
			//printf("%d\n", aimbot_system_number);
			switch(aimbot_system_number)
			{
			case 1:
				xMsg("ncz_check_aimbots : Using system 1.\n");
				break;

			case 2:
#ifdef NCZ_CSS
				xMsg("ncz_check_aimbots : Using system 2.\n");
				//xMsg("ncz_check_aimbots : Using system 4 & 2 & 1 (Detect & Block).\n");
				HookCmd(NULL);
#else
				xMsg("ncz_check_aimbots : System 4 only exists with CS:S. Using system 1 (Detect) instead ...\n");
				engine->ServerCommand("ncz_check_aimbots system 1\n");
#endif
				break;
				
			default:
				xMsg("ncz_check_aimbots : Unknown system number \"%s\".\n", RETRIEVE_ARGV(2));
#ifdef NCZ_CSS
				engine->ServerCommand("ncz_check_aimbots system 1\n");
#else
				engine->ServerCommand("ncz_check_aimbots system 1\n");
#endif
				break;
			}
		}
		else
		{
			xMsg("ncz_check_aimbots : Unknown system number \"%s\".\n", RETRIEVE_ARGV(2));
#ifdef NCZ_CSS
			engine->ServerCommand("ncz_check_aimbots system 1\n");
#else
			engine->ServerCommand("ncz_check_aimbots system 1\n");
#endif
		}
		//aimbot_system_number = 1;
	}
	else
	{
		xMsg("Unknown argument : ncz_check_aimbots \"%s\"\n", to_lowercase(tostring(RETRIEVE_ARGV(1))).c_str());
		engine->LogPrint("[" NCZ_PLUGIN_NAME "] Syntaxe : ncz_check_aimbots \"action\" [args]\n");
		engine->LogPrint("[" NCZ_PLUGIN_NAME "] action = \"on\" -> Check for aimbots.\n");
		engine->LogPrint("[" NCZ_PLUGIN_NAME "] action = \"off\" -> Don't check for aimbots.\n");
	}
}

CON_COMMAND(ncz_check_triggerbots, "")
{
	if (vcomp(RETRIEVE_ARGV(1), "on"))
	{
		check_triggerbot = true;
		engine->LogPrint("[" NCZ_PLUGIN_NAME "] ncz_check_triggerbots has been set to on.\n");
	}
	else if (vcomp(RETRIEVE_ARGV(1), "off"))
	{
		check_triggerbot = false;
		engine->LogPrint("[" NCZ_PLUGIN_NAME "] ncz_check_triggerbots has been set to off.\n");
	}
	else
	{
		xMsg("Unknown argument : ncz_check_triggerbots \"%s\"\n", to_lowercase(tostring(RETRIEVE_ARGV(1))).c_str());
		engine->LogPrint("[" NCZ_PLUGIN_NAME "] Syntaxe : ncz_check_triggerbots \"action\" [args]\n");
		engine->LogPrint("[" NCZ_PLUGIN_NAME "] action = \"on\" -> Check for triggerbots.\n");
		engine->LogPrint("[" NCZ_PLUGIN_NAME "] action = \"off\" -> Don't check for triggerbots.\n");
	}
}

#if !defined(NCZ_CSGO) && !defined(NCZ_CSP)
CON_COMMAND(ncz_check_bunny_scripts, "")
{
	if (vcomp(RETRIEVE_ARGV(1), "on"))
	{
		check_bunny = true;
		engine->LogPrint("[" NCZ_PLUGIN_NAME "] ncz_check_bunny_scripts has been set to on.\n");
	}
	else if (vcomp(RETRIEVE_ARGV(1), "off"))
	{
		check_bunny = false;
		engine->LogPrint("[" NCZ_PLUGIN_NAME "] ncz_check_bunny_scripts has been set to off.\n");
	}
	else
	{
		xMsg("Unknown argument : ncz_check_bunny_scripts \"%s\"\n", to_lowercase(tostring(RETRIEVE_ARGV(1))).c_str());
		engine->LogPrint("[" NCZ_PLUGIN_NAME "] Syntaxe : ncz_check_bunny_scripts \"action\" [args]\n");
		engine->LogPrint("[" NCZ_PLUGIN_NAME "] action = \"on\" -> Check for bunny scripts.\n");
		engine->LogPrint("[" NCZ_PLUGIN_NAME "] action = \"off\" -> Don't check for bunny scripts.\n");
	}
}
#endif

CON_COMMAND(ncz_check_client_vars, "")
{
	int cArgC = 0;
	cArgC = RETRIEVE_ARGC - 1;
	std::string c_logtext = "";
	if (vcomp(RETRIEVE_ARGV(1), "on"))
	{
		check_client_vars = TRUE;
		engine->LogPrint("[" NCZ_PLUGIN_NAME "] Anti Client ConVar Bypass Activated.\n");
	}
	else if (vcomp(RETRIEVE_ARGV(1), "off"))
	{
		check_client_vars = FALSE;
		engine->LogPrint("[" NCZ_PLUGIN_NAME "] Anti Client ConVar Bypass Disabled.\n");
	}
	else
	{
		if (!cArgC == 0)
		{
			engine->LogPrint("[" NCZ_PLUGIN_NAME "] Unknown argument for ncz_check_client_vars.\n");
		}
		engine->LogPrint("Syntaxe : ncz_check_client_vars \"action\" [args]\naction = \"print\" -> Print all client cvars to check.\naction = \"on\" -> Check for bypassed client cvars.\naction = \"off\" -> Don't check for bypassed client cvars.\n");
	}
}

// -----------------------------------------------------------
//                           CLASSES
// -----------------------------------------------------------

// ##########################################
// Cg_NCZ_PlayerManager
// ##########################################
Cg_NCZ_PlayerManager::Cg_NCZ_PlayerManager()
{
	g_ncz_playerList.clear();
	for(int i=0; i<=64; i++)
	{
		g_ncz_playerList.push_back(NULL);
	}
}
Cg_NCZ_PlayerManager::~Cg_NCZ_PlayerManager()
{
	for(int i=0; i<=64; i++)
	{
		delete g_ncz_playerList[i];
	}
	g_ncz_playerList.clear();
}

int Cg_NCZ_PlayerManager::getPlayerCount()
{
	return GetPlayerCount();
}

int Cg_NCZ_PlayerManager::getMaxClients()
{
	if(playerinfomanager)
	{
		if(playerinfomanager) return playerinfomanager->GetGlobalVars()->maxClients;
		else return 0;
	}
	else return 0;
}

Cg_NCZ_Player* Cg_NCZ_PlayerManager::getPlayer(int index)
{
	if(index > 0)
	{
		Cg_NCZ_Player * MyPlayer = g_ncz_playerList.at((unsigned int)index);
		if(MyPlayer == NULL) g_ncz_playerList.at((unsigned int)index) = new Cg_NCZ_Player((int)index);
		return g_ncz_playerList.at((unsigned int)index);
	}
	else return NULL;
}

Cg_NCZ_Player* Cg_NCZ_PlayerManager::getPlayer(edict_t *pPlayer)
{
	return this->getPlayer(IndexOfEdict(pPlayer));
}

Cg_NCZ_Player* Cg_NCZ_PlayerManager::getPlayer(void * myvoid)
{
	for(int i=1; i<=this->getMaxClients(); i++)
	{
		/*//if( reinterpret_cast<void *>(this->getPlayer(i)->getEdict()->GetUnknown()->GetBaseEntity()) == myvoid) return this->getPlayer(i);
		Cg_NCZ_Player *MyPlayer = this->getPlayer(i);
		if(MyPlayer)
		{
			edict_t *MyEdict = MyPlayer->getEdict();
			if(MyEdict)
			{
				IServerUnknown *MyUnknown = MyEdict->GetUnknown();
				if(MyUnknown)
				{
					void *ThisVoid = reinterpret_cast<void *>(MyUnknown->GetBaseEntity());
					if(ThisVoid == myvoid)
					{
						// Assume this function is always used by a system which wants to observe real players only.
						IPlayerInfo *MyInfo = MyPlayer->getPlayerInfo();
						if(MyInfo)
						{
							if( 
								  MyInfo->IsConnected() && 
								  MyInfo->IsPlayer() && 
								! MyInfo->IsObserver() && 
								! MyInfo->IsDead() && 
								! MyInfo->IsFakeClient() && 
								! MyInfo->IsHLTV()
								  //MyPlayer->isCheckable()
								) return MyPlayer;
						}
					}
				}
			}
		}*/
		edict_t *MyEdict = PEntityOfEntIndex(i);
		if(MyEdict)
		{
			if(isValidEdict(MyEdict))
			{
				IServerUnknown *MyUnknown = MyEdict->GetUnknown();
				if(MyUnknown)
				{
					void *ThisVoid = reinterpret_cast<void *>(MyUnknown->GetBaseEntity());
					if(ThisVoid == myvoid)
					{
						return getPlayer(MyEdict);
					}
				}
			}
		}
	}
	return NULL;
}

void Cg_NCZ_PlayerManager::lockAll()
{
	for(int i=1; i<=this->getMaxClients(); i++)
	{
		this->getPlayer(i)->eat_lock();
	}
}

void Cg_NCZ_PlayerManager::resetAll()
{
}

void Cg_NCZ_PlayerManager::resetAllUkCmd()
{
	for(int i=1; i<=this->getMaxClients(); i++)
	{
		this->getPlayer(i)->resetUkCmd();
	}
}

void Cg_NCZ_PlayerManager::checkAllAimDirChange()
{
	for(int i=1; i<=this->getMaxClients(); i++)
	{
		this->getPlayer(i)->checkAimDirChange();
	}
}

#if !defined(NCZ_CSGO) && !defined(NCZ_CSP)
void Cg_NCZ_PlayerManager::checkAllBunny()
{
	for(int i=1; i<=this->getMaxClients(); i++)
	{
		this->getPlayer(i)->testBunny();
		//}
	}
}
#endif

void Cg_NCZ_PlayerManager::checkAllUkCmd()
{
	if(UkCmdCheck <= GetTime())
	{
		UkCmdCheck = GetTime() + 1.0f;
		for(int i=1; i<=this->getMaxClients(); i++)
		{
			Cg_NCZ_Player *MyPlayer = this->getPlayer(i);
			if(MyPlayer)
			{
				if(MyPlayer->getUkCmdCount() > 10)
				{
					MyPlayer->addLog(GetTranslate("cmd_flood"));
					if(MyPlayer->getUkCmdCount() > 25)
					{
						MyPlayer->Ban(false, 240, "Banned by NoCheatZ 3");
					}
					else
					{
						MyPlayer->Kick(GetTranslate("cmd_flood").c_str());
					}
				}
				//}
			}
		}
		this->resetAllUkCmd();
	}
}

void Cg_NCZ_PlayerManager::checkAllFire()
{
	for(int i=1; i<=this->getMaxClients(); i++)
	{
		this->getPlayer(i)->updateFire();
	}
}

void Cg_NCZ_PlayerManager::checkAllEyeAngles()
{
	for(int i=1; i<=this->getMaxClients(); i++)
	{
		this->getPlayer(i)->testEyeAngles(NULL);
	}
}

void Cg_NCZ_PlayerManager::handleAllConvarTest()
{
	if(check_client_vars)
	{
		for(int i=1; i<=this->getMaxClients(); i++)
		{
			if(this->getPlayer(i)) this->getPlayer(i)->handleConvarTest();
		}
	} 
}

void Cg_NCZ_PlayerManager::fullcheckBanlists()
{
	if(use_banlists)
	{
		if(!banlists) banlists = new Cg_NCZ_Queue();
		if(banlists)
		{
			for(int i=1; i<=this->getMaxClients(); i++)
			{
				if(this->getPlayer(i)) this->getPlayer(i)->AddToBanlistCheck();
			}
		}
	} 
}

void Cg_NCZ_PlayerManager::checkAllBanRequest()
{
	for(int i=1; i<=this->getMaxClients(); i++)
	{
		edict_t *MyEdict = PEntityOfEntIndex(i);
		if(::isValidEdict(MyEdict))
		{
			IPlayerInfo *pInfo = playerinfomanager->GetPlayerInfo(MyEdict);
			if (pInfo)
			{
				if (pInfo->IsConnected() && !pInfo->IsFakeClient())
				{
					this->getPlayer(i)->check_ban_request();
					this->getPlayer(i)->resetNamesChanges();
				}
			}
		}
	}
}

void Cg_NCZ_PlayerManager::updateAll()
{
	for(int i=1; i<=this->getMaxClients(); i++)
	{
		this->getPlayer(i)->update();
	}
}

void Cg_NCZ_PlayerManager::disconnectPlayer(edict_t *pPlayer)
{
	this->disconnectPlayer(IndexOfEdict(pPlayer));
}

void Cg_NCZ_PlayerManager::disconnectPlayer(int index)
{
	delete g_ncz_playerList[index];
}

// ##########################################
// Cg_NCZ_Player
// ##########################################


Cg_NCZ_Player::Cg_NCZ_Player(const int index) :
cIndex(index),
	eat_x_lock(true),
	eat_y_lock(true),
	eat_z_lock(true),
	beax(false),
	beay(false),
	beaz(false),
	m_pitchReady(false),
	m_pitchValue("0"),
	ignoreFirst(true),
	last_lock_time(0),
	last_log_time_x(0.0),
	last_log_time_y(0.0),
	last_log_time_twice(0.0),
	last_tick_marked(0.0),
	last_trigger_add(0.0),
	lastfx(0.0),
	lastfy(0.0),
	lastdx(0),
	fy(0.0),
	fx(0.0),
	fz(0.0),
	total_badm(0),
	connectionTime(0),
	sub_badm(0),
	total_beax(0),
	total_bh(0),
	total_beay(0),
	total_beaz(0),
	total_cvb(0),
	total_rf(0),
	total_badv(0),
	lastAimVariation(1),
	xtrigger_count(0),
	ban_request_time(0.0),
	cmd_p_s(0),
	lastFireButtonPushedTick(0),
	lastFireButtonPushedTime(0.0),
	lastSpawnTime(0),
	lastVADetectionTime(0.0),
	last_auto_status(0.0),
	fireState(false),
	m_filterReady(false),
	EyeAnglesAlreadyChecked(false),
	worldHitTick(0),
	jumpTick(0),
	jump_cmd_count(0),
	jump_world_count(0),
	last_jump_cmd_state(false),
	lastJumpTick(0),
	m_filterValue("0"),
	groundState(false),
	recordingRIE(false),
	bunnyDetectCount(0),
	myDemoTicksCount(0),
#if !defined(NCZ_CSGO) && !defined(NCZ_CSP)
	lastBunnyReset(0.0),
#endif
	rfDetectCount(0),
	t_rfDetectCount(0),
	sub_badv(0),
	blockedCmd(0),
	apDetectCount(0),
	last_cvb_log(0.0),
	lastRfReset(0.0),
	speed(0.0),
	lastAimSpeed(0.0),
	consecutiveSpeedInfraction(0),
	lastSpeedInfraction(0.0),
	prepareTriggerbotDetection(false),
	lastGoodAngles(QAngle(0,0,0)),
	AimDir(QAngle(0,0,0)),
	lastHurtTime(0.0),
	notsureabout(true),
	lastImpactTime(0.0),
	lastHurtUserid(0),
	ytrigger_count(0),
	target(NULL),
	bads(""),
	lastCvarDetected(""),
	lastCvarTested(NULL),
	lastCvarTest(0),
	waitcvarresp(false),
	t_tbDetectCount(0),
	sugg("")
{
}

Cg_NCZ_Player::~Cg_NCZ_Player()
{
}

bool Cg_NCZ_Player::isFirering() const
{
	return fireState;
}

#if !defined(NCZ_CSGO) && !defined(NCZ_CSP)
bool Cg_NCZ_Player::isOnGround() const
{
	if(!(entpropsmanager->getPropValue<int>("CBasePlayer.localdata.m_hGroundEntity", this->getEdict()) == -1)) return false;
	else return true;
}
#endif

bool Cg_NCZ_Player::isInBuyZone() const
{
	return false;
}

#if !defined(NCZ_CSGO) && !defined(NCZ_CSP)
void Cg_NCZ_Player::testBunny()
{
	edict_t *pEdict = this->getEdict();
	if(this->isCheckable())
	{
		IPlayerInfo * myInfo = this->getPlayerInfo();
		CBotCmd cmd = myInfo->GetLastUserCommand();
		if(last_jump_cmd_state == false && cmd.buttons & IN_JUMP)
		{
			last_jump_cmd_state = true;
			++jump_cmd_count;
		}
		else if(last_jump_cmd_state == true && cmd.buttons | IN_JUMP)
		{
			last_jump_cmd_state = false;
		}
		int tgroundstate = entpropsmanager->getPropValue<int>("CBasePlayer.localdata.m_hGroundEntity", pEdict);
		if(!(tgroundstate == -1)) // Au sol
		{
			if(!groundState) // En l'air
			{
				groundState = true;
				worldHitTick = gpGlobals->tickcount;
			}
		}
		if(tgroundstate == -1)
		{
			if(groundState)
			{
				groundState = false;
				lastJumpTick = jumpTick;
				++jump_world_count;
				jumpTick = gpGlobals->tickcount;
				int diff = abs(jumpTick - worldHitTick);
				if(diff < BHOP && (jumpTick - lastJumpTick > 20))
				{
					int iBhops = ++g_cBhops;
					int tiPercents = (g_cPerfectBhops * 100) / g_cBhops;
					//Msg("BHOP Total %d, PerfectBhop %d, PercentPerfect %d / 90, GoodBhop %d\n", g_cBhops, g_cPerfectBhops, tiPercents, g_cGoodBhops);
					if(diff < 2)
					{
						++g_cPerfectBhops;
					}
					else if(diff < GOOD_BHOP)
					{
						++g_cGoodBhops;
					}
   
					if(  (  ( iBhops % BHOPS_CHECK_FREQ ) == 0 ) &&  ( iBhops > BHOPS_CHECK_FREQ)  )
					{
						iBhops = g_cBhops;
						int iPerfectBhops = g_cPerfectBhops;
						int iPercents = (iPerfectBhops * 100) / iBhops;
						if(jump_cmd_count > 10*jump_world_count && iPerfectBhops > 0)
						{
							++total_bh;
							this->addLog("BunnyHop Script (CFG) " + GetTranslate("Detected"));
							edict_t * pTV = getSourceTV();
							if(pTV)
							{
								::tell(pTV, format("\1[\004" NCZ_PLUGIN_NAME "\001] %s [\003%s\001] : BunnyHop Script (CFG) %s.\n", this->getName(), this->getSteamID(), GetTranslate("Detected").c_str()));
							}
							//std::string tolog = "";
							//tolog  = NCZ_REPORT_FIRST_LINE
							//tolog += format("%s [%s] : BunnyHop Script (CFG) Detected (Perfect Bhop Percent : %d).\n", this->getName(), this->getSteamID(), iPercents);
							//tolog += "Debug : Weapon, Time Connected,  Kills/Deaths, Total Detections Count)\n";
							//tolog += format("------> (%s, %s, %s/%s, %d)\n", this->getWeapon(), formatSeconds(this->getTimeConnected()).c_str(), tostring(this->getPlayerInfo()->GetFragCount()).c_str(), tostring(this->getPlayerInfo()->GetDeathCount()).c_str(), this->getTotalDetectionsCount());
							//queue_send_report->send(format("i=%s&l=%s&aab=0", this->getSteamID(), tolog.c_str()));
							this->Kick("You have to turn off your BunnyHop Script to play on this server.");
						}
						if(iPercents >= MIN_ADVERTISE_PERCENT)
						{
							if( iPercents > MIN_PERCENTS_FOR_BAN )
							{
								if( iBhops >= MIN_PERFBHOPS_FOR_BAN)
								{
									if(jump_cmd_count < 3*jump_world_count)
									{
										++total_bh;
										this->addLog("BunnyHop Cheat (Injected) " + GetTranslate("Detected"));
										edict_t * pTV = getSourceTV();
										if(pTV)
										{
											::tell(pTV, format("\1[\004" NCZ_PLUGIN_NAME "\001] %s [\003%s\001] : BunnyHop Cheat (Injected) %s.\n", this->getName(), this->getSteamID(), GetTranslate("Detected").c_str()));
										}
										std::string tolog = "";
										tolog  = NCZ_REPORT_FIRST_LINE
										tolog += format("%s [%s] : BunnyHop Cheat (Injected) Detected (Perfect Bhop Percent : %d).\n", this->getName(), this->getSteamID(), iPercents);
										tolog += format("JCC : %d, JWC : %d, GCB : %d.\n", jump_cmd_count, jump_world_count, g_cBhops);
										tolog += "Debug : Weapon, Time Connected,  Kills/Deaths, Total Detections Count)\n";
										tolog += format("------> (%s, %s, %s/%s, %d)\n", this->getWeapon(), formatSeconds(this->getTimeConnected()).c_str(), tostring(this->getPlayerInfo()->GetFragCount()).c_str(), tostring(this->getPlayerInfo()->GetDeathCount()).c_str(), this->getTotalDetectionsCount());
										queue_send_report->send(format("i=%s&l=%s&aab=1", this->getSteamID(), tolog.c_str()));
										this->Ban(true, 0, "Banned by NoCheatZ 3 (Injected BunnyHop Cheat)");
									}
									g_cBhops = 0;
									g_cPerfectBhops = 0;
									g_cGoodBhops = 0;
								 }
							}
						}
					}
				}
			}
		}
	}
}
#endif

void Cg_NCZ_Player::updateFire(CUserCmd *pCmd)
{
	if(this->isCheckable())
	{
		int buttons;
		if(!pCmd)
		{
			IPlayerInfo *pInfo = this->getPlayerInfo();
			if(pInfo)
			{
				CBotCmd lastcmd = pInfo->GetLastUserCommand();
				buttons = lastcmd.buttons;
			}
			else buttons = 0;
		}
		else buttons = pCmd->buttons;

		if(buttons & IN_ATTACK)
		{
			if(!this->isFirering())
			{
				fireState = true;
				lastFireButtonPushedTick = gpGlobals->tickcount;
				lastFireButtonPushedTime = GetTime();
				target = aimingAt();
				if(target) prepareTriggerbotDetection = true;
				else prepareTriggerbotDetection = false;
			}
		}
		else
		{
			if(this->isFirering())
			{
				fireState = false;
				float buttonReleaseTime = GetTime() - lastFireButtonPushedTime;
				if((gpGlobals->tickcount - lastFireButtonPushedTick) <= 1 && (tickrate > 60) && (buttonReleaseTime > 0.00f))
				{
					if(prepareTriggerbotDetection && target)
					{
						t_tbDetectCount ++;
						//if(t_rfDetectCount > 1)
						//{
							//std::string tolog = "";
							//tolog  = NCZ_REPORT_FIRST_LINE
							//tolog += format("%s [%s] : %s.\n", this->getName(), this->getSteamID(), format("%s TriggerBot (Fire Release Time : %1.4f s / Aiming at %s [%s])", GetTranslate("probably_using").c_str(), buttonReleaseTime, target->getName(), target->getSteamID()).c_str());
							//tolog += "Debug : Weapon, Time Connected,  Kills/Deaths, Total Detections Count)\n";
							//tolog += format("------> (%s, %s, %s/%s, %d)\n", this->getWeapon(), formatSeconds(this->getTimeConnected()).c_str(), tostring(this->getPlayerInfo()->GetFragCount()).c_str(), tostring(this->getPlayerInfo()->GetDeathCount()).c_str(), this->getTotalDetectionsCount());
							//queue_send_report->send(format("i=%s&l=%s&aab=0&ah=1", this->getSteamID(), tolog.c_str()));
							edict_t * pTV = getSourceTV();
							if(pTV)
							{
								::tell(pTV, format("\1[\004" NCZ_PLUGIN_NAME "\001] %s [\003%s\001] : %s TriggerBot.\n", this->getName(), this->getSteamID(), GetTranslate("probably_using").c_str()));
							}
						//}
					} 
					else
					{
							t_rfDetectCount ++;
							if(t_rfDetectCount > 30)
							{
								t_rfDetectCount = 0;
								total_rf ++;
								if(total_rf > 6)
								{
									this->addLog("AutoPistol " + GetTranslate("Detected"));
									std::string tolog = "";
									tolog  = NCZ_REPORT_FIRST_LINE
									tolog += format("%s [%s] : %s.\n", this->getName(), this->getSteamID(), "AutoPistol Detected");
									tolog + format("TTB : %d, TRF : %d\n", t_tbDetectCount, total_rf);
									tolog += "Debug : Weapon, Time Connected,  Kills/Deaths, Total Detections Count)\n";
									tolog += format("------> (%s, %s, %s/%s, %d)\n", this->getWeapon(), formatSeconds(this->getTimeConnected()).c_str(), tostring(this->getPlayerInfo()->GetFragCount()).c_str(), tostring(this->getPlayerInfo()->GetDeathCount()).c_str(), this->getTotalDetectionsCount());
									queue_send_report->send(format("i=%s&l=%s", this->getSteamID(), tolog.c_str()));
									noTell(this->getEdict(), format("\1[\004" NCZ_PLUGIN_NAME "\001] %s [\003%s\001] -> TriggerBot %s.", this->getName(), this->getSteamID(), GetTranslate("Detected").c_str()).c_str());
								
									this->Ban(true, 0, "Banned by NoCheatZ 3");
									//rfDetectCount = 0;
								}
							}
							else
							{
								//std::string tolog = "";
								//tolog  = NCZ_REPORT_FIRST_LINE
								//tolog += format("%s [%s] : %s.\n", this->getName(), this->getSteamID(), format("%s AutoPistol (Fire Release Time : %1.4f s)", GetTranslate("probably_using").c_str(), buttonReleaseTime).c_str());
								//tolog += "Debug : Weapon, Time Connected,  Kills/Deaths, Total Detections Count)\n";
								//tolog += format("------> (%s, %s, %s/%s, %d)\n", this->getWeapon(), formatSeconds(this->getTimeConnected()).c_str(), tostring(this->getPlayerInfo()->GetFragCount()).c_str(), tostring(this->getPlayerInfo()->GetDeathCount()).c_str(), this->getTotalDetectionsCount());
								//queue_send_report->send(format("i=%s&l=%s&aab=0&ah=1", this->getSteamID(), tolog.c_str()));
								edict_t * pTV = getSourceTV();
								if(pTV)
								{
									::tell(pTV, format("\1[\004" NCZ_PLUGIN_NAME "\001] %s [\003%s\001] : %s AutoPistol.\n", this->getName(), this->getSteamID(), GetTranslate("probably_using").c_str()));
								}
							}
					}
				}
				else
				{
					prepareTriggerbotDetection = false;
					target = NULL;
				}
			}
		}
		if(t_tbDetectCount > 6 && total_rf > 1)
		{
			rfDetectCount ++;
			
			this->addLog("TriggerBot" + GetTranslate("Detected"));
			std::string tolog = "";
			tolog  = NCZ_REPORT_FIRST_LINE
			tolog += format("%s [%s] : %s.\n", this->getName(), this->getSteamID(), "TriggerBot Detected");
			tolog + format("TTB : %d, TRF : %d\n", t_tbDetectCount, total_rf);
			tolog += "Debug : Weapon, Time Connected,  Kills/Deaths, Total Detections Count)\n";
			tolog += format("------> (%s, %s, %s/%s, %d)\n", this->getWeapon(), formatSeconds(this->getTimeConnected()).c_str(), tostring(this->getPlayerInfo()->GetFragCount()).c_str(), tostring(this->getPlayerInfo()->GetDeathCount()).c_str(), this->getTotalDetectionsCount());
			queue_send_report->send(format("i=%s&l=%s", this->getSteamID(), tolog.c_str()));
			noTell(this->getEdict(), format("\1[\004" NCZ_PLUGIN_NAME "\001] %s [\003%s\001] -> TriggerBot %s.", this->getName(), this->getSteamID(), GetTranslate("Detected").c_str()).c_str());
			//total_rf ++;
			t_tbDetectCount = 0;
			this->Ban(true, 0, "Banned by NoCheatZ 3");
			rfDetectCount = 0;
		}
	}
	EyeAnglesAlreadyChecked = false;
}

Cg_NCZ_Player* Cg_NCZ_Player::aimingAt()
{
	trace_t trace;
	Ray_t ray;

	edict_t* edict = this->getEdict();
	if ( !edict ) return NULL;
	IPlayerInfo* playerinfo = this->getPlayerInfo();
	if ( !playerinfo ) return NULL;
	CBotCmd cmd = playerinfo->GetLastUserCommand();

	Vector earPos;
	gameclients->ClientEarPosition(edict, &earPos);
	Vector eyePos = earPos;

	QAngle eyeAngles = cmd.viewangles;
	Vector vEnd;
	AngleVectors(eyeAngles, &vEnd);
	vEnd = vEnd * 8192.0f + eyePos;
	
	ray.Init(eyePos,vEnd);
	enginetrace->TraceRay( ray, (CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_MONSTER | CONTENTS_DEBRIS | CONTENTS_HITBOX), NULL, &trace );
	
	edict_t* target = gameents->BaseEntityToEdict(trace.m_pEnt);
	if ( target && !IndexOfEdict(target) == 0 && !trace.allsolid )
	{
		if(!::isValidEdict(target)) return NULL;
#undef GetClassName
		if(strcmp(target->GetClassName(), "player") == 0)
		{
			IPlayerInfo* targetinfo = playerinfomanager->GetPlayerInfo(target);
			if(targetinfo)
			{
				int ta = targetinfo->GetTeamIndex();
				int tb = playerinfo->GetTeamIndex();
				if( ta != tb )
				{
					if( targetinfo->IsPlayer() && !targetinfo->IsHLTV() && !targetinfo->IsObserver() )
					{
						return g_NoCheatZPlugin.get_m_pm()->getPlayer(target);
					}
				} 
			}
		}
	}
	return NULL;
}

void Cg_NCZ_Player::resetPlayer()
{
	total_beax = total_beay = total_bh = total_beaz = total_cvb = total_rf = connectionTime = bunnyDetectCount = rfDetectCount = t_rfDetectCount = apDetectCount = consecutiveSpeedInfraction = lastHurtUserid = 0;
	blockedCmd = sub_badv = total_badv = lastFireButtonPushedTick = worldHitTick = jumpTick = lastJumpTick =  myDemoTicksCount = ytrigger_count = xtrigger_count = total_badm = sub_badm = 0;
	lastAimSpeed = lastFireButtonPushedTime = fx = fy = lastVADetectionTime = lastRfReset = speed = lastSpeedInfraction = lastHurtTime = lastImpactTime = lastfy = lastfx = last_trigger_add = 0.0f;
	beax = beay = beaz = groundState = fireState = prepareTriggerbotDetection = recordingRIE = m_pitchReady = false;
#if !defined(NCZ_CSGO) && !defined(NCZ_CSP)
	lastBunnyReset = 0.0f;
#endif
	t_tbDetectCount = 0;
	ignoreFirst = true;
	m_filterReady = waitcvarresp = false;
	notsureabout = true;
	bads = sugg = "";
	resetNamesChanges();
	g_cGroundFrames = 0;
	g_cPerfectBhops = 0;
	g_cGoodBhops = 0;
	g_cBhops = 0;
	lastdx = 0;
	lastGoodAngles = QAngle(0,0,0);
	jump_cmd_count = 0;
	jump_world_count = 0;
	last_jump_cmd_state = false;
	last_cvb_log = 0.0f;
	lastCvarDetected = "";
	lastCvarTested = NULL;
	lastCvarTest = 0;
}

const char * Cg_NCZ_Player::getName() const
{
	IPlayerInfo *MyPlayerInfo = this->getPlayerInfo();
	if (MyPlayerInfo)
	{
		return MyPlayerInfo->GetName();
	}
	else
	{
		return "";
	}
}

INetChannelInfo* Cg_NCZ_Player::getChannelInfo() const
{
	return engine->GetPlayerNetInfo(this->getIndex());
}

INetChannel* Cg_NCZ_Player::getChannel()
{
	INetChannel* netChan = dynamic_cast<INetChannel *>(engine->GetPlayerNetInfo(this->getIndex()));
	if(netChan)	return netChan;
	return NULL;
}

const char * Cg_NCZ_Player::getIPAddress() const
{
	return this->getChannelInfo()->GetAddress();
}

const char * Cg_NCZ_Player::getWeapon() const
{
	return this->getPlayerInfo()->GetWeaponName();
}

bool Cg_NCZ_Player::isValidEdict() const
{
	return this->getEdict() != NULL && !this->getEdict()->IsFree();
}

int Cg_NCZ_Player::getUserid() const
{
	return engine->GetPlayerUserId(this->getEdict());
}

edict_t * Cg_NCZ_Player::getEdict() const
{
	return PEntityOfEntIndex(this->getIndex());
}

IPlayerInfo *Cg_NCZ_Player::getPlayerInfo() const
{
	if (this->isValidEdict())
	{
		if(playerinfomanager)
		{
			IPlayerInfo *MyPlayerInfo = playerinfomanager->GetPlayerInfo(this->getEdict());
			if(MyPlayerInfo)
			{
				return MyPlayerInfo;
			}
		}
	}
	return NULL;
}

int Cg_NCZ_Player::getIndex() const
{
	return cIndex;
}

int Cg_NCZ_Player::getTimeConnected() const
{
	return (int)((int)(GetTime() + 0.00000001) - connectionTime);
}

const char * Cg_NCZ_Player::getSteamID() const
{
	return engine->GetPlayerNetworkIDString(this->getEdict());
}

bool Cg_NCZ_Player::isValidPlayer()
{
	if(this->cIndex > 0)
	{
		IPlayerInfo *pInfo = this->getPlayerInfo();
		const char *SteamId = this->getSteamID();
		if (pInfo)
		{
			if (pInfo->IsConnected() && !pInfo->IsFakeClient() && !pInfo->IsObserver() && !pInfo->IsInAVehicle() && !pInfo->IsHLTV() && !pInfo->IsDead())
			{
					if(!(strcmp(SteamId, "") == 0 || SteamId[0] == 'B' || SteamId[6] == 'I'))
					{
						return true;
					}
			}
			else
			{
				lastfx = pInfo->GetLastUserCommand().viewangles.x;
				lastfy = pInfo->GetLastUserCommand().viewangles.y;
			}
		}
	}
	return false;
}

bool Cg_NCZ_Player::isCheckable()
{
	if (this->isValidPlayer())
	{
		IPlayerInfo *pInfo = this->getPlayerInfo();
		if(pInfo)
		{
			if (!(pInfo->IsDead()) && (pInfo->GetTeamIndex() > 1) && !(pInfo->IsInAVehicle()) && !(eat_x_lock) && !(eat_y_lock))
			{
					return true;
			}
		}
	}
	return false; 
}

bool Cg_NCZ_Player::isRealPlayer()
{
	const char * SteamID = this->getSteamID();
	IPlayerInfo *pInfo = this->getPlayerInfo();
	if(pInfo)
	{
		if (SteamID && this->isValidEdict() && !pInfo->IsFakeClient() && pInfo->IsPlayer() && !pInfo->IsHLTV())
		{
			return !(SteamID[0] == 'B' || SteamID[6] == 'I' || strcmp(SteamID, "") == 0);
		}
	}
	return false;
}

void Cg_NCZ_Player::setLastSpawnTime()
{
	lastSpawnTime = gpGlobals->tickcount;
}

void Cg_NCZ_Player::markTick()
{
	if(last_tick_marked + 3.0f <= GetTime())
	{
		if(recording || recordingRIE)
		{
			last_tick_marked = GetTime();
			edict_t *pTV = getSourceTV();
			if(pTV)
			{
				::tell(pTV, format("\1[\004" NCZ_PLUGIN_NAME "\001] %s [\003%s\001] have marked the tick number \003%s\001 on this demo.\n", this->getName(), this->getSteamID(), tostring(demoticks).c_str()).c_str());
			}
			//contell(getSourceTV(), format("[" NCZ_PLUGIN_NAME "] %s [%s] has marked the tick number %s on this demo.\n", this->getName(), this->getSteamID(), tostring(demoticks).c_str()).c_str());
			if(demoticks) contell(this->getEdict(), format("[" NCZ_PLUGIN_NAME "] You have marked the tick number %s on the demo %s.dem of this server.\n", tostring(demoticks).c_str(), recordFilename.c_str()).c_str());
			if(myDemoTicksCount && recordingRIE) contell(this->getEdict(), format("[" NCZ_PLUGIN_NAME "] You have marked the tick number %s on the demo you are actually recording.\n", tostring(myDemoTicksCount).c_str()).c_str());
			writeToLogfile(format("%s [%s] have marked the tick number %s on the demo.", this->getName(), this->getSteamID(), tostring(demoticks).c_str()));
			Msg("%s [%s] has marked the tick number %s on the demo.\n", this->getName(), this->getSteamID(), tostring(demoticks).c_str());
		}
		else
		{
			contell(this->getEdict(), "[" NCZ_PLUGIN_NAME "] Cannot mark a tick in demo will not recording.\n");
		}
	}
	else
	{
		contell(this->getEdict(), "[" NCZ_PLUGIN_NAME "] Please, dont flood this function.\n");
	}
}

void Cg_NCZ_Player::Connecting()
{
	connectionTime = (int)(GetTime() + 0.00000001);
}

void Cg_NCZ_Player::AddToBanlistCheck()
{
	if(banlists)
	{
		const char * SteamID = this->getSteamID();
		if(!SteamID) return;

		if(!(SteamID[0] == 'B' || SteamID[6] == 'I'))
		{
			banlists->send(tostring(SteamID));
		}
	}
}

void Cg_NCZ_Player::mystatus()
{
	if(/*allow_status &&*/(last_auto_status + 1.0f < GetTime()))
	{
		last_auto_status = GetTime();
		edict_t* edict = this->getEdict();
		contell(edict, format("[" NCZ_PLUGIN_NAME "] %s :", GetTranslate("AutoStatus").c_str()).c_str());
		contell(edict, "--------------------------");
		contell(edict, format("uID  - NAME             [     Steam-ID     ] - %s      - K/D - rate / cmd / update / interp (ratio)", GetTranslate("Team").c_str()).c_str());
		for (int i=1; i<=gpGlobals->maxClients; i++)
		{
			Cg_NCZ_Player *MyPlayer = g_NoCheatZPlugin.get_m_pm()->getPlayer(i);
			if(MyPlayer)
			{
				IPlayerInfo* PlayerInfo = MyPlayer->getPlayerInfo();
				if(PlayerInfo)
				{
					if(PlayerInfo->IsConnected())
					{
						if(MyPlayer->isRealPlayer())
						{
							const char * team = GetTranslate("Spectator").c_str();
							if (PlayerInfo->GetTeamIndex() == 2)
							{
								team = "Terrorist";
							}
							else if (PlayerInfo->GetTeamIndex() == 3)
							{
								team = "CT";
							}
							contell(edict, format("%s - %s [%s] - %s - %d/%d - %s", minStrf(tostring(MyPlayer->getUserid()), 4).c_str(), minStrf(MyPlayer->getName(), 16).c_str(), minStrf(MyPlayer->getSteamID(), 16).c_str(), minStrf(team, 9).c_str(), PlayerInfo->GetFragCount(), PlayerInfo->GetDeathCount(), getStrRates(i).c_str()).c_str());
							//delete team;
						}
					}
				}
			}
		}
		edict_t *pTV = getSourceTV();
		if(pTV)
		{
			Cg_NCZ_Player *MyTV = g_NoCheatZPlugin.get_m_pm()->getPlayer(pTV);
			contell(edict, format("%s - %s [     Source TV    ] -> TV Delay : %s s, TV Port : %s", minStrf(format("%d", MyTV->getUserid()), 4).c_str(), minStrf(MyTV->getName(), 16).c_str(), g_pCVar->FindVar("tv_delay")->GetString(), g_pCVar->FindVar("tv_port")->GetString()).c_str());
		}
		contell(edict, "--------------------------");
		contell(edict, "Steam Community Links :");
		for (int i=1; i<=gpGlobals->maxClients; i++)
		{
			Cg_NCZ_Player *MyPlayer = g_NoCheatZPlugin.get_m_pm()->getPlayer(i);
			if(MyPlayer)
			{
				if(MyPlayer->isRealPlayer())
				{
					contell(edict, format("%s -> %s", MyPlayer->getName(), getCommunityID(MyPlayer->getSteamID()).c_str()).c_str());
				}
			}
		}
		contell(edict, "--------------------------");
	}
}

void Cg_NCZ_Player::setBanRequestTime()
{
	if(ban_request_time == 0.0f)
	{
		ban_request_time = GetTime() + 11.0f;
	}
	else
	{
		ban_request_time = GetTime() + 60.0f;
	}
}

void Cg_NCZ_Player::Disconnecting()
{
	if (this->getTotalDetectionsCount() > 0)
	{
		std::string tolog = "";
		tolog  = NCZ_REPORT_FIRST_LINE
		tolog += format("%s [%s] disconnected with %d detections.", this->getName(), this->getSteamID(), this->getTotalDetectionsCount());
		tolog += format("Detection Details -> Bad Eye Angle X : %d, Bad Eye Angle Y : %d, Bad Eye Angle Z : %d, Aimbot : %d, ConVar Bypass : %d, TriggerBot : %d, BunnyHop Script %d.\n", this->total_beax, this->total_beay, this->total_beaz, total_badm, this->total_cvb, this->total_rf, total_bh);
		queue_send_report->send(format("i=%s&l=%s&aab=0", this->getSteamID(), tolog.c_str()));
		writeToLogfile(format("%s : Player %s [%s] disconnected with %d detections.\n------------------------------------------------------------", getStrDateTime("%x %X").c_str(), this->getName(), this->getSteamID(), this->getTotalDetectionsCount()));
		if(kick_ban)
		{
			//xMsg("Can't ban %d %s [%s] - %s (ncz_kick_and_ban = 0).\n");
			engine->ServerCommand("writeid\n");
			engine->ServerCommand("writeip\n");
		}
	}
	ban_request_time = 0.0f;
	total_beax = 0;
	total_beay = 0;
	total_beaz = 0;
	total_cvb = 0;
	total_rf = 0;
	total_badm = 0;
	xtrigger_count = 0;
	ytrigger_count = 0;
	sub_badm = 0;
	total_bh = 0;
	lastGoodAngles = QAngle(0,0,0);
}

void Cg_NCZ_Player::nameChanged(const char * newName)
{
	std::string name = newName;
	if(name.find('&') != std::string::npos)
	{
		engine->ServerCommand(format("kickid %s \"Illegal char detected in your name.\"\n", this->getSteamID()).c_str());
	}
	++nameChanges;
	if(nameChanges > 1)
	{
		this->addLog("Too many name changes.");
		std::string tolog = "";
		tolog  = NCZ_REPORT_FIRST_LINE
		tolog += format("%s [%s] : %s.\n", this->getName(), this->getSteamID(), "Too many name changes.");
		queue_send_report->send(format("i=%s&l=%s", this->getSteamID(), tolog.c_str()));
		noTell(this->getEdict(), format("\1[\004" NCZ_PLUGIN_NAME "\001] %s [\003%s\001] -> Too many name changes.", this->getName(), this->getSteamID()).c_str());
		this->Ban(false, 0, "Banned by NoCheatZ 3");
	}
}

void Cg_NCZ_Player::resetNamesChanges()
{
	nameChanges = 0;
}

void Cg_NCZ_Player::Ban(bool wait, int minutes, const char * msg)
{
	if(kick_ban)
	{
		if(!wait)
		{
			const char * SteamID = this->getSteamID();
			if(SteamID)
			{
				bool isConnected = this->getPlayerInfo()->IsConnected();
				bool banHandled = false;
				if (isConnected)
				{
					if(FINDCOMMAND("gb_externalBanUser"))
					{
						engine->ServerCommand(format("gb_externalBanUser \"%s\" \"%s\" \"%s\" %d minutes \"%s\"\n", gb_admin_id.c_str(), SteamID, gb_reason_id.c_str(), minutes, this->getName()).c_str());
						//banHandled = true;
					}
					if(FINDCOMMAND("sm_ban"))
					{
						engine->ServerCommand(format("sm_ban %d %d \"%s\"\n", this->getUserid(), minutes, msg).c_str());
						banHandled = true;
					}
					//if(FINDCOMMAND("ma_ban")) engine->ServerCommand(format("ma_ban \"%s\" %d \"%s\"\n", SteamID, minutes, msg).c_str());
				}
			
				if(!banHandled && isConnected)
				{
					engine->ServerCommand(format("banid %d %s\n", minutes, this->getSteamID()).c_str());
					this->Kick(msg);
					engine->ServerCommand("writeid\n");
					if (strcmp("127.0.0.1", this->getIPAddress()) != 0 && !banHandled)
					{
						engine->ServerCommand(format("addip 1440 \"%s\"\n", this->getIPAddress()).c_str());
						engine->ServerCommand("writeip\n");
					}
				}
			}
			ban_request_time = 0.0f;
		}
		else
		{
			if (kick_ban && (ban_request_time < GetTime()))
			{
				xMsg("Delayed ban for %s [%s] - %s, waiting for other detections ...\n", this->getName(), this->getSteamID(), this->getIPAddress());
				this->setBanRequestTime();
			}
		}
	}
}

void Cg_NCZ_Player::startRIE()
{
	writeToLogfile(getStrDateTime("%x %X") + format(" : %s [%s] %s.", this->getName(), this->getSteamID(), GetTranslate("player_record").c_str()));
	myDemoTicksCount = 0;
	recordingRIE = true;
}

void Cg_NCZ_Player::check_ban_request()
{
	if((ban_request_time <= GetTime()) && (!ban_request_time == 0.0f))
	{
		if(total_beay == 1) total_beay = 0; // Supress false detection.
		if(this->getTotalDetectionsCount() > 20 || this->total_cvb > 1 || this->total_beax > 40 || this->total_beay > 40 || this->total_beaz > 10 || this->total_rf > 1 || total_badm > 6 || total_bh > 1)
		{
			if (kick_ban)
			{
				xMsg("Ban of %d %s [%s] - %s permanently.\n", this->getUserid(), this->getName(), this->getSteamID(), this->getIPAddress());
			}
			const char * msg = "Banned by NoCheatZ 3";
			if(report_cheats) msg = format("Banned by NoCheatZ 3 (http://nocheatz.com/?id=%s)", this->getSteamID()).c_str();
			this->Ban(false, 0, msg);
		}
		else
		{
			/*if (kick_ban)
			{
				//xMsg("Ban delayed for %d %s [%s] - %s. Need more detections.\n", this->getUserid(), this->getName(), this->getSteamID(), this->getIPAddress());
			}*/
			ban_request_time += 11.0f;
		}
	}
}

void Cg_NCZ_Player::Kick(const char * msg)
{
	if (kick_ban)
	{
		engine->ServerCommand(format("kickid %d [" NCZ_PLUGIN_NAME "] %s\n", this->getUserid(), msg).c_str());
	}
}

int Cg_NCZ_Player::getTotalDetectionsCount() const
{
	return (total_beax + total_beay + total_beaz + total_cvb + total_rf + total_badm + total_bh);
}

void Cg_NCZ_Player::addLog(std::string text)
{
	Msg("%s [%s - %s] : %s.\n", this->getName(), this->getSteamID(), this->getIPAddress(), text.c_str());

	std::string tolog = "";
	tolog  = "MAP : " + getMapName() +  " - Date Time : " + getStrDateTime("%x %X") + " : ";
	tolog += format("%s [%s - %s] : %s.\n", this->getName(), this->getSteamID(), this->getIPAddress(), text.c_str());
	if(recording)
	{
		tolog += format("Demo  : %s.dem -> Tick number %d.\n", recordFilename.c_str(), demoticks);
	}
	writeToLogfile(tolog);
}

void Cg_NCZ_Player::testConfig()
{
#ifndef NCZ_CSS
	if(this->isValidPlayer() && ncz_active && test_config)
	{
		if(this->isRealPlayer())
		{
			float rate = atof(getClCV(this->getIndex(), "rate"));
			float cmdrate = atof(getClCV(this->getIndex(), "cl_cmdrate"));
			float updaterate = atof(getClCV(this->getIndex(), "cl_cmdrate"));
			float interp = atof(getClCV(this->getIndex(), "cl_interp"));
			if(rate != 0 && rate <= 10000.0f)
			{
				this->Kick(GetTranslate("rate_kick").c_str());
			}
			if(!between(cmdrate, 30.0f, 101.0f))
			{
				this->Kick(GetTranslate("cl_cmdrate_kick").c_str());
			}
			if(!between(updaterate, 20.0f, 101.0f))
			{
				this->Kick(GetTranslate("cl_updaterate_kick").c_str());
			}
			if(!between(interp, 0.0f, 0.1f))
			{
				this->Kick(GetTranslate("cl_interp_kick").c_str());
			}
		}
	}
#endif
}

void Cg_NCZ_Player::handleConvarTest()
{
	if(check_client_vars) return;
	if(lastCvarTest + 33 > gpGlobals->tickcount) return;
	if(!this->isCheckable()) return;
	if(waitcvarresp) return;

	lastCvarTest = gpGlobals->tickcount;
	ConvarList::iterator it;

	if(lastCvarTested)
	{
		for(it = g_cvars.begin(); it != g_cvars.end(); ++it)
		{
			if(strcmp(it->first, lastCvarTested) == 0)
			{
				++it;
				break;
			}
		}
	}
	else it = g_cvars.begin();

	//if(it != g_cvars.end())
	//{
		
		if(it != g_cvars.end())
		{
			waitcvarresp = true;
			helpers->StartQueryCvarValue(this->getEdict(), it->first);
			//Msg("Asking %s.\n", it->first);
		}
		else 
		{
			waitcvarresp = true;
			helpers->StartQueryCvarValue(this->getEdict(), g_cvars.begin()->first);
			//Msg("Asking %s.\n", g_cvars.begin()->first);
		}

	//}
}

void Cg_NCZ_Player::testConVarsCallback(QueryCvarCookie_t iCookie, EQueryCvarValueStatus eStatus, const char *pCvarName, const char *pCvarValue)
{
	//Msg("Got %s = %s.\n", pCvarName, pCvarValue);
	ConvarList::iterator it;
	/*if(strcmp("m_pitch", pCvarName) == 0)
	{
		//m_pitchValue = pCvarValue;
		last_m_pitchValue = m_pitchValue;
		m_pitchValue = pCvarValue;
		m_pitchReady = true;
	}
	if(strcmp("m_filter", pCvarName) == 0)
	{
		//m_filterValue = pCvarValue;
		m_filterValue = pCvarValue;
		m_filterReady = true;
	}*/
	if (check_client_vars)
	{
		bool isOurVar = false;
		if (eStatus == eQueryCvarValueStatus_ValueIntact)
		{
			for(it = g_cvars.begin(); it != g_cvars.end(); ++it)
			{
				if(strcmp(it->first, pCvarName) == 0) break;
			}
			if(it != g_cvars.end()) isOurVar = true;
			if (isOurVar)
			{
				std::string value(it->second);
				ConVar * convar_to_link = g_pCVar->FindVar(pCvarName);
				if (value == "$sv" && convar_to_link)
				{
					value = convar_to_link->GetString();
				}
				if (value.find(pCvarValue) == std::string::npos && value != "$n")
				{
					if(tostring(pCvarName) == "host_framerate")
					{
							//total_cvb ++;
							//std::string tolog = "";
							//tolog  = NCZ_REPORT_FIRST_LINE
							//tolog += format("%s [%s] : %s.\n", this->getName(), this->getSteamID(), format("Bad ConVar value : %s = %s (Must be equal to 0)", pCvarName, pCvarValue, value.c_str()).c_str());
							//queue_send_report->send(format("i=%s&l=%s", this->getSteamID(), tolog.c_str()));
							noTell(this->getEdict(), format("\1[\004" NCZ_PLUGIN_NAME "\001] %s [\003%s\001] -> Bad host_framerate.", this->getName(), this->getSteamID()).c_str());
							this->Kick(format("%s host_framerate %s %s.", GetTranslate("Type").c_str(), value.c_str(), GetTranslate("into_console").c_str()).c_str());
					}
					else if(tostring(pCvarName) == "host_timescale")
					{
						if (strcmp("1.0", pCvarValue) != 0)
						{
							total_cvb ++;
							std::string tolog = "";
							tolog  = NCZ_REPORT_FIRST_LINE
							tolog += format("%s [%s] : %s.\n", this->getName(), this->getSteamID(), format("Bad ConVar value : %s = %s (Must be equal to %s)", pCvarName, pCvarValue, value.c_str()).c_str());
							queue_send_report->send(format("i=%s&l=%s", this->getSteamID(), tolog.c_str()));
							noTell(this->getEdict(), format("\1[\004" NCZ_PLUGIN_NAME "\001] %s [\003%s\001] -> Bad host_timescale.", this->getName(), this->getSteamID()).c_str());
							this->Kick(format("%s host_timescale %s %s.", GetTranslate("Type").c_str(), value.c_str(), GetTranslate("into_console").c_str()).c_str());
						}
					}
					else
					{
						total_cvb ++;
						if(last_cvb_log + 10.0f < GetTime() && lastCvarDetected.find(pCvarName) == std::string::npos)
						{
							lastCvarDetected = pCvarName;
							last_cvb_log = GetTime();
							this->addLog(format("%s : %s = %s (%s %s)", GetTranslate("bad_value").c_str(), pCvarName, pCvarValue, GetTranslate("must_be_equal").c_str(), value.c_str()));
							std::string tolog = "";
							tolog  = NCZ_REPORT_FIRST_LINE
							tolog += format("%s [%s] : %s.\n", this->getName(), this->getSteamID(), format("Bad ConVar value : %s = %s (Must be equal to %s)", pCvarName, pCvarValue, value.c_str()).c_str());
							queue_send_report->send(format("i=%s&l=%s&fab=1", this->getSteamID(), tolog.c_str()));
							noTell(this->getEdict(), format("\1[\004" NCZ_PLUGIN_NAME "\001] %s [\003%s\001] -> %s ConVar Bypass %s.", this->getName(), this->getSteamID(), pCvarName, GetTranslate("Detected").c_str()).c_str());
						}
						this->Ban(true, 0, "Banned by NoCheatZ 3");
					}
				}
				if(value == "$n")
				{
					total_cvb ++;
					if(last_cvb_log + 10.0f < GetTime() && lastCvarDetected.find(pCvarName) == std::string::npos)
					{
						lastCvarDetected = pCvarName;
						this->addLog(format("ConVar must not exists : %s.", pCvarName));
						std::string tolog = "";
						tolog  = NCZ_REPORT_FIRST_LINE
						tolog += format("%s [%s] : %s.\n", this->getName(), this->getSteamID(), format("ConVar must not exists : %s.", pCvarName).c_str());
						queue_send_report->send(format("i=%s&l=%s&aab=1&fab=1", this->getSteamID(), tolog.c_str()));
						noTell(this->getEdict(), format("\1[\004" NCZ_PLUGIN_NAME "\001] %s [\003%s\001] -> Illegal ConVar Detected.", this->getName(), this->getSteamID()).c_str());
					}
					this->Ban(true, 0, "Banned by NoCheatZ 3");
				}
			}
		}
		else if (eStatus == eQueryCvarValueStatus_CvarNotFound)
		{
			for(it = g_cvars.begin(); it != g_cvars.end(); ++it)
			{
				if(strcmp(it->first, pCvarName) == 0) break;
			}
			if(it != g_cvars.end()) isOurVar = true;
			if(isOurVar)
			{
				std::string value = it->second;
				if(!(value == "$n"))
				{
					std::string tolog = "";
					tolog  = NCZ_REPORT_FIRST_LINE
					tolog += format("%s [%s] : %s.\n", this->getName(), this->getSteamID(), format("Cvar Not Found : %s", pCvarName).c_str());
					queue_send_report->send(format("i=%s&l=%s&aab=0&fab=1", this->getSteamID(), tolog.c_str()));
				}
			}
		}
		else if (eStatus == eQueryCvarValueStatus_NotACvar)
		{
			std::string tolog = "";
			tolog  = NCZ_REPORT_FIRST_LINE
			tolog += format("%s [%s] : %s.\n", this->getName(), this->getSteamID(), format("Not A Cvar : %s", pCvarName).c_str());
			queue_send_report->send(format("i=%s&l=%s&aab=0", this->getSteamID(), tolog.c_str()));
		}
		else if (eStatus == eQueryCvarValueStatus_CvarProtected)
		{
			std::string tolog = "";
			tolog  = NCZ_REPORT_FIRST_LINE
			tolog += format("%s [%s] : %s.\n", this->getName(), this->getSteamID(), format("Cvar protected : %s", pCvarName).c_str());
			queue_send_report->send(format("i=%s&l=%s&aab=0", this->getSteamID(), tolog.c_str()));
		}
		waitcvarresp = false;
	}
}

void Cg_NCZ_Player::tell(const char *message)
{
	if (this->isValidPlayer())
	{
		MRecipientFilter filter;
		filter.AddRecipient(this->getIndex());
		bf_write *pBuffer = engine->UserMessageBegin( &filter, 3 );
		pBuffer->WriteByte( 0 );
		pBuffer->WriteString(message);
		pBuffer->WriteString("\n");
		engine->MessageEnd();
	}
}

float Cg_NCZ_Player::getLastLogTime(int ang) const
{
	if (ang == 0)
	{
		return this->last_log_time_x;
	}
	else if (ang == 1)
	{
		return this->last_log_time_y;
	}
	else if (ang == 3)
	{
		return this->last_log_time_twice;
	}
	else
	{
		return GetTime();
	}
}

void Cg_NCZ_Player::setLastLogTime(int ang)
{
	if (ang == 0)
	{
		last_log_time_x = GetTime() + 3.0f;
	}
	else if (ang == 1)
	{
		last_log_time_y = GetTime() + 3.0f;
	}
	else if (ang == 3)
	{
		last_log_time_twice = GetTime() + 3.0f;
	}
}

void Cg_NCZ_Player::addUkCmd()
{
	cmd_p_s ++;
}

void Cg_NCZ_Player::resetUkCmd()
{
	cmd_p_s = 0;
}

int Cg_NCZ_Player::getUkCmdCount() const
{
	return cmd_p_s;
}

bool testSurfAA(int y)
{
	if((tostring(gpGlobals->mapname.ToCStr()).find("surf") != std::string::npos) && (y == 270))
	{
		return false;
	}
	return true;
}

bool testSurfAA(float y)
{
	if((tostring(gpGlobals->mapname.ToCStr()).find("surf") != std::string::npos) && (y == 270.0f))
	{
		return false;
	}
	return true;
}

bool testFloatInt(float y)
{
	int iy = (int)y;
	float fy = (float)iy;
	if(y == fy) return true;
	return false;
}

bool testFloatY(float y)
{
	int iy = (int)(y + 0.000001);
	float fy = (float)iy;
	if(((y == fy) && aimbots_extra_filter))
	{
		return false;
	}
	return true;
}

void Cg_NCZ_Player::setIgnoreFirst()
{
	ignoreFirst = true;
}

#define DIR_RIGHT_UP 0
#define DIR_RIGHT 1
#define DIR_RIGHT_DOWN 2
#define DIR_DOWN 3
#define DIR_LEFT_DOWN 4
#define DIR_LEFT 5
#define DIR_LEFT_UP 6
#define DIR_UP 7

float maxAimSpeed = 0.0f;

void Cg_NCZ_Player::checkAimDirChange()
{
	if(sub_badv > 7)
	{
		//Msg("Aimbot Detection\n");
		++total_badv;
		sub_badv = 0;
		if(total_badv > 1)
		{
			//this->addLog("Aimbot Detected (System 3 - Aim Speed)");
			//std::string msg = format("\1[\004" NCZ_PLUGIN_NAME "\001] %s [\003%s\001] -> Aimbot Detected (System 3).\n", this->getName(), this->getSteamID());
			//noTell(this->getEdict(), msg.c_str());
			//edict_t * pTV = getSourceTV();
			//if(pTV)
			//{
			//	::tell(pTV, msg.c_str());
			//}
			//std::string tolog = "";
			//xMsg(
			//tolog  = NCZ_REPORT_FIRST_LINE
			//tolog += format("%s [%s] : Aimbot Detected (System 3).\n", this->getName(), this->getSteamID());
			//tolog += "Debug : (Weapon, Time Connected,  Kills/Deaths, Total Detections Count)\n";
			//tolog += format("------> (%s, %s, %s/%s, %d)\n", this->getWeapon(), formatSeconds(this->getTimeConnected()).c_str(), tostring(this->getPlayerInfo()->GetFragCount()).c_str(), tostring(this->getPlayerInfo()->GetDeathCount()).c_str(), this->getTotalDetectionsCount());
			//queue_send_report->send(format("i=%s&l=%s", this->getSteamID(), tolog.c_str()));
			//this->Ban(true, 0, "Banned by NoCheatZ 3");
			
		}
	}
}

void Cg_NCZ_Player::testEyeAngles(CUserCmd * pCmd)
{
	if(EyeAnglesAlreadyChecked) return;
	if((aimbot_system_number < 2) && pCmd) return;
	if(!check_aimbots) return;
	if(!this->isRealPlayer()) return;
	if(!this->isCheckable()) return;
	bool detect = false;
	edict_t *pEdict = this->getEdict();
	if(!::isValidEdict(pEdict)) return;
	INetChannelInfo *channel = this->getChannelInfo();
	QAngle hisangles = QAngle(0,0,0);
	short accum = 0;
	short xaccum = 0;
	int command_number = 0;
	if(pCmd)
	{
		//Msg("We got pCmd.\n");
		hisangles = QAngle(pCmd->viewangles);
		accum = pCmd->mousedy;
		xaccum = pCmd->mousedx;
		EyeAnglesAlreadyChecked = true;
	}
	else
	{
		hisangles = this->getPlayerInfo()->GetLastUserCommand().viewangles;
		accum = this->getPlayerInfo()->GetLastUserCommand().mousedy;
		xaccum = this->getPlayerInfo()->GetLastUserCommand().mousedx;
		//if(this->getPlayerInfo()->GetLastUserCommand().weaponselect != 0) printf("%d\n", this->getPlayerInfo()->GetLastUserCommand().weaponselect);
	}
	// New hack detection which can detect aimbots that is modifying Eye Angles even if they fix angles from going too far.

	float aimSpeed = 0.0f;
	float x = (hisangles.x - lastfx);
	float absCurY = fabs(hisangles.y);
	float absLastY = fabs(lastfy);
	float y = (absCurY - absLastY);

	aimSpeed = sqrt(x) + sqrt(y);


	if(fabs(hisangles.x) < 89.0f && fabs(hisangles.y) < 180.0f && fabs(hisangles.z) < 90.0f)
	{
		/*                                                 FIXME
		if(aimbot_system_number >= 3)
		{
		if(aimSpeed > maxAimSpeed)
		{
		maxAimSpeed = aimSpeed;
		//Msg("MAX Aim Speed : %3.12f \n", aimSpeed);
		}

		if(aimSpeed > 80.0f && !eat_x_lock)
		{
		//Msg("Aim Speed : {%3.3f, %3.3f} {%3.3f, %3.3f} => %3.12f \n", hisangles.x, hisangles.y, lastfx, lastfy, aimSpeed);
		++sub_badv;
		}
		}
		*/

#ifndef NCZ_CSGO
		//lastAimSpeed = aimSpeed;
		Cg_NCZ_Player *ptarget = aimingAt();
		if(aimbot_system_number > 1)
		{
			/*
			if(m_pitchReady && m_filterReady)
			{
				if(notsureabout)
				{
					if(last_m_pitchValue)
					{
						if(strcmp(last_m_pitchValue, m_pitchValue) != 0)
						{
							if(sub_badm)
							{
								//xMsg("Removed %d false pre-detections on anti-aimbot system 2 for %s [%s].", sub_badm, this->getName(), this->getSteamID());
								sub_badm = 0;
							}
						}
						last_m_pitchValue = m_pitchValue;
					}
					notsureabout = false;
				}

				fx = hisangles.x;
				fy = hisangles.y;
				//Msg("%3.6f (%3.6f) : %3.6f : %3.6f\n", hisangles.x, fx, hisangles.y, hisangles.z);

				// Is he using inverted mouse ?
				//const char *sPitch = engine->GetClientConVarValue(this->getIndex(), "m_pitch");
				if(std::string(m_pitchValue).find("-") != std::string::npos) accum *= -1; // If so then invert his accumm too.
				int iFilter = atoi(m_filterValue);

#define SUBPROCEDURE_DETECT_SENDINFO(a) std::string sublog = ""; \
	sublog  = NCZ_REPORT_FIRST_LINE \
	sublog += format("Aimbot SubDetection Case "a" : CM {%d, %d} CVA {%3.4f, %3.4f, %3.1f} LM {%d, %d} LVA {%3.4f, %3.4f} TC {%d}\n", accum, xaccum, hisangles.x, hisangles.y, hisangles.z, lastdy, lastdx, lastfx, lastfy, xtrigger_count); \
	if(ptarget) sublog += format("+ Aiming at : %s [%s]\n", target->getName(), target->getSteamID()); \
	sublog += "Debug : (Weapon, Time Connected,  Kills/Deaths, Total Detections Count)\n"; \
	sublog += format("------> (%s, %s, %s/%s, %d)\n", this->getWeapon(), formatSeconds(this->getTimeConnected()).c_str(), tostring(this->getPlayerInfo()->GetFragCount()).c_str(), tostring(this->getPlayerInfo()->GetDeathCount()).c_str(), this->getTotalDetectionsCount()); \
	queue_send_report->send(format("i=%s&l=%s&aab=0&ah=1", this->getSteamID(), sublog.c_str()));
				
				if(ptarget)
				{
					if(!lastdy == 0)
					{
						if(accum == 0 && lastdy == 0 && xaccum == 0)
						{
							if(fabs(fabs(fx) - fabs(lastfx)) > 0.5f || fabs(fabs(fy) - fabs(lastfy)) > 0.5f)
							{
								if(!ignoreFirst && iFilter > 0) // Avoid false detection at spawn
								{
									SUBPROCEDURE_DETECT_SENDINFO("1")
										detect = true;
								}
								else
								{
									ignoreFirst = false;
								}
							}
						}
						if((fx != lastfx || fy != lastfy)) // Si la souris bouge avec la visée
						{
							if((accum > 0) && !(lastdy < 0))
							{
								if(!(fx >= lastfx) && !((fabs(fx) == 90.0f) || (fabs(lastfx) == 90.0f)) )
								{
									if(!ignoreFirst) // Avoid false detection at spawn
									{
										SUBPROCEDURE_DETECT_SENDINFO("2")
											detect = true;
									}
									else
									{
										ignoreFirst = false;
									}
								}
							}
							else if((accum < 0) && !(lastdy > 0))
							{		
								if(!(fx <= lastfx) && !((fabs(fx) == 90.0f) || (fabs(lastfx) == 90.0f)) )
								{
									if(!ignoreFirst) // Avoid false detection at spawn
									{
										SUBPROCEDURE_DETECT_SENDINFO("3")
											detect = true;
									}
									else
									{
										ignoreFirst = false;
									}
								}
							}
							if((xaccum > 0) && !(lastdx < 0))
							{
								if(!(fy <= lastfy) && (!(fy > 0.0f && lastfy < 0.0f) && !((fy < 0.0f && lastfy > 0.0f)) && !(lastfy == 0.0f)))
								{
									if(!ignoreFirst) // Avoid false detection at spawn
									{
										SUBPROCEDURE_DETECT_SENDINFO("4")
											detect = true;
									}
									else
									{
										ignoreFirst = false;
									}
								}
							}
							else if((xaccum < 0) && !(lastdx > 0))
							{
								if(!(fy >= lastfy) && (!(fy > 0.0f && lastfy < 0.0f) && !((fy < 0.0f && lastfy > 0.0f)) && !(lastfy == 0.0f)))
								{
									if(!ignoreFirst) // Avoid false detection at spawn
									{
										SUBPROCEDURE_DETECT_SENDINFO("5")
											detect = true;
									}
									else
									{
										ignoreFirst = false;
									}
								}
							}
						}
					}
				}
			}
			else
			{
				notsureabout = true;
				helpers->StartQueryCvarValue(pEdict, "m_pitch");
				helpers->StartQueryCvarValue(pEdict, "m_filter");
				//m_pitchReady = false;
				//m_filterReady = false;
			}

			lastdy = accum;
			lastdx = xaccum;
			lastfx = fx;
			lastfy = fy;

			if(!detect)
			{
				xtrigger_count = 0;
			}
			else
			{
				++xtrigger_count;
				last_trigger_add = GetTime();
			}


			if(xtrigger_count > 8)
			{
				SUBPROCEDURE_DETECT_SENDINFO("6")
				++sub_badm;
				xtrigger_count = 0;
				helpers->StartQueryCvarValue(pEdict, "m_pitch");
				notsureabout = true;
				last_m_pitchValue = m_pitchValue;
				m_pitchReady = false;
			}

			if(sub_badm > 12 && notsureabout == false)
			{
				++total_badm;
				sub_badm = 0;
				this->addLog("Aimbot Detected (System 2)");
				std::string msg = format("\1[\004" NCZ_PLUGIN_NAME "\001] %s [\003%s\001] -> Aimbot Detected (System 2).\n", this->getName(), this->getSteamID());
				//this->addLog(format("Latency : %3.9f (%3.9f)\n", channel->GetLatency(FLOW_INCOMING), channel->GetAvgLatency(FLOW_INCOMING)));
				noTell(this->getEdict(), msg.c_str());
				edict_t * pTV = getSourceTV();
				if(pTV)
				{
					::tell(pTV, msg.c_str());
				}
				std::string tolog = "";
				tolog  = NCZ_REPORT_FIRST_LINE
					tolog += format("%s [%s] : Aimbot Detected (System2).\n", this->getName(), this->getSteamID());
				if(ptarget) tolog += format("+ Aiming at : %s [%s]\n", target->getName(), target->getSteamID());
				//tolog += format("Latency : %3.9f (%3.9f)\n", channel->GetLatency(FLOW_INCOMING), channel->GetAvgLatency(FLOW_INCOMING));
				tolog += "Debug : (Weapon, Time Connected,  Kills/Deaths, Total Detections Count)\n";
				tolog += format("------> (%s, %s, %s/%s, %d)\n", this->getWeapon(), formatSeconds(this->getTimeConnected()).c_str(), tostring(this->getPlayerInfo()->GetFragCount()).c_str(), tostring(this->getPlayerInfo()->GetDeathCount()).c_str(), this->getTotalDetectionsCount());
				queue_send_report->send(format("i=%s&l=%s&aab=0", this->getSteamID(), tolog.c_str()));
				this->Ban(true, 0, "Banned by NoCheatZ 3");
			}*/
		}
#endif
	}
	else
	{

		beax = false;
		beay = false;
		beaz = false;
		const float currentTime = GetTime();
		fx = fabs(hisangles.x);
		fy = fabs(hisangles.y);
		fz = fabs(hisangles.z);


		bads = "";
		sugg = "";
		if (fx > 89.0f)
		{
			beax = true;
		}
		if(fz > 90.0f)
		{
			beaz = true;
		}
		if((fy > 180.0f) && !(fy == 255.0f) )
		{
			bool tsAA = testSurfAA(fy);
			bool tempCond = true;
			if(aimbots_extra_filter && (fy == 270.0f)) tempCond = false;
			if(tempCond && tsAA)
			{
				if(!this->isInBuyZone())
				{
					beay = true;
				}
				else if(this->isInBuyZone() && !beax) return;
			}
			else if(!tempCond && !tsAA)
			{
				if (this->getLastLogTime(2) < currentTime)
				{
					fy = hisangles.y;
					std::string message(getStrDateTime("%x %X") + format(" : Eye Angles Extra Filter -> Ignoring this detection : Bad Y (Yaw) Eye Angle (Y = %3.4f) (Probably Anti Aim Hack) for %s [%s - %s].", fy, this->getName(), this->getSteamID(), this->getIPAddress()));
					writeToLogfile(message);
				}
			}
		}

		if(!beay) ytrigger_count = 0;
		else ytrigger_count ++;

		if(ytrigger_count < 3) beay = false;

		if(!beax && !beay && !beaz) return;

		fx = hisangles.x;
		fy = hisangles.y;
		fz = hisangles.z;

		bool already = false;
		bool sugg_already = false;
		bool aah = false;
		bool aimbot = false;
		bool spinhack = false;
		bool nospread = false;
		bool norecoil = false;
		if(beax)
		{
			already = true;
			bads.append("Bad X (Pitch)");
			if(testFloatInt(fx))
			{
				aah = true;
			}
			else
			{
				norecoil = true;
			}
		}
		if(beay)
		{
			if(already)
			{
				bads.append(" and ");
			}
			already = true;
			bads.append("Bad Y (Yaw)");
			if(testFloatInt(fy))
			{
				aah = true;
				spinhack = true;
			}
			else
			{
				nospread = true;
				aimbot = true;
			}
		}
		if(beaz)
		{
			if(already)
			{
				bads.append(" and ");
			}
			already = true;
			bads.append("Bad Z (Roll)");
			aah = true;
		}
		if(aah)
		{
			sugg.append("Anti Aim Hack");
			sugg_already = true;
		}
		if(aimbot)
		{
			if(sugg_already)
			{
				sugg.append("/");
			}
			sugg_already = true;
			sugg.append("Aimbot");
		}
		if(spinhack)
		{
			if(sugg_already)
			{
				sugg.append("/");
			}
			sugg_already = true;
			sugg.append("Spinhack");
		}
		if(nospread || norecoil)
		{
			if(sugg_already)
			{
				sugg.append("/");
			}
			sugg_already = true;
			sugg.append("Accuracy Hack");
		}
		if(pCmd) sugg.append(" (System 4 - Blocked and Detected)");
		else sugg.append(" (System 1 - Detected)");
		if((gpGlobals->tickcount - lastSpawnTime > 335) && lastSpawnTime > 0)
		{
			if(beax) total_beax ++;
			if(beay) total_beay ++;
			if(beaz) total_beaz ++;
			if (this->getLastLogTime(3) < currentTime )
			{
				this->setLastLogTime(3);
				this->addLog(format("%s Eye Angles (x=%3.4f, y=%3.4f, z=%3.4f) -> %s", bads.c_str(), fx, fy, fz, sugg.c_str()));
				noTell(this->getEdict(), format("\1[\004" NCZ_PLUGIN_NAME "\001] %s [\003%s\001] -> %s %s.\n", this->getName(), this->getSteamID(), sugg.c_str()).c_str());
				std::string tolog = "";
				tolog  = NCZ_REPORT_FIRST_LINE
					tolog += format("%s [%s] : %s.\n", this->getName(), this->getSteamID(), format("%s Eye Angles (x=%3.4f, y=%3.4f, z=%3.4f) -> %s", bads.c_str(), fx, fy, fz, sugg.c_str()).c_str());
				tolog += "Debug : (Weapon, Time Connected,  Kills/Deaths, Total Detections Count)\n";
				tolog += format("------> (%s, %s, %s/%s, %d)\n", this->getWeapon(), formatSeconds(this->getTimeConnected()).c_str(), tostring(this->getPlayerInfo()->GetFragCount()).c_str(), tostring(this->getPlayerInfo()->GetDeathCount()).c_str(), this->getTotalDetectionsCount());
				queue_send_report->send(format("i=%s&l=%s&fab=1", this->getSteamID(), tolog.c_str()));
				this->Ban(true, 0, "Banned by NoCheatZ 3");
				//entpropsmanager->getPropValue<float>("CCSPlayer.m_angEyeAngles[0]", pEdict) = lastGoodAngles.x;
				//entpropsmanager->getPropValue<float>("CCSPlayer.m_angEyeAngles[1]", pEdict) = lastGoodAngles.y;
			}
		}
	}
}

void Cg_NCZ_Player::update()
{
	if(recordingRIE) myDemoTicksCount ++;
	if(GetTime() > nextSecond) this->eat_unlock();
}

void Cg_NCZ_Player::setLastLockTime()
{
	last_lock_time = gpGlobals->tickcount;
}

void Cg_NCZ_Player::eat_lock()
{
	this->setLastLockTime();
	eat_x_lock = true;
	eat_y_lock = true;
	eat_z_lock = true;
	ignoreFirst = true;
}

void Cg_NCZ_Player::eat_unlock()
{
	if (this->isValidPlayer())
	{
		IPlayerInfo* pInfo = this->getPlayerInfo();
		if(pInfo)
		{
			if(!pInfo->IsDead() && pInfo->GetTeamIndex() > 1)
			{
				if ((last_lock_time + 330 <= gpGlobals->tickcount) && last_lock_time)
				{
					eat_x_lock = false;
					eat_y_lock = false;
					eat_z_lock = false;
					return;
				}
			}
		}
	}
}


// -------------------------------------------------------------------------------------------------------------
// Others Functions
// -------------------------------------------------------------------------------------------------------------

void needUseWeb()
{
	if(!report_cheats && !queue_send_report->empty())
	{
		queue_send_report->erase();
	}
	if(!banlists->empty() && !use_banlists)
	{
		banlists->erase();
	}
	if ((!queue_send_report->empty() && report_cheats) || !disconnectingPlayers->empty() /*|| !player_conf->empty()*/ || (!banlists->empty() && use_banlists) || maySendHeartbeat)
	{
		//Msg("Waking up WebThread.\n");
#ifndef NCZ_CSGO
		if(!TheWebThread) TheWebThread = new NczThread();
		if(!TheWebThread->IsAlive())
		{
			TheWebThread->Start();
		}
#else
		if(!TheWebThread) TheWebThread = new NczThread();
		if(TheWebThread->MustBeKilled())
		{
			TheWebThread->Terminate(0);
			//delete TheWebThread;
			TheWebThread = new NczThread();
		}
		if(!TheWebThread->IsAlive()) TheWebThread->Start();
#endif
	}
}

#ifdef WIN32
std::string NumString_Add(std::string num, std::string add)
{
	int numero1, numero2, temp, retenue = 0;
	std::string resultat = "";

	while(num.length() || add.length())
	{

		numero1 = 0;
		numero2 = 0;
		if(num.length())
		{
			numero1 = atoi(tostring(num.back()).c_str());
			num.pop_back();
		}
		if(add.length())
		{
			numero2 = atoi(tostring(add.back()).c_str());
			add.pop_back();
		}

		temp = numero1 + numero2 + retenue;

		if(temp > 9)
		{
			std::string tretenue = tostring(format("%d", temp).at(0));
			retenue = atoi(tretenue.c_str());
		}
		else
		{
			retenue = 0;
		}
		std::string fix = format("%d", temp);
		resultat = fix.back() + resultat;
	}
	return resultat;
}
#else
std::string NumString_Add(std::string num, std::string add)
{
	int numero1, numero2, temp, retenue = 0;
	std::string resultat = "";

	while(num.length() || add.length())
	{
		//if(isdigit(num.back()) && isdigit(add.back()))

		numero1 = 0;
		numero2 = 0;
		if(num.length())
		{
			numero1 = atoi(tostring(num.substr(num.length()-1, num.length())).c_str());
			num = num.substr(0, num.length()-1);
		}
		if(add.length())
		{
			numero2 = atoi(tostring(add.substr(add.length()-1, add.length())).c_str());
			add = add.substr(0, add.length()-1);
		}

		temp = numero1 + numero2 + retenue;

		if(temp > 9)
		{
			std::string tretenue = tostring(format("%d", temp).at(0));
			retenue = atoi(tretenue.c_str());
		}
		else
		{
			retenue = 0;
		}
		std::string fix = format("%d", temp);
		fix = fix.substr(fix.length()-1, fix.length());
		resultat = fix + resultat;
	}
	return resultat;
}
#endif

bool between(const int n, const int min, const int max) // min and max Included
{
	if ((n <= max) && (n >= min))
	{
		return true;
	}
	return false;
}

bool between(const float n, const float min, const float max) // min and max Included
{
	if ((n <= max) && (n >= min))
	{
		return true;
	}
	return false;
}

std::string to_lowercase(const std::string& s)
{
	std::string k = s;
	toLowerCase(k);
	return k;
}


std::string format(const char *fmt, ...)
{
	va_list		argptr;
	static char		string[4096];

	va_start(argptr, fmt);
	Q_vsnprintf(string, sizeof(string), fmt,argptr);
	va_end (argptr);

	return tostring(string);
}

template<class T>
T *FindIFace(const char *IFaceName, CreateInterfaceFn Factory)
{
	T *pIFace = NULL;
	int i_v = 40;
	char s_v[64];
	while ( !pIFace )
	{
		if(i_v < 0)
		{
			return NULL;
		}
		if(i_v < 10)
		{
			sprintf(s_v, "%s00%d", IFaceName, i_v);
			//s_v = format("%s00%d", IFaceName, i_v);
		}
		else if(i_v < 100 && i_v > 9)
		{
			sprintf(s_v, "%s0%d", IFaceName, i_v);
			//s_v = format("%s0%d", IFaceName, i_v);
		}
		pIFace = (T*)Factory(s_v,NULL);
		--i_v;
	}
	return pIFace;
}

char* long2ip(unsigned long v) 
{ 
	struct in_addr x;
	x.s_addr = htonl(v);
	return inet_ntoa(x);
} 

float positiv(const float n)
{
	float x = n;
	if(x < 0)
	{
		x = x * -1;
	}
	return x;
}

int positiv(const int n)
{
	int x = n;
	if(x < 0)
	{
		x = x * -1;
	}
	return x;
}

int getIndexFromUserID(int userid)
{
	for(int i = 1; i <= playerinfomanager->GetGlobalVars()->maxClients; i++)
	{
		if(playerinfomanager->GetPlayerInfo(PlayerList+i))
		{
			if(playerinfomanager->GetPlayerInfo(PlayerList+i)->GetUserID() == userid)
			{
				return i;
			}
		}
	}
	return -1;
}

bool isValidEdict(edict_t * entity)
{
	return entity != NULL && !entity->IsFree() && !entity->GetUnknown() == NULL;
}

std::string getStrGameDir()
{
	return tostring(GAMEDIR);
}

std::string getStrDateTime(const char *format)
{
	time_t rawtime;
	struct tm * timeinfo;
	char date[256];
	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	strftime(date,sizeof(date),format,timeinfo);
	date[255] = '\0';
	return tostring(date);
}

void writeToLogfile(std::string text)
{
	if(engine)
	{
		std::string path = GAMEDIR"/logs/NoCheatZ_3_Logs/" + getStrDateTime("NoCheatZ-%d-%b-%Y") + ".log";
		std::ofstream fichier;
		fichier.open(path.c_str(), std::ios::out | std::ios::app);
		if(fichier)
		{
			fichier << text << std::endl;
			fichier.close();
		}
		else
		{
			Msg("[NoCheatZ 3] Can't write to logfile ...\n");
			Msg("[NoCheatZ 3] Ensure we have permissions to write in  ...\n");
			Msg(
		}
	}
}

#define PORT 80

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems)
{
	std::stringstream ss(s);
	std::string item;
	while(std::getline(ss, item, delim))
	{
		elems.push_back(item);
	}
	return elems;
}

std::vector<std::string> split(const std::string &s, char delim)
{
	std::vector<std::string> elems;
	return split(s, delim, elems);
}

edict_t * getEdictFromSteamID(const char *SteamID)
{
	const int imax = gpGlobals->maxClients;
	for(int i = 1;i<=imax;i++)
	{
		edict_t* pEntity = PEntityOfEntIndex(i);
		if(V_strcmp(engine->GetPlayerNetworkIDString(pEntity), SteamID) == 0)
		{
			return pEntity;
		}
	}
	return NULL;
}

int WebThread()
{
	//while(!must_thread_be_killed)
	//{
		while((!queue_send_report->empty()) && UseNet)
		{
			std::string elem = queue_send_report->get();
			if(!report_cheats) continue;
			elem += "&d=";
			elem += dumpConfig();

			NczHttpSockHandler *SendReport = new NczHttpSockHandler();
			if(SendReport->Connect(PIPELINE_ADDRESS, 80))
			{
				SendReport->SendPost("/reportcheat.php", elem);
				SendReport->End();
			}
			else
			{
				xMsg("Connection failed to Master Server. NoCheatZ Network functions will be off for 10 minutes.\n");
				lastNetOff = GetTime();
				UseNet = false;
			}
			delete SendReport;
		}
		if(maySendHeartbeat && UseNet)
		{
			std::string ip;
			if(g_pCVar->GetCommandLineValue("ip") == NULL) ip = "127.0.0.1";
			else ip = g_pCVar->GetCommandLineValue("ip");
			std::string port = g_pCVar->FindVar("hostport")->GetString();
			//if(ip.empty()) ip = "127.0.0.1";

			std::string content = "ip=" + ip + "&port=" + port + "&players=" + tostring(GetPlayerCount()) + "&version=NoCheatZ v" + NCZ_VERSION_STR;

			NczHttpSockHandler *Heartbeat = new NczHttpSockHandler();
			if(Heartbeat->Connect(PIPELINE_ADDRESS, 80))
			{
				Heartbeat->SendPost("/register.php", content);
				Heartbeat->End();
			}
			else
			{
				xMsg("Connection failed to Master Server. NoCheatZ Network functions will be off for 10 minutes.\n");
				lastNetOff = GetTime();
				UseNet = false;
			}
			delete Heartbeat;

			// Check for an update as well

			std::string sVersion;
			int iVersion = 0;
			int iNewVersion = 0;
			std::string pathEfile = GAMEDIR"/addons/nocheatz/version.txt";

			std::ifstream fVersion(pathEfile.c_str());
			if(fVersion)
			{
				fVersion >> iVersion;
			}
			fVersion.close();

			NczHttpSockHandler *GetVersion = new NczHttpSockHandler();
			if(!GetVersion->Connect(PIPELINE_ADDRESS, 80))
			{
				xMsg("Connection failed to Master Server. NoCheatZ Network functions will be off for 10 minutes.\n");
				lastNetOff = GetTime();
				UseNet = false;
				GetVersion->End();
				delete GetVersion;
			}
			else
			{
				GetVersion->SendGet("/update.php");
				std::string content = GetVersion->GetContent();
				iNewVersion = atoi(content.c_str());
			}
			GetVersion->End();
			delete GetVersion;

			if(iNewVersion > iVersion)
			{
		#ifndef WIN32
				xMsg("Plugin is out-of-date. Server will be restarted, when no human players are connected, in order to apply the update with the Auto Updater.\n");
				requireRestart = true;
		#else
				xMsg("Plugin is out-of-date. Server can't be restarted with Windows. Please, restart your server or download the new version here : http://download.nocheatz.com/ \n");
		#endif
			}
			else if(iNewVersion == iVersion)
			{
				xMsg("Plugin is up-to-date.\n");
			}
			else if(  iNewVersion < iVersion &&  !( iNewVersion == 0 )  )
			{
				xMsg("You are using a beta-test version.\n");
			}

			maySendHeartbeat = false;
		}
		while ((!banlists->empty()) && UseNet)
		{
			std::string elem = banlists->get();
			if(!use_banlists) continue;
			std::string content = "i=" + elem;

			NczHttpSockHandler *Banlisting = new NczHttpSockHandler();
			std::string sbuf;
			if(Banlisting->Connect(PIPELINE_ADDRESS, 80))
			{
				Banlisting->SendPost("/get_v3.php", content);
				sbuf = Banlisting->GetContent();
				Banlisting->End();
			}
			else
			{
				xMsg("Connection failed to Master Server. NoCheatZ Network functions will be off for 10 minutes.\n");
				lastNetOff = GetTime();
				UseNet = false;
			}
			delete Banlisting;
			int from = atoi(sbuf.substr(0, 1).c_str());
			if(from > 0 /* && kick_ban */)
			{
				switch(from)
				{
				case 1:
					{
						if(use_ncz_banlists)
						{
							engine->ServerCommand(format("kickid %s \"[" NCZ_PLUGIN_NAME "] WEB BANNED (NoCheatZ Master Banlist)\"\n", elem.c_str()).c_str());
							engine->ServerCommand(format("banid 1440 %s\n", elem.c_str()).c_str());
							engine->ServerCommand("writeid\n");
						}
						break;
					}

				case 2:
					{
						if(use_lgz_banlists)
						{
							engine->ServerCommand(format("kickid %s \"[" NCZ_PLUGIN_NAME "] WEB BANNED (LeetGamerZ Secure Source Banlist)\"\n",elem.c_str()).c_str());
							engine->ServerCommand(format("banid 1440 %s\n", elem.c_str()).c_str());
							engine->ServerCommand("writeid\n");
						}
						break;
					}

				case 4:
					{
						if(use_esl_banlists)
						{
							engine->ServerCommand(format("kickid %s \"[" NCZ_PLUGIN_NAME "] WEB BANNED (Electronic Sports League Banlist)\"\n",elem.c_str()).c_str());
							engine->ServerCommand(format("banid 1440 %s\n", elem.c_str()).c_str());
							engine->ServerCommand("writeid\n");
						}
					break;
					}

				case 5:
					{
						if(use_esea_banlists)
						{
							engine->ServerCommand(format("kickid %s \"[" NCZ_PLUGIN_NAME "] WEB BANNED (ESEA Anti-Cheat Banlist)\"\n",elem.c_str()).c_str());
							engine->ServerCommand(format("banid 1440 %s\n", elem.c_str()).c_str());
							engine->ServerCommand("writeid\n");
						}
					break;
					}

				/*case 6:
					{
						if(use_eac_banlists)
						{
							engine->ServerCommand(format("kickid %s \"[" NCZ_PLUGIN_NAME "] WEB BANNED (Easy Anti Cheat Banlist)\"\n",elem.c_str()).c_str());
							engine->ServerCommand(format("banid 1440 %s\n", elem.c_str()).c_str());
							engine->ServerCommand("writeid\n");
						}
					break;
					}*/
				default:
					break;
				}
			}
		}
		while(!disconnectingPlayers->empty() && UseNet)
		{
			std::string elem = disconnectingPlayers->get();
			if(!UseNet) continue;
			NczHttpSockHandler *Disconnect = new NczHttpSockHandler();
			std::string content = "i=" + elem;
			if(Disconnect->Connect(PIPELINE_ADDRESS, 80))
			{
				Disconnect->SendPost("/disconnect.php", content);
				Disconnect->End();
			}
			else
			{
				xMsg("Connection failed to Master Server. NoCheatZ Network functions will be off for 10 minutes.\n");
				lastNetOff = GetTime();
				UseNet = false;
			}
			delete Disconnect;
		}
		//break;
	//}
	return 0;
}

bool CreateAThreadAndCallTheFunction()
{
	if(!TheWebThread) TheWebThread = new NczThread();
	if(!TheWebThread->IsAlive())
	{
		//TheWebThread->SetName("NCZ_WEB_THREAD");
		//TheWebThread->SetPriority(10);
		maySendHeartbeat = true;
		TheWebThread->Start();
	}
	return true;
}

/*
bool checkForUpdate()
{
	if(!UpdateThread)
		UpdateThread = new NczThread(checkForUpdate_thread);
	if(!UpdateThread->IsAlive())
	{
		UpdateThread->SetName("NCZ_UPDATE_THREAD");
		UpdateThread->SetPriority(1);
		UpdateThread->Start();
	}
	return true;
}

char * nghost(char *host_name)
{
#ifdef WIN32
//	WSADATA wsaData;
	int iResult;
	//DWORD dwError;
	struct hostent *remoteHost;
//	struct in_addr addr;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	remoteHost = gethostbyname(host_name);
	if (!(remoteHost == NULL))
	{
		addr.s_addr = *(u_long *) remoteHost->h_addr_list[0];
		return inet_ntoa(addr);
	}
	return NULL;
#else
	struct hostent *host;
	struct in_addr addr;
	host = gethostbyname(host_name);
	if (!(host == NULL))
	{
#undef h_addr
		addr.s_addr = *((unsigned long *) host->h_addr_list[0]);
		return inet_ntoa(addr);
	}
	return NULL;
#endif
}*/

std::string urlencode(std::string str)
{
	return str;
}

// ##########################################
// Cg_NCZ_Queue
// ##########################################


Cg_NCZ_Queue::Cg_NCZ_Queue()
{
	locked = false;
}

Cg_NCZ_Queue::~Cg_NCZ_Queue()
{
}

bool Cg_NCZ_Queue::isLocked()
{
	return locked;
}

void Cg_NCZ_Queue::lock()
{
	locked = true;
}

void Cg_NCZ_Queue::unlock()
{
	locked = false;
}

void Cg_NCZ_Queue::erase()
{
	this->lock();
	queue.clear();
	temp_queue.clear();
	this->unlock();
}

bool Cg_NCZ_Queue::empty()
{
	return queue.empty();
}

std::string Cg_NCZ_Queue::get()
{
	this->lock();
	std::string x = queue.front();
	queue.erase(queue.begin());
	this->unlock();
	return x;
}

void Cg_NCZ_Queue::send(std::string text)
{
	if(!this->isLocked())
	{
		queue.push_back(text);
	}
	else
	{
		temp_queue.push_back(text);
	}
}

void Cg_NCZ_Queue::update()
{
	while(!this->isLocked() && !temp_queue.empty())
	{
		queue.push_back(temp_queue.front());
		temp_queue.erase(temp_queue.begin());
	}
}

//---------------------------------------------------------------------------------
// Purpose: called when the plugin is unloaded (turned off)
//---------------------------------------------------------------------------------
void NoCheatZ::Unload( void )
{
	Msg("[NoCheatZ 3] Unloading ...\n");
	gameeventmanager->RemoveListener( this ); // make sure we are unloaded from the event system
	//xMsg("Waiting for threads to be terminated ...\n");
	must_thread_be_killed = true;
	//TheWebThread->ResumeExec();
	if(TheWebThread)
	{
		if(TheWebThread->IsAlive()) TheWebThread->Join();
	}
	//TheWebThread->Stop(0);
	if(engine)
	{
		std::string path = GAMEDIR"/addons/nocheatz/nocheatz.dat";
		std::ofstream fichier(path.c_str(), std::ios::out | std::ios::trunc);
		if(fichier)
		{
			fichier.clear();
			fichier.put('1');
			fichier.close();
		}
		else
		{
			Msg("[NoCheatZ 3] Can't write to datafile ...\n");
		}
	}
	//if(TheWebThread) delete TheWebThread;
	if(queue_send_report) delete queue_send_report;
	if(banlists) delete banlists;
#if !defined(NCZ_CSGO) && !defined(NCZ_CSP)
	if(entpropsmanager) delete entpropsmanager;
#endif
	UnhookCmd();
	RemoveTag("NoCheatZ3");
#ifndef NCZ_EP1
	ConVar_Unregister( );
#endif
	Msg("[NoCheatZ 3] Unloaded.\n");
#ifndef NCZ_EP1
	DisconnectTier1Libraries( );
#endif
}
