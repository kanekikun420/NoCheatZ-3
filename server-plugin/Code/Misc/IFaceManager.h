#include "Preprocessors.h"
#include "interface.h"
#include "Types.h"
#include "eiface.h"
#include "igameevents.h"
#include "game/server/iplayerinfo.h"
#include "engine/iserverplugin.h"
#include "engine/IEngineTrace.h"
#include "temp_Singleton.h"

#ifndef IFaceManagerClass
#define IFaceManagerClass

enum IFactoryIndex
{
	gameFactory = 0,
	interfaceFactory
};

enum IFaceIndex
{
	EngineServer = 2,
	EventManager2,
	PlayerInfoManager,
	ServerPluginHelpers,
	GameDLL,
	GameEnts,
	GameClients,
	Globals,
	Trace
};

struct IFaceElement
{
	ptr IFacePtr;
	const char *name;
	IFactoryIndex factory;
	unsigned short version;
	unsigned short compiled_version;
};

#define INTERFACES_COUNT 12

class CIFaceManager : public Singleton<CIFaceManager>
{
public:
	CIFaceManager();
	~CIFaceManager();

#ifndef IFACEMANAGER_AUTOINIT
	IVEngineServer* GetIengine();
	IGameEventManager2* GetIevents();
	IPlayerInfoManager* GetIplayers();
	IServerPluginHelpers* GetIpluginhelpers();
	IServerGameDLL* GetIgamedll();
	IEngineTrace* GetItrace();
	IServerGameEnts* GetIents();
	IServerGameClients* GetIclients();
	CGlobalVars* GetGlobals();
	virtual ptr GetIFace(IFaceIndex Index);
#else
	IVEngineServer* GetIengine() const;
	IGameEventManager2* GetIevents() const;
	IPlayerInfoManager* GetIplayers() const;
	IServerPluginHelpers* GetIpluginhelpers() const;
	IServerGameDLL* GetIgamedll() const;
	IEngineTrace* GetItrace() const;
	IServerGameEnts* GetIents() const;
	IServerGameClients* GetIclients() const;
	CGlobalVars* GetGlobals() const;
	ptr GetIFace(IFaceIndex Index) const;
#endif

	void UpdateIFactoryPtr(IFactoryIndex Index, ptr newPtr);


	ptr				_GetIFacePtr(IFaceIndex Index) const;
	ptr				_GetIFaceFactoryPtr(IFaceIndex Index) const;
	ptr				_GetFactoryPtr(IFactoryIndex Index) const;
private:
	void			_UpdateIFacePtr(IFaceIndex Index, ptr newPtr);
	void			_UpdateIFaceName(IFaceIndex Index, const char * name);
	const char *	_GetIFaceName(IFaceIndex Index) const;

	IFaceElement IFaces[INTERFACES_COUNT];
};

#endif
