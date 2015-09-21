//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: 
//
// $NoKeywords: $
//
//===========================================================================//

#include "Preprocessors.h"
#include "plugin.h"
#include "Players/NczPlayerManager.h"
#include "Misc/IFaceManager.h"
#include "Misc/Helpers.h"
#include "Systems/Testers/ValidationTester.h"
#include "Systems/Testers/JumpTester.h"
#include "Systems/Testers/EyeAnglesTester.h"
#include "Systems/Testers/ConVarTester.h"
#include "Systems/Testers/ShotTester.h"
#include "Systems/Testers/SpeedTester.h"
#include "Hooks/PlayerRunCommandHookListener.h"
#include "Hooks/OnGroundHookListener.h"
#include "Systems/BanRequest.h"
#include "Systems/Logger.h"

// 
// The plugin is a static singleton that is exported as an interface
//
CNoCheatZPlugin g_NoCheatZPlugin;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CNoCheatZPlugin, IServerPluginCallbacks, INTERFACEVERSION_ISERVERPLUGINCALLBACKS, g_NoCheatZPlugin );

//---------------------------------------------------------------------------------
// Purpose: constructor/destructor
//---------------------------------------------------------------------------------
CNoCheatZPlugin::CNoCheatZPlugin()
{
	m_iClientCommandIndex = 0;
	m_bAlreadyLoaded = false;
}

CNoCheatZPlugin::~CNoCheatZPlugin()
{
}

static ConVar nocheatz_instance("nocheatz_instance", "0");

void LoadTestersByConfig()
{
	EyeAnglesTester::GetInstance()->SetActive(true);
	EyeAnglesTester::GetInstance()->SetVerbose(false);

	JumpTester::GetInstance()->SetActive(true);
	JumpTester::GetInstance()->SetVerbose(false);

	ValidationTester::GetInstance()->SetActive(true);
	ValidationTester::GetInstance()->SetVerbose(false);

	ConVarTester::GetInstance()->SetActive(true);
	ConVarTester::GetInstance()->SetVerbose(false);

	ShotTester::GetInstance()->SetActive(true);
	ShotTester::GetInstance()->SetVerbose(true);

	SpeedTester::GetInstance()->SetActive(true);
	SpeedTester::GetInstance()->SetVerbose(true);
}

void HookBasePlayer(NczPlayer* player)
{
	OnGroundHookListener::HookOnGround(player);
	PlayerRunCommandHookListener::HookPlayerRunCommand(player);
	TeleportHookListener::HookTeleport(player);
}

//---------------------------------------------------------------------------------
// Purpose: called when the plugin is loaded, load the interface we need from the engine
//---------------------------------------------------------------------------------
bool CNoCheatZPlugin::Load(CreateInterfaceFn _interfaceFactory, CreateInterfaceFn gameServerFactory )
{
	ConnectTier1Libraries( &_interfaceFactory, 1 );
	if(g_pCVar->FindVar("nocheatz_instance"))
	{
		if(g_pCVar->FindVar("nocheatz_instance")->GetInt() == 1)
		{
			Error("[NoCheatZ 4] CNoCheatZPlugin - Error : CNoCheatZPlugin already loaded.\n");
			m_bAlreadyLoaded = true;
			return false;
		}
	}
	//ConnectTier2Libraries( &interfaceFactory, 1 );

	CIFaceManager::GetInstance()->UpdateIFactoryPtr(interfaceFactory, (void*)_interfaceFactory);
	CIFaceManager::GetInstance()->UpdateIFactoryPtr(gameFactory, (void*)gameServerFactory);
	NczPlayerManager::GetInstance()->LoadPlayerManager(); // Mark any present player as PLAYER_CONNECTED

	ConVar_Register( 0 );

	EyeAnglesTester::GetInstance();
	JumpTester::GetInstance();
	ValidationTester::GetInstance();
	ConVarTester::GetInstance();
	ShotTester::GetInstance();
	SpeedTester::GetInstance();

	CIFaceManager::GetInstance()->GetIengine()->ServerExecute();
	CIFaceManager::GetInstance()->GetIengine()->ServerCommand("exec nocheatz.cfg\n");
	CIFaceManager::GetInstance()->GetIengine()->ServerExecute();

	for(int i = 0; i < MAX_PLAYERS; ++i)
	{
		PlayerHandler* ph = NczPlayerManager::GetInstance()->GetPlayerHandlerByIndex(i);
		if(ph->status == PLAYER_CONNECTED)
		{
			HookBasePlayer(ph->playerClass);
			LoadTestersByConfig();
			ValidationTester::GetInstance()->ResetPlayerDataStruct(ph->playerClass);
			JumpTester::GetInstance()->ResetPlayerDataStruct(ph->playerClass);
			EyeAnglesTester::GetInstance()->ResetPlayerDataStruct(ph->playerClass);
			ConVarTester::GetInstance()->ResetPlayerDataStruct(ph->playerClass);
			ShotTester::GetInstance()->ResetPlayerDataStruct(ph->playerClass);
			SpeedTester::GetInstance()->ResetPlayerDataStruct(ph->playerClass);
		}
	}

	//---------------

	//MathLib_Init( 2.2f, 2.2f, 0.0f, 2 );
	
	g_pCVar->FindVar("nocheatz_instance")->SetValue("1");
	return true;
}

