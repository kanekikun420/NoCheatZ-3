//===== Copyright © 2011, DJ Fire-Black production, All rights reserved. ======//
//
// Purpose:
//
// $NoKeywords: $
//
//===========================================================================//

//#include "player.h"
#include <functional>
#include <cstring>
#include <stdio.h>
#include <ctype.h>
#include <vector>
#include <algorithm>
#include <string>
#include <list>
#include <map>
#include "MRecipientFilter.h"
#include "bitbuf.h"
#include "interface.h"
#include "engine/iserverplugin.h"
#include "eiface.h"
#include "igameevents.h"
#include "convar.h"
#include "engine/IEngineTrace.h"
#include "tier2/tier2.h"
#ifndef NCZ_EP1
#include "game/server/iplayerinfo.h"
#else
#include "dlls/iplayerinfo.h"
#endif
#ifndef NCZ_CSGO
#include "util.h"
#endif
#include "AutoStatus.h"
#include "MostUsedPointers.h"
#ifdef WIN32
#include <tchar.h>
#else
#ifndef _LINUX
#define _LINUX
#endif
#include <wchar.h>
#define TCHAR wchar_t
#endif
#include <sstream>
//#include "server_class.h"
#include "edict.h"
#include "threadtools.h"
#include "inetchannelinfo.h"
#ifndef NCZ_EP1
#include "mathlib/vector.h"
#else
#include "vector.h"
#endif
//#include "C:\nocheatz3\hl2sdk-ob-valve-77da091662a6\utils\serverplugin_sample\dns.h"

typedef std::basic_string<TCHAR, std::char_traits<TCHAR>, std::allocator<TCHAR> > tstring;
typedef std::basic_ostringstream<TCHAR, std::char_traits<TCHAR>, std::allocator<TCHAR> > tostringstream;

template<typename T>
std::string tostring(const T & toConvert);
std::string to_lowercase(const std::string& s);

template<class T>
T *FindIFace(const char *IFaceName, CreateInterfaceFn Factory); // factory 

edict_t* getSourceTV();

bool isValidEdict(edict_t * entity);

void noTell(edict_t *pEdict, const char *msg); // Send message to all players but not to him (pEdict)

void noTell(edict_t *pEntity, const std::string& msg);

void ForceFile(const char *file);

void contell(edict_t *pEntity,const char * message);

void allTell(const char *message);

std::string formatSeconds(int sec);

bool CreateAThreadAndCallTheFunction();

std::string dumpConfig();

void setCVar(const char * cvarname, int value);

void setCVar(const char * cvarname, float value);

void setCVar(const char * cvarname, const char * value);

void tell(edict_t *pEntity,const char * message);

void tell(edict_t *pEntity, const std::string& message);

int getIndexFromUserID(int userid);

int IndexOfEdict(const edict_t *pEdict);

edict_t *PEntityOfEntIndex(int iEntIndex);

void messagetous(int joueursmax,const char * message);

void message(int index_joueur,const char * message);

//int GetOffset(std::string path);

bool between(const int n, const int min, const int max);

bool between(const float n, const float min, const float max);

bool ReallyUnknownCommand(const char *ccmd);

//void register_va_offset();

std::string NumString_Add(std::string num, std::string add);

void strSplit(std::string str, std::string separator, std::vector<string>* results);

std::string getCommunityID(const char *SteamID);

std::string getStrRates(int index);

void writeToLogfile(std::string text);

int getClosestTeleportEntDistanceFrom(float x, float y, float z);

float GetTime();

std::string getStrGameDir();

bool estImpaire(int num);

void AddTag(char* MyTag);

bool checkForUpdate_thread();

void RemoveTag(char* MyTag);

bool checkForUpdate();

bool isHackOrCheatCommand_Kick(const std::string& commandline);

bool isHackOrCheatCommand_Ban(const std::string& commandline);

std::string quoteQuotes(std::string str);

void SetTranslate(std::string key, std::string sentence);

std::string GetTranslate(const std::string& key);

std::string minStrf(std::string text, unsigned int minSize);

