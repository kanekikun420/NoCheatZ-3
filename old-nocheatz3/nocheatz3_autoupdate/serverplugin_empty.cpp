//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: 
//
// $NoKeywords: $
//
//===========================================================================//

#include "interface.h"
#include "engine/iserverplugin.h"
#include "eiface.h"
#include "tier1.h"
#include "convar.h"
#include "strtools.h"
#include "../Preprocessors.h"

//#include "Misc.h"
#include "NczHttpSockHandler.h"
#include "NczThread.h"
#include "BaseDef.h"

#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include "stdarg.h"

// memdbgon must be the last include file in a .cpp file!!!
//#include "tier0/memdbgon.h"

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

template<typename T>
std::string tostring(const T & toConvert)
{
	std::stringstream convertion;
	convertion << toConvert;
	std::string str(convertion.str());
	return str;
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


// Interfaces from the engine
IVEngineServer	*engine = NULL;
//ICvar *g_pCVar;
NczHttpSockHandler *UpdateSock = NULL;
NczHttpSockHandler *DownloadSock = NULL;
NczThread *UpdateThread = NULL;

int UpdateThread_job();

bool isThreadAlive = true;

bool plugin_loaded = false;

//---------------------------------------------------------------------------------
// Purpose: a sample 3rd party plugin class
//---------------------------------------------------------------------------------
class CEmptyServerPlugin: public IServerPluginCallbacks
{
public:
	CEmptyServerPlugin();
	~CEmptyServerPlugin();

	// IServerPluginCallbacks methods
	virtual bool			Load(	CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory );
	virtual void			Unload( void );
	virtual void			Pause( void );
	virtual void			UnPause( void );
	virtual const char     *GetPluginDescription( void );      
	virtual void			LevelInit( char const *pMapName );
	virtual void			ServerActivate( edict_t *pEdictList, int edictCount, int clientMax );
	virtual void			GameFrame( bool simulating );
	virtual void			LevelShutdown( void );
	virtual void			ClientActive( edict_t *pEntity );
#ifdef NCZ_CSGO
	virtual void            ClientFullyConnect(edict_t *) {};
#endif
	virtual void			ClientDisconnect( edict_t *pEntity );
	virtual void			ClientPutInServer( edict_t *pEntity, char const *playername );
	virtual void			SetCommandClient( int index );
	virtual void			ClientSettingsChanged( edict_t *pEdict );
	virtual PLUGIN_RESULT	ClientConnect( bool *bAllowConnect, edict_t *pEntity, const char *pszName, const char *pszAddress, char *reject, int maxrejectlen );
	virtual PLUGIN_RESULT	ClientCommand( edict_t *pEntity, const CCommand &args );
	virtual PLUGIN_RESULT	NetworkIDValidated( const char *pszUserName, const char *pszNetworkID );
	virtual void			OnQueryCvarValueFinished( QueryCvarCookie_t iCookie, edict_t *pPlayerEntity, EQueryCvarValueStatus eStatus, const char *pCvarName, const char *pCvarValue );

	virtual void			OnEdictAllocated( edict_t *edict ) {};
	virtual void			OnEdictFreed( const edict_t *edict  ) {};

#ifdef NCZ_CSGO
	virtual int	 GetEventDebugID( void ) { return 42; };
#endif

	virtual int GetCommandIndex() { return m_iClientCommandIndex; }
private:
	int m_iClientCommandIndex;
};


// 
// The plugin is a static singleton that is exported as an interface
//
CEmptyServerPlugin g_EmtpyServerPlugin;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CEmptyServerPlugin, IServerPluginCallbacks, INTERFACEVERSION_ISERVERPLUGINCALLBACKS, g_EmtpyServerPlugin );

//---------------------------------------------------------------------------------
// Purpose: constructor/destructor
//---------------------------------------------------------------------------------
CEmptyServerPlugin::CEmptyServerPlugin()
{
	m_iClientCommandIndex = 0;
}

CEmptyServerPlugin::~CEmptyServerPlugin()
{
}