//---------------------------------------------------------------------------------
// Purpose: called when the plugin is unloaded (turned off)
//---------------------------------------------------------------------------------
void CNoCheatZPlugin::Unload( void )
{
	if(BanRequest::GetInstance()->NeedWriteID())
	{
		CIFaceManager::GetInstance()->GetIengine()->ServerCommand("writeid\n");
		CIFaceManager::GetInstance()->GetIengine()->ServerCommand("writeip\n");
		CIFaceManager::GetInstance()->GetIengine()->ServerExecute();
	}

	if(!m_bAlreadyLoaded && g_pCVar)
	{
		ConVar* inst = nullptr;
		inst = g_pCVar->FindVar("nocheatz_instance");
		if(inst) inst->SetValue("0");
	}

	ValidationTester::Delete();
#ifndef NCZ_CSGO
	JumpTester::Delete();
#endif
	EyeAnglesTester::Delete();
	ConVarTester::Delete();
	ShotTester::Delete();
	SpeedTester::Delete();
	NczPlayerManager::Delete();

	CIFaceManager::Delete();
	BanRequest::Delete();

	PlayerRunCommandHookListener::UnhookPlayerRunCommand();
	OnGroundHookListener::UnhookOnGround();
	TeleportHookListener::UnhookTeleport();

	ILogger.Flush();

	ConVar_Unregister( );
	//DisconnectTier2Libraries( );
	DisconnectTier1Libraries( );
}

//---------------------------------------------------------------------------------
// Purpose: called when the plugin is paused (i.e should stop running but isn't unloaded)
//---------------------------------------------------------------------------------
void CNoCheatZPlugin::Pause( void )
{
}

//---------------------------------------------------------------------------------
// Purpose: called when the plugin is unpaused (i.e should start executing again)
//---------------------------------------------------------------------------------
void CNoCheatZPlugin::UnPause( void )
{
}

//---------------------------------------------------------------------------------
// Purpose: the name of this plugin, returned in "plugin_print" command
//---------------------------------------------------------------------------------
const char *CNoCheatZPlugin::GetPluginDescription( void )
{
	return NCZ_PLUGIN_NAME " v" NCZ_VERSION_STR;
}

//---------------------------------------------------------------------------------
// Purpose: called on level start
//---------------------------------------------------------------------------------
void CNoCheatZPlugin::LevelInit( char const *pMapName )
{
	ILogger.Flush();
}

