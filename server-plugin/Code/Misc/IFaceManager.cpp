#include "IFaceManager.h"

////////////////////////////////////////
CIFaceManager::~CIFaceManager()
{
}
////////////////////////////////////////

CIFaceManager::CIFaceManager()
{
	IFaces[gameFactory].name = "CreateInterfaceFn gameFactory";
	IFaces[interfaceFactory].name = "CreateInterfaceFn interfaceFactory";
	IFaces[EngineServer].name = "VEngineServer";
	IFaces[EventManager2].name = "GAMEEVENTSMANAGER";
	IFaces[PlayerInfoManager].name = "PlayerInfoManager";
	IFaces[ServerPluginHelpers].name = "ISERVERPLUGINHELPERS";
	IFaces[GameDLL].name = "ServerGameDLL";
	IFaces[GameEnts].name = "ServerGameEnts";
	IFaces[GameClients].name = "ServerGameClients";
	IFaces[Trace].name = "EngineTraceServer";

#ifdef NCZ_CSS
	IFaces[EngineServer].compiled_version = 23;
	IFaces[EventManager2].compiled_version = 2;
	IFaces[PlayerInfoManager].compiled_version = 2;
	IFaces[ServerPluginHelpers].compiled_version = 1;
	IFaces[GameDLL].compiled_version = 10;
	IFaces[GameEnts].compiled_version = 1;
	IFaces[GameClients].compiled_version = 4;
	IFaces[Trace].compiled_version = 3;
#endif // NCZ_CSS

	IFaces[gameFactory].factory = gameFactory;
	IFaces[interfaceFactory].factory = interfaceFactory;
	IFaces[EngineServer].factory = interfaceFactory;
	IFaces[EventManager2].factory = interfaceFactory;
	IFaces[PlayerInfoManager].factory = gameFactory;
	IFaces[ServerPluginHelpers].factory = interfaceFactory;
	IFaces[GameDLL].factory = gameFactory;
	IFaces[GameEnts].factory = gameFactory;
	IFaces[GameClients].factory = gameFactory;
	IFaces[Trace].factory = interfaceFactory;

	for(int x = 0; x < INTERFACES_COUNT; ++x)
	{
		IFaces[x].version = 0;
		IFaces[x].IFacePtr = nullptr;
	}
}

ptr CIFaceManager::_GetIFacePtr(IFaceIndex Index) const
{
	return IFaces[Index].IFacePtr;
}

const char * CIFaceManager::_GetIFaceName(IFaceIndex Index) const
{
	return IFaces[Index].name;
}

ptr CIFaceManager::_GetFactoryPtr(IFactoryIndex Index) const
{
	return IFaces[Index].IFacePtr;
}

ptr CIFaceManager::_GetIFaceFactoryPtr(IFaceIndex Index) const
{
	return _GetFactoryPtr(IFaces[Index].factory);
}

void CIFaceManager::_UpdateIFacePtr(IFaceIndex Index, ptr newPtr)
{
	IFaces[Index].IFacePtr = newPtr;
}

void CIFaceManager::UpdateIFactoryPtr(IFactoryIndex Index, ptr newPtr)
{
	Msg("[NoCheatZ 4] CIFaceManager - Info : Registering %s at %x.\n", IFaces[Index].name, newPtr);
	IFaces[Index].IFacePtr = newPtr;

#ifdef IFACEMANAGER_AUTOINIT
	if(IFaces[interfaceFactory].IFacePtr && IFaces[gameFactory].IFacePtr)
	{
		for(int x = EngineServer; x <= Trace; x++)
		{
			if(IFaces[x].IFacePtr != nullptr) continue;
			if(x == Globals) continue;

			ptr pIFace = nullptr;
			CreateInterfaceFn Factory = reinterpret_cast<CreateInterfaceFn>(_GetFactoryPtr(IFaces[x].factory));

			int i_v = 40;
			char s_v[64];
			while ( !pIFace )
			{
				--i_v;
				if(i_v == 0)
				{
					pIFace = nullptr;
					break;
				}
				if(i_v < 10)
				{
					sprintf(s_v, "%s00%d\0", IFaces[x].name, i_v);
				}
				else if(i_v > 9)
				{
					sprintf(s_v, "%s0%d\0", IFaces[x].name, i_v);
				}
				pIFace = (ptr)Factory(s_v, nullptr);
			}
			IFaces[x].version = i_v;
			IFaces[x].IFacePtr = pIFace;
			Msg("[NoCheatZ 4] CIFaceManager - Info : Registering interface %s at %x.\n", s_v, IFaces[x].IFacePtr);
			Assert(IFaces[x].version == IFaces[x].compiled_version);
			if(IFaces[x].version != IFaces[x].compiled_version) Msg("CIFaceManager - Warning : Using other version of interface %s.\n", IFaces[x].name);
		}
		if(this->GetIplayers() && IFaces[Globals].IFacePtr == nullptr)
		{
			Msg("[NoCheatZ 4] CIFaceManager - Info : Registering GlobalVars at %x.\n", this->GetIplayers()->GetGlobalVars());
			IFaces[Globals].IFacePtr = this->GetIplayers()->GetGlobalVars();
		}

	}
#endif
}