void needUseWeb();

bool estImpaire(int num);

void charencode (unsigned char ch, char dest[4]);

void strSplit(std::string str, std::string separator, std::vector<std::string>* results);

std::string urlencode (const std::string &str);

std::string getStrDateTime(const char *format);

char * nghost(char *host_name);

int WebThread();

void* WebThread_l(void* args);

/*
template<class T>
T & GetPVA(edict_t *pPlayer, bool angle_id);
template<class T>
T & GetGround(edict_t *pPlayer);*/

bool	CreateAThreadAndCallTheFunction();

float positiv(const float n); // Convert unknown float to only positive float
int positiv(const int n);

/*template<class T>
T & GetPVA(CBaseEntity *pBase);*/

std::string format(const char *fmt, ...); // format the specified text
const tchar * format(std::string fmt, ...);

#ifndef NCZ_EP1
#ifndef DEF_HOOKCLASS
#define DEF_HOOKCLASS
/* Prototype de la fonction à appeler lorsque la commande est utilisée
* La fonction pourra retourner true pour interdire l'exécution de la commande hookée
*/
typedef bool (* HookCallback)(int playerIndex, const CCommand & args);

/** Dérivée de ConCommand afin de surcharger ConCommand::Dispatch */
class ConCommandHook : public ConCommand
{
protected:
	/** Pointeur sur la commande hookée */
	ConCommand * hooked;

	/** Fonction à appeler lorsque la commande est utilisée */
	HookCallback callback;
public:
	ConCommandHook(const char * name, HookCallback hookCallback, int flags = FCVAR_NONE);

	// Fonctions héritées de ConCommand
	void Init();
	void Dispatch(const CCommand & args);
};

#endif
#endif

// ##########################################
// Cg_NCZ_PlayerManager
// ##########################################

#ifndef DEF_CGNCZPMANAGERCLASS
#define DEF_CGNCZPMANAGERCLASS
class Cg_NCZ_PlayerManager
{
public:
	// Inits

	Cg_NCZ_PlayerManager();
	~Cg_NCZ_PlayerManager();
	// Methods

	int getPlayerCount();
	int getMaxClients();
	Cg_NCZ_Player* getPlayer(int index);
	Cg_NCZ_Player* getPlayer(edict_t *pPlayer);
	Cg_NCZ_Player* getPlayer(void * myvoid); // CBaseEntity, CCSPlayer ...

	// Functions

	void resetAll();
	void lockAll();
	//void checkAllCvars();
	void checkAllEyeAngles();
	//void checkAllSpeedhacks();
	void checkAllBanRequest();
	void checkAllAimDirChange();
	void checkAllFire();
	void checkAllBunny();
	void updateAll();
	void disconnectPlayer(edict_t *pPlayer);
	void disconnectPlayer(int index);
	void resetAllUkCmd();
	void checkAllUkCmd();
	void fullcheckBanlists();
	void handleAllConvarTest();

	//void updateCheckList();

	std::vector<Cg_NCZ_Player*> g_ncz_playerList;
};

#endif

// ##########################################
// Cg_NCZ_RealPlayers
// ##########################################

/*#ifndef DEF_CGNCZREALP
#define DEF_CGNCZREALP
class Cg_NCZ_RealPlayers
{
public:
	// Inits

	Cg_NCZ_RealPlayers();
	~Cg_NCZ_RealPlayers();

	// Functions

	void addIndex(int index);
	void removeIndex(int index);
	
	// Methods

	bool isInList(int index);

private:
	std::vector<int> rpList;
};

#endif*/
#ifndef NCZPLUGIN
#define NCZPLUGIN

struct StrCompare : public std::binary_function<const char*, const char*, bool> {
public:
    bool operator() (const char* str1, const char* str2) const
    { return std::strcmp(str1, str2) < 0; }
};

typedef std::map<const char*, const char*, StrCompare> ConvarList;


