#include "TeleportHookListener.h"

std::list<TeleportHookListener*> TeleportHookListener::m_listeners;
Teleport_t TeleportHookListener::gpOldTeleportFn = nullptr;
DWORD* TeleportHookListener::pdwInterface = nullptr;

TeleportHookListener::TeleportHookListener()
{
}

TeleportHookListener::~TeleportHookListener()
{
}

void TeleportHookListener::HookTeleport(NczPlayer* player)
{
	IServerUnknown * p1 = player->GetEdict()->GetUnknown();
	CBaseEntity * p2 = p1->GetBaseEntity();
	CBaseFlex *BasePlayer = reinterpret_cast<CBaseFlex *>(p2);
	Assert(BasePlayer);

	if(pdwInterface != ( DWORD* )*( DWORD* )BasePlayer)
	{
		pdwInterface = ( DWORD* )*( DWORD* )BasePlayer;

		DWORD OldFunc = VirtualTableHook(pdwInterface, Config::GetInstance()->GetVTable()->Offset_teleport, ( DWORD )nTeleport);
		*(DWORD*)&(gpOldTeleportFn) = OldFunc;
	}
}

void TeleportHookListener::UnhookTeleport()
{
	if(pdwInterface && gpOldTeleportFn)
	{
		VirtualTableHook(pdwInterface, Config::GetInstance()->GetVTable()->Offset_teleport, (DWORD)gpOldTeleportFn);
		pdwInterface = nullptr;
		gpOldTeleportFn = nullptr;
	}
}

#ifdef GNUC
void TeleportHookListener::nTeleport(CBaseFlex * basePlayer, Vector const* va, QAngle const* qa, Vector const* vb)
#else
void HOOKFN_INT TeleportHookListener::nTeleport(CBaseFlex * basePlayer, void*, Vector const* va, QAngle const* qa, Vector const* vb)
#endif
{
#ifdef WIN32
	__asm pushad;
#else
	//__asm("pusha");
#endif

	PlayerHandler* ph = NczPlayerManager::GetInstance()->GetPlayerHandlerByBasePlayer(basePlayer);

	if(ph->status == BOT) goto callteleportfn;

	for(std::list<TeleportHookListener*>::iterator it = m_listeners.begin(); it != m_listeners.end(); ++it)
		(*it)->TeleportCallback(ph->playerClass, va, qa, vb);

callteleportfn:
#ifdef WIN32
	__asm popad;
#else
	//__asm("popa");
#endif
	gpOldTeleportFn(basePlayer, va, qa, vb);
}

void TeleportHookListener::RegisterTeleportHookListener(TeleportHookListener* listener)
{
	m_listeners.push_back(listener);
}

void TeleportHookListener::RemoveTeleportHookListener(TeleportHookListener* listener)
{
	m_listeners.remove(listener);
}
