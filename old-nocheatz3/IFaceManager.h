#include "interface.h"

class IVEngineServer;
class IGameEventManager2;
class IPlayerInfoManager;
class IServerPluginHelpers;
class IServerGameDLL;
class IServerGameEnts;
class IServerGameClients;
class EntityPropsManager;
class IEngineTrace;
class CGlobalVars;

enum interfaceOffset
{
	ENGINE,
	EVENT,
	HELPERS,
	DLL,
	ENTS,
	CLIENTS,
	ENTITYPROPS,
	TRACE,
	GVARS
}

#ifndef IFaceManagerClass
#define IFaceManagerClass

class IFaceManager
{
private:
	IVEngineServer *           engine;
	IGameEventManager2 *       gameeventmanager;
	IPlayerInfoManager *       playerinfomanager;
	IServerPluginHelpers *     helpers;
	IServerGameDLL *           gamedll;
	IServerGameEnts *          gameents;
	IServerGameClients *       gameclients;
	EntityPropsManager *       entpropsmanager;
	IEngineTrace *             enginetrace;
	CGlobalVars*               gpGlobals;
	CreateInterfaceFn gameFactory;
	CreateInterfaceFn interfaceFactory;
	bool isinit;

	template<class T>
	T *FindIFace(const char *IFaceName, CreateInterfaceFn Factory);

public:
	IFaceManager();
	~IFaceManager();

	void Init(CreateInterfaceFn gameFactory, CreateInterfaceFn interfaceFactory);
	template<class T>
	T *GetIFace(interfaceOffset offset);
};

#endif