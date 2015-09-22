#include "TeleportHookListener.h"

std::list<TeleportHookListener*> TeleportHookListener::m_listeners;
Teleport_t TeleportHookListener::gpOldTeleportFn = nullptr;
DWORD* TeleportHookListener::pdwInterface = nullptr;

//https://www.sourcemodplugins.org/vtableoffsets
// CBaseFlex::Teleport(Vector const*, QAngle const*, Vector const*)

#ifdef NCZ_CSS
#	ifdef GNUC
#		define DEFAULT_TELEPORT_OFFSET "109"
#	else
#		define DEFAULT_TELEPORT_OFFSET "108"
#	endif
#else
#	ifdef NCZ_CSGO
#		ifdef GNUC
#			define DEFAULT_TELEPORT_OFFSET "114"
#		else
#			define DEFAULT_TELEPORT_OFFSET "113"
#		endif
#	else
#		ifdef NCZ_CSP

#		endif
#	endif
#endif

ConVar var_teleport_offset = ConVar( "ncz_teleport_offset",	DEFAULT_TELEPORT_OFFSET);

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

	if(pdwInterface != ( DWORD* )*( DWORD* )BasePlayer)
	{
		pdwInterface = ( DWORD* )*( DWORD* )BasePlayer;

#		ifdef DEBUG
		Msg("TeleportHookListener::HookTeleport(NczPlayer* player)\n- var_teleport_offset.GetInt() = %d\n- BasePlayer = %X\n- pdwInterface = %X\n", var_teleport_offset.GetInt(), BasePlayer, pdwInterface);
#		endif

		DWORD OldFunc = VirtualTableHook(pdwInterface, var_teleport_offset.GetInt(), ( DWORD )nTeleport);
		*(DWORD*)&(gpOldTeleportFn) = OldFunc;
	}
}

void TeleportHookListener::UnhookTeleport()
{
	if(pdwInterface && gpOldTeleportFn)
	{
		VirtualTableHook(pdwInterface, var_teleport_offset.GetInt(), (DWORD)gpOldTeleportFn);
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
	PlayerHandler* ph = NczPlayerManager::GetInstance()->GetPlayerHandlerByBasePlayer(basePlayer);

	if(ph->status != BOT)
	{
		for(std::list<TeleportHookListener*>::iterator it = m_listeners.begin(); it != m_listeners.end(); ++it)
			(*it)->TeleportCallback(ph->playerClass, va, qa, vb);
	}

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