//---------------------------------------------------------------------------------
// Purpose: a sample 3rd party plugin class
//---------------------------------------------------------------------------------
class NoCheatZ: public IServerPluginCallbacks, public IGameEventListener2, public AutoStatus
{
public:
	NoCheatZ();
	~NoCheatZ();

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
#ifndef NCZ_EP1
	virtual PLUGIN_RESULT	ClientCommand( edict_t *pEntity, const CCommand &args );
#else
	virtual PLUGIN_RESULT	ClientCommand( edict_t *pEntity );
#endif
	virtual PLUGIN_RESULT	NetworkIDValidated( const char *pszUserName, const char *pszNetworkID );
	virtual void			OnQueryCvarValueFinished( QueryCvarCookie_t iCookie, edict_t *pPlayerEntity, EQueryCvarValueStatus eStatus, const char *pCvarName, const char *pCvarValue );

	// added with version 3 of the interface.
	virtual void			OnEdictAllocated( edict_t *edict );
	virtual void			OnEdictFreed( const edict_t *edict  );
	//virtual void allstatus();
	//virtual void ForceUnload();

	// IGameEventListener Interface
	virtual void FireGameEvent( IGameEvent * event );
#ifdef NCZ_CSGO
	virtual int	 GetEventDebugID( void ) { return 42; };
#endif

	virtual int GetCommandIndex()
	{
		return m_iClientCommandIndex;
	}
	virtual Cg_NCZ_PlayerManager * get_m_pm();
	//Cg_NCZ_RealPlayers rpl;
private:
	int m_iClientCommandIndex;
	Cg_NCZ_PlayerManager *m_pm;
	ConvarList::iterator cit;
	int ccl;
	//float last_auto_status;
};

#endif

// ##########################################
// Cg_NCZ_Queue
// ##########################################

#ifndef DEF_Cg_NCZ_Queue
#define DEF_Cg_NCZ_Queue
class Cg_NCZ_Queue
{
public:
	Cg_NCZ_Queue();
	~Cg_NCZ_Queue();

	// Methods

	std::string get(); // Get the first element and delete the element after, so you have to register it.
	bool empty();
	void erase();
	// Functions

	void update();
	void send(std::string text);

private:
	// Methods

	bool isLocked();

	// Functions

	void lock();
	void unlock();

	// Vars

	std::vector<std::string> queue;
	std::vector<std::string> temp_queue;
	bool locked;
};
#endif

/*#ifdef WIN32
#ifndef ClassCNCZWebThread
#define ClassCNCZWebThread
class CNCZWebThread: public CWorkerThread
{
public:
	CNCZWebThread()
	{
		SetName( "NCZWebThread" );
	}

	~CNCZWebThread()
	{
	}

	enum
	{
		CALL_FUNC,
		EXIT,
	};

	bool	CallThreadFunction()
	{
		CallWorker( CALL_FUNC );
		return true;
	}

	int Run()
	{
		unsigned nCall;
		while ( WaitForCall( &nCall ) )
		{
			if ( nCall == EXIT )
			{
				Reply( 1 );
				break;
			}

			Reply( 1 );
			WebThread();

		}
		return 0;
	}

private:
};

static CNCZWebThread g_CNCZWebThread;
#endif
#endif*/
/*
// ##########################################
// Cg_NCZ_Lang
// ##########################################

typedef struct ncz_lang ncz_lang;
struct ncz_lang
{
	std::string textIndex;
	std::string dg;
};

#ifndef DEF_Cg_NCZ_Lang
#define DEF_Cg_NCZ_Lang
class Cg_NCZ_Lang
{
public:
	Cg_NCZ_Lang();
	~Cg_NCZ_Lang();

	// Methods

	std::string get(std::string textIndex); // Get the first element and delete the element after, so you have to register it.
	bool empty();

	// Functions

	void init(std::string PathToLangFile);
	void setLang(std::string Lang);

private:

};
#endif*/
/*
class CNczBasePlayer : public CBasePlayer
{
	CNczBasePlayer();
	virtual ~CNczBasePlayer();

	virtual int ShouldTransmit( const CCheckTransmitInfo *pInfo );
};*/