void CIFaceManager::_UpdateIFaceName(IFaceIndex Index, const char * name)
{
	IFaces[Index].name = name;
}

#ifndef IFACEMANAGER_AUTOINIT
ptr CIFaceManager::GetIFace(IFaceIndex Index)
{
	ptr pIFace = nullptr;
	CreateInterfaceFn Factory = reinterpret_cast<CreateInterfaceFn>(_GetIFaceFactoryPtr(Index));
	if(_GetIFacePtr(Index) != nullptr) return _GetIFacePtr(Index);

	int i_v = 40;
	char s_v[64];
	while ( !pIFace )
	{
		--i_v;
		if(i_v < 0)
		{
			return nullptr;
		}
		if(i_v < 10)
		{
			sMsg(s_v, "%s00%d", IFaces[Index].name, i_v);
		}
		else if(i_v > 9)
		{
			sMsg(s_v, "%s0%d", IFaces[Index].name, i_v);
		}
		pIFace = (ptr)Factory(s_v, nullptr);
	}
	IFaces[Index].version = i_v;
	_UpdateIFacePtr(Index, pIFace);
	return pIFace;
}

IVEngineServer* CIFaceManager::GetIengine()
{
	return reinterpret_cast<IVEngineServer*>(GetIFace(IFaceIndex::EngineServer));
}

IGameEventManager2* CIFaceManager::GetIevents()
{
	return reinterpret_cast<IGameEventManager2*>(GetIFace(IFaceIndex::EventManager2));
}

IPlayerInfoManager* CIFaceManager::GetIplayers()
{
	return reinterpret_cast<IPlayerInfoManager*>(GetIFace(IFaceIndex::PlayerInfoManager));
}

IServerPluginHelpers* CIFaceManager::GetIpluginhelpers()
{
	return reinterpret_cast<IServerPluginHelpers*>(GetIFace(IFaceIndex::ServerPluginHelpers));
}

IServerGameDLL* CIFaceManager::GetIgamedll()
{
	return reinterpret_cast<IServerGameDLL*>(GetIFace(IFaceIndex::GameDLL));
}

IEngineTrace* CIFaceManager::GetItrace()
{
	return reinterpret_cast<IEngineTrace*>(GetIFace(IFaceIndex::Trace));
}

IServerGameEnts* CIFaceManager::GetIents()
{
	return reinterpret_cast<IServerGameEnts*>(GetIFace(IFaceIndex::GameEnts));
}

IServerGameClients* CIFaceManager::GetIclients()
{
	return reinterpret_cast<IServerGameClients*>(GetIFace(IFaceIndex::GameClients));
}

CGlobalVars* CIFaceManager::GetGlobals()
{
	if((ptr)(GetIFace(IFaceIndex::Globals))) return reinterpret_cast<CGlobalVars*>(GetIFace(IFaceIndex::Globals));
	if((ptr)(GetIFace(IFaceIndex::PlayerInfoManager)))
	{
		if(GetIplayers()->GetGlobalVars())
		{
			_UpdateIFacePtr(IFaceIndex::Globals, GetIplayers()->GetGlobalVars());
			return GetIplayers()->GetGlobalVars();
		}
	}
	return nullptr;
}

#else

ptr CIFaceManager::GetIFace(IFaceIndex Index) const
{
	return _GetIFacePtr(Index);
}

IVEngineServer* CIFaceManager::GetIengine() const
{
	return reinterpret_cast<IVEngineServer*>(GetIFace(EngineServer));
}

IGameEventManager2* CIFaceManager::GetIevents() const
{
	return reinterpret_cast<IGameEventManager2*>(GetIFace(EventManager2));
}

IPlayerInfoManager* CIFaceManager::GetIplayers() const
{
	return reinterpret_cast<IPlayerInfoManager*>(GetIFace(PlayerInfoManager));
}

IServerPluginHelpers* CIFaceManager::GetIpluginhelpers() const
{
	return reinterpret_cast<IServerPluginHelpers*>(GetIFace(ServerPluginHelpers));
}

IServerGameDLL* CIFaceManager::GetIgamedll() const
{
	return reinterpret_cast<IServerGameDLL*>(GetIFace(GameDLL));
}

IEngineTrace* CIFaceManager::GetItrace() const
{
	return reinterpret_cast<IEngineTrace*>(GetIFace(Trace));
}

IServerGameEnts* CIFaceManager::GetIents() const
{
	return reinterpret_cast<IServerGameEnts*>(GetIFace(GameEnts));
}

IServerGameClients* CIFaceManager::GetIclients() const
{
	return reinterpret_cast<IServerGameClients*>(GetIFace(GameClients));
}

CGlobalVars* CIFaceManager::GetGlobals() const
{
	return reinterpret_cast<CGlobalVars*>(GetIFace(Globals));
}

#endif