//---------------------------------------------------------------------------------
// Purpose: called when the plugin is loaded, load the interface we need from the engine
//---------------------------------------------------------------------------------
bool CEmptyServerPlugin::Load(	CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory )
{
	ConnectTier1Libraries( &interfaceFactory, 1 );
	engine            = FindIFace<IVEngineServer>       ("VEngineServer",        interfaceFactory);
	if(!engine) return false;

	//if(UpdateThread) delete[] UpdateThread;
	if(!UpdateThread) UpdateThread = new NczThread();
	if(UpdateThread) UpdateThread->Start();
	else return false;

	return true;
}

int UpdateThread_job()
{
	// Prepare vars
	bool needupdate = false;
	const char * domain = PIPELINE_ADDRESS;
	std::string page = "/update.php";
	int version = 0;
	std::ifstream fichierv(GAMEDIR"/addons/nocheatz/version.txt", std::ios::in);
	if(fichierv)
	{
		version = 0;
		fichierv >> version;
		printf("[NoCheatZ 3 AutoUpdate] Your version : %d.\n", version);
		fichierv.close();
	}
	else
	{
		needupdate = true;
	}
	std::string filename = "nocheatz3."NCZ_EXT;
	std::string pathtofile = GAMEDIR"/addons/nocheatz/" + filename;
	std::ifstream fichier(pathtofile.c_str(), std::ios::in);
	if(fichier)
	{
		fichier.close();
	}
	else
	{
		needupdate = true;
	}
	UpdateSock = new NczHttpSockHandler();
	int iNewVersion = version;
	if(UpdateSock->Connect(domain, 80))
	{
		UpdateSock->SendGet(page);
		std::string resp = UpdateSock->GetContent();
		//printf("Resp : %s\n", resp.c_str());
		iNewVersion = atoi(resp.c_str());
		if(iNewVersion == 0)
		{
			printf("[NoCheatZ 3 AutoUpdate] Unable to get in touch with the master server.\n", iNewVersion);
			isThreadAlive = false;
			return 0;
		}
		else printf("[NoCheatZ 3 AutoUpdate] Last version : %d.\n", iNewVersion);
	}
	UpdateSock->End();
	delete UpdateSock;
	if(iNewVersion > version)
	{
		needupdate = true;
		printf("[NoCheatZ 3 AutoUpdate] Update needed.\n", iNewVersion);
	}
	else
	{
		printf("[NoCheatZ 3 AutoUpdate] No update needed.\n", iNewVersion);
	}
	if(needupdate)
	{
		DownloadSock = new NczHttpSockHandler();
		if(DownloadSock->Connect(domain, 80))
		{
			std::string remotefile = "/bin/" + filename;
			std::string localfile = GAMEDIR"/addons/nocheatz/" + filename;
			printf("[NoCheatZ 3 AutoUpdate] Downloading new plugin ...\n");
			DownloadSock->SendGet(remotefile);
			DownloadSock->WriteBinaryContentTo(localfile);
			std::ofstream fichier(GAMEDIR"/addons/nocheatz/version.txt", std::ios::out | std::ios::trunc);
			if(fichier)
			{
				fichier << iNewVersion;
				fichier.close();
			}
		}
		delete DownloadSock;
		printf("[NoCheatZ 3 AutoUpdate] Update done.\n");
	}
	isThreadAlive = false;
	return 0;
}

//---------------------------------------------------------------------------------
// Purpose: called when the plugin is unloaded (turned off)
//---------------------------------------------------------------------------------
void CEmptyServerPlugin::Unload( void )
{
	DisconnectTier1Libraries();
}

//---------------------------------------------------------------------------------
// Purpose: called when the plugin is paused (i.e should stop running but isn't unloaded)
//---------------------------------------------------------------------------------
void CEmptyServerPlugin::Pause( void )
{
}

//---------------------------------------------------------------------------------
// Purpose: called when the plugin is unpaused (i.e should start executing again)
//---------------------------------------------------------------------------------
void CEmptyServerPlugin::UnPause( void )
{
}

//---------------------------------------------------------------------------------
// Purpose: the name of this plugin, returned in "plugin_print" command
//---------------------------------------------------------------------------------
const char *CEmptyServerPlugin::GetPluginDescription( void )
{
	return "NoCheatZ 3 AutoUpdate";
}

//---------------------------------------------------------------------------------
// Purpose: called on level start
//---------------------------------------------------------------------------------
void CEmptyServerPlugin::LevelInit( char const *pMapName )
{
}