//---------------------------------------------------------------------------------
// Purpose: called on level start, when the server is ready to accept client connections
//		edictCount is the number of entities in the level, clientMax is the max client count
//---------------------------------------------------------------------------------
void CNoCheatZPlugin::ServerActivate( edict_t *pEdictList, int edictCount, int clientMax )
{
	Helpers::m_EdictList = pEdictList;
	//Helpers::m_edictCount = edictCount;
	//Helpers::m_clientMax = clientMax;

	NczPlayerManager::GetInstance()->LoadPlayerManager();
}

//---------------------------------------------------------------------------------
// Purpose: called once per server frame, do recurring work here (like checking for timeouts)
//---------------------------------------------------------------------------------
void CNoCheatZPlugin::GameFrame( bool simulating )
{
	BaseFramedTester::OnFrame();
	BanRequest::GetInstance()->Think();
}

//---------------------------------------------------------------------------------
// Purpose: called on level end (as the server is shutting down or going to a new map)
//---------------------------------------------------------------------------------
void CNoCheatZPlugin::LevelShutdown( void ) // !!!!this can get called multiple times per map change
{
	if(BanRequest::GetInstance()->NeedWriteID())
	{
		CIFaceManager::GetInstance()->GetIengine()->ServerCommand("writeid\n");
		CIFaceManager::GetInstance()->GetIengine()->ServerCommand("writeip\n");
		CIFaceManager::GetInstance()->GetIengine()->ServerExecute();
	}
	BaseSystem::UnloadAllSystems();
	ILogger.Flush();
}

//---------------------------------------------------------------------------------
// Purpose: called when a client spawns into a server (i.e as they begin to play)
//---------------------------------------------------------------------------------
void CNoCheatZPlugin::ClientActive( edict_t *pEntity )
{
	Msg("%f : CNoCheatZPlugin::ClientActive\n", Plat_FloatTime());
	NczPlayerManager::GetInstance()->ClientActive(pEntity);

	PlayerHandler* ph = NczPlayerManager::GetInstance()->GetPlayerHandlerByEdict(pEntity);
	if(ph->status > BOT) HookBasePlayer(ph->playerClass);
}

//---------------------------------------------------------------------------------
// Purpose: called when a client leaves a server (or is timed out)
//---------------------------------------------------------------------------------
void CNoCheatZPlugin::ClientDisconnect( edict_t *pEntity )
{
	NczPlayerManager::GetInstance()->ClientDisconnect(pEntity);
    if(		NczPlayerManager::GetInstance()->GetPlayerCount(PLAYER_CONNECTED) 
		  + NczPlayerManager::GetInstance()->GetPlayerCount(PLAYER_IN_TESTS)
										==
			                                                        0   )
	{
		BaseSystem::UnloadAllSystems();
	}
}

//---------------------------------------------------------------------------------
// Purpose: called on 
//---------------------------------------------------------------------------------
void CNoCheatZPlugin::ClientPutInServer( edict_t *pEntity, char const *playername )
{
}

//---------------------------------------------------------------------------------
// Purpose: called on level start
//---------------------------------------------------------------------------------
void CNoCheatZPlugin::SetCommandClient( int index )
{
	m_iClientCommandIndex = index;
}

//---------------------------------------------------------------------------------
// Purpose: called on level start
//---------------------------------------------------------------------------------
void CNoCheatZPlugin::ClientSettingsChanged( edict_t *pEdict )
{
}

