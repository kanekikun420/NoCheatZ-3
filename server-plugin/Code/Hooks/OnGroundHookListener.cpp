#include "OnGroundHookListener.h"

std::list<OnGroundHookListener*> OnGroundHookListener::m_listeners;
GroundEntity_t OnGroundHookListener::gpOldGroundFn = nullptr;
DWORD* OnGroundHookListener::pdwInterface = nullptr;

OnGroundHookListener::OnGroundHookListener()
{
}

OnGroundHookListener::~OnGroundHookListener()
{
}

void OnGroundHookListener::HookOnGround(NczPlayer* player)
{
	IServerUnknown * p1 = player->GetEdict()->GetUnknown();
	CBaseEntity * p2 = p1->GetBaseEntity();
	CBasePlayer *BasePlayer = reinterpret_cast<CBasePlayer *>(p2);
	Assert(BasePlayer);

	if(pdwInterface != ( DWORD* )*( DWORD* )BasePlayer)
	{
		pdwInterface = ( DWORD* )*( DWORD* )BasePlayer;

		DWORD OldFunc = VirtualTableHook(pdwInterface, Config::GetInstance()->GetVTable()->Offset_m_hGroundEntityChanged, ( DWORD )nNetworkStateChanged_m_hGroundEntity );
		*(DWORD*)&(gpOldGroundFn) = OldFunc;
	}
}

void OnGroundHookListener::UnhookOnGround()
{
	if(pdwInterface && gpOldGroundFn)
	{
		VirtualTableHook(pdwInterface, Config::GetInstance()->GetVTable()->Offset_m_hGroundEntityChanged, (DWORD)gpOldGroundFn);
		pdwInterface = nullptr;
		gpOldGroundFn = nullptr;
	}
}

#ifdef GNUC
void HOOKFN_INT OnGroundHookListener::nNetworkStateChanged_m_hGroundEntity(CBasePlayer* basePlayer, int * new_m_hGroundEntity)
#else
void HOOKFN_INT OnGroundHookListener::nNetworkStateChanged_m_hGroundEntity(CBasePlayer* basePlayer, void*, int * new_m_hGroundEntity)
#endif
{
#ifdef WIN32
	__asm push esi;
	__asm push ecx;
#else
	//__asm("pusha");
#endif
	CNoCheatZPlugin::GetInstance()->game_frame.EndExec();
	CNoCheatZPlugin::GetInstance()->ncz_frame.StartExec();
	PlayerHandler* ph = NczPlayerManager::GetInstance()->GetPlayerHandlerByBasePlayer(basePlayer);
	bool new_isOnground = true;

	if(ph->status < PLAYER_CONNECTED) goto callgroundfn;

	if(*new_m_hGroundEntity == -1) new_isOnground = true;
	else new_isOnground = false;

	for(std::list<OnGroundHookListener*>::iterator it = m_listeners.begin(); it != m_listeners.end(); ++it)
		(*it)->m_hGroundEntityStateChangedCallback(ph->playerClass, new_isOnground);

callgroundfn:
	CNoCheatZPlugin::GetInstance()->ncz_frame.EndExec();
	CNoCheatZPlugin::GetInstance()->game_frame.StartExec();
#ifdef WIN32
	__asm pop esi;
	__asm pop ecx;

	__asm mov esi, dword ptr [new_m_hGroundEntity];
	__asm push esi;
	__asm mov ecx, dword ptr [basePlayer];
	__asm call gpOldGroundFn;
#else
	//__asm("popa");

	gpOldGroundFn(basePlayer, new_m_hGroundEntity);
#endif
}

void OnGroundHookListener::RegisterOnGroundHookListener(OnGroundHookListener* listener)
{
	m_listeners.push_back(listener);
}

void OnGroundHookListener::RemoveOnGroundHookListener(OnGroundHookListener* listener)
{
	m_listeners.remove(listener);
}