//---------------------------------------------------------------------------------
// Purpose: called on level start, when the server is ready to accept client connections
//		edictCount is the number of entities in the level, clientMax is the max client count
//---------------------------------------------------------------------------------
void CEmptyServerPlugin::ServerActivate( edict_t *pEdictList, int edictCount, int clientMax )
{
}

//---------------------------------------------------------------------------------
// Purpose: called once per server frame, do recurring work here (like checking for timeouts)
//---------------------------------------------------------------------------------
void CEmptyServerPlugin::GameFrame( bool simulating )
{
	if(!plugin_loaded && !isThreadAlive && engine)
	{
		/*std::string filename = "nocheatz3."NCZ_EXT;
		std::string localfile = "addons/nocheatz/" + filename;*/
		//std::string command = "plugin_load addons/nocheatz/nocheatz3\n";
		printf("[NoCheatZ 3 AutoUpdate] Trying to load NoCheatZ 3 ...\n");
#ifndef NCZ_CSP
		engine->ServerCommand("plugin_load addons/nocheatz/nocheatz3\n");
#else
#ifdef WIN32
		engine->ServerCommand("plugin_load addons/nocheatz/nocheatz3.dll\n");
#else
		engine->ServerCommand("plugin_load addons/nocheatz/nocheatz3.so\n");
#endif
#endif
		plugin_loaded = true;
	}
	//UpdateThread->ResumeExec();
	//if(isThreadAlive) Msg("Thread Alive\n");
}

//---------------------------------------------------------------------------------
// Purpose: called on level end (as the server is shutting down or going to a new map)
//---------------------------------------------------------------------------------
void CEmptyServerPlugin::LevelShutdown( void ) // !!!!this can get called multiple times per map change
{
}

//---------------------------------------------------------------------------------
// Purpose: called when a client spawns into a server (i.e as they begin to play)
//---------------------------------------------------------------------------------
void CEmptyServerPlugin::ClientActive( edict_t *pEntity )
{
}

//---------------------------------------------------------------------------------
// Purpose: called when a client leaves a server (or is timed out)
//---------------------------------------------------------------------------------
void CEmptyServerPlugin::ClientDisconnect( edict_t *pEntity )
{
}

//---------------------------------------------------------------------------------
// Purpose: called on 
//---------------------------------------------------------------------------------
void CEmptyServerPlugin::ClientPutInServer( edict_t *pEntity, char const *playername )
{
}

//---------------------------------------------------------------------------------
// Purpose: called on level start
//---------------------------------------------------------------------------------
void CEmptyServerPlugin::SetCommandClient( int index )
{
	m_iClientCommandIndex = index;
}

//---------------------------------------------------------------------------------
// Purpose: called on level start
//---------------------------------------------------------------------------------
void CEmptyServerPlugin::ClientSettingsChanged( edict_t *pEdict )
{
}

//---------------------------------------------------------------------------------
// Purpose: called when a client joins a server
//---------------------------------------------------------------------------------
PLUGIN_RESULT CEmptyServerPlugin::ClientConnect( bool *bAllowConnect, edict_t *pEntity, const char *pszName, const char *pszAddress, char *reject, int maxrejectlen )
{
	return PLUGIN_CONTINUE;
}

//---------------------------------------------------------------------------------
// Purpose: called when a client types in a command (only a subset of commands however, not CON_COMMAND's)
//---------------------------------------------------------------------------------
PLUGIN_RESULT CEmptyServerPlugin::ClientCommand( edict_t *pEntity, const CCommand &args )
{
	return PLUGIN_CONTINUE;
}

//---------------------------------------------------------------------------------
// Purpose: called when a client is authenticated
//---------------------------------------------------------------------------------
PLUGIN_RESULT CEmptyServerPlugin::NetworkIDValidated( const char *pszUserName, const char *pszNetworkID )
{
	return PLUGIN_CONTINUE;
}

//---------------------------------------------------------------------------------
// Purpose: called when a cvar value query is finished
//---------------------------------------------------------------------------------
void CEmptyServerPlugin::OnQueryCvarValueFinished( QueryCvarCookie_t iCookie, edict_t *pPlayerEntity, EQueryCvarValueStatus eStatus, const char *pCvarName, const char *pCvarValue )
{
}