//---------------------------------------------------------------------------------
// Purpose: called when a client joins a server
//---------------------------------------------------------------------------------
PLUGIN_RESULT CNoCheatZPlugin::ClientConnect( bool *bAllowConnect, edict_t *pEntity, const char *pszName, const char *pszAddress, char *reject, int maxrejectlen )
{
#define MAX_CHARS_NAME 32
	Msg("%f - CNoCheatZPlugin::ClientConnect (%s, %s)\n", Plat_FloatTime(), pszName, pszAddress);

	// Scan for overflow in name
	int char_count = 0;
	for(;pszName[char_count] != '\0' && char_count <= MAX_CHARS_NAME; ++char_count);
	if(char_count == MAX_CHARS_NAME)
	{
		*bAllowConnect = false;
		strcpy(reject, "Buffer overflow in name.");
		return PLUGIN_STOP;
	}
	if(char_count > 32)
	{
		*bAllowConnect = false;
		strcpy(reject, "Your name is too long.");
		return PLUGIN_STOP;
	}
	if(Helpers::bStrEq(pszName, "unnamed") || Helpers::bStrEq(pszName, "disconnected"))
	{
		*bAllowConnect = false;
		strcpy(reject, "\0"); // No name, no message. Sorry. :)
		return PLUGIN_STOP;
	}

	NczPlayerManager::GetInstance()->ClientConnect(pEntity);
	NczPlayer* player = NczPlayerManager::GetInstance()->GetPlayerHandlerByEdict(pEntity)->playerClass;
	
	LoadTestersByConfig();
	
	ValidationTester::GetInstance()->ResetPlayerDataStruct(player);
	JumpTester::GetInstance()->ResetPlayerDataStruct(player);
	EyeAnglesTester::GetInstance()->ResetPlayerDataStruct(player);
	ConVarTester::GetInstance()->ResetPlayerDataStruct(player);
	ShotTester::GetInstance()->ResetPlayerDataStruct(player);
	SpeedTester::GetInstance()->ResetPlayerDataStruct(player);
	return PLUGIN_CONTINUE;
}

//---------------------------------------------------------------------------------
// Purpose: called when a client types in a command (only a subset of commands however, not CON_COMMAND's)
//---------------------------------------------------------------------------------
PLUGIN_RESULT CNoCheatZPlugin::ClientCommand( edict_t *pEntity, const CCommand &args )
{
	Msg("ClientCommand(%x, %s)\n", pEntity, args.GetCommandString());
	
	if ( !pEntity || pEntity->IsFree() ) 
	{
		return PLUGIN_CONTINUE;
	}

	NczPlayer* myPlayer = NczPlayerManager::GetInstance()->GetPlayerHandlerByEdict(pEntity)->playerClass;
	/*ActionID result = HashTable::GetInstance()->ScanCommandString(args.GetCommandString());
	if(result > SAFE)
	{
		if(result == BAN)
		{
			myPlayer->Ban();
		}
		if(result == KICK)
		{
			myPlayer->Kick();
		}
		return PLUGIN_STOP;
	}*/

	return PLUGIN_CONTINUE;
}

//---------------------------------------------------------------------------------
// Purpose: called when a client is authenticated
//---------------------------------------------------------------------------------
PLUGIN_RESULT CNoCheatZPlugin::NetworkIDValidated( const char *pszUserName, const char *pszNetworkID )
{
	NczPlayer* player = NczPlayerManager::GetInstance()->GetPlayerHandlerBySteamID(pszNetworkID)->playerClass;
	ValidationTester::GetInstance()->SetValidated(player);
	
	return PLUGIN_CONTINUE;
}

//---------------------------------------------------------------------------------
// Purpose: called when a cvar value query is finished
//---------------------------------------------------------------------------------
void CNoCheatZPlugin::OnQueryCvarValueFinished( QueryCvarCookie_t iCookie, edict_t *pPlayerEntity, EQueryCvarValueStatus eStatus, const char *pCvarName, const char *pCvarValue )
{
	PlayerHandler* ph = NczPlayerManager::GetInstance()->GetPlayerHandlerByEdict(pPlayerEntity);
	if(ph->status < ConVarTester::GetInstance()->GetFilter()) return;

	ConVarTester::GetInstance()->OnQueryCvarValueFinished(ph->playerClass, eStatus, pCvarName, pCvarValue);
}

#ifdef NCZ_CSGO
void CNoCheatZPlugin::ClientFullyConnect(edict_t *)
{

}
#endif

void CNoCheatZPlugin::OnEdictAllocated( edict_t *edict )
{
	//Msg("OnEdictAllocated(%x)\n", edict);
}
void CNoCheatZPlugin::OnEdictFreed( const edict_t *edict  )
{
	//Msg("OnEdictFreed(%x)\n", edict);
}
