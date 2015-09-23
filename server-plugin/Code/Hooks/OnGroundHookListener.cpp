#include "OnGroundHookListener.h"

std::list<OnGroundHookListener*> OnGroundHookListener::m_listeners;
GroundEntity_t OnGroundHookListener::gpOldGroundFn = nullptr;
DWORD* OnGroundHookListener::pdwInterface = nullptr;

//https://www.sourcemodplugins.org/vtableoffsets
// CBasePlayer::NetworkStateChanged_m_hGroundEntity(void*)

#ifdef NCZ_CSS
#	ifdef GNUC
#		define DEFAULT_GROUND_OFFSET "179"
#	else
#		define DEFAULT_GROUND_OFFSET "177"
#	endif
#else
#	ifdef NCZ_CSGO
#		ifdef GNUC
#			define DEFAULT_GROUND_OFFSET "177"
#		else
#			define DEFAULT_GROUND_OFFSET "175"
#		endif
#	else
#		ifdef NCZ_CSP

#		endif
#	endif
#endif

static ConVar var_onground_offset("ncz_onground_offset", DEFAULT_GROUND_OFFSET);

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

	if(pdwInterface != ( DWORD* )*( DWORD* )BasePlayer)
	{
		pdwInterface = ( DWORD* )*( DWORD* )BasePlayer;

#		ifdef DEBUG
		Msg("OnGroundHookListener::HookOnGround(NczPlayer* player)\n- var_onground_offset.GetInt() = %d\n- BasePlayer = %X\n- pdwInterface = %X\n", var_onground_offset.GetInt(), BasePlayer, pdwInterface);
#		endif

		DWORD OldFunc = VirtualTableHook(pdwInterface, var_onground_offset.GetInt(), ( DWORD )nNetworkStateChanged_m_hGroundEntity );
		*(DWORD*)&(gpOldGroundFn) = OldFunc;
	}
}

void OnGroundHookListener::UnhookOnGround()
{
	if(pdwInterface && gpOldGroundFn)
	{
		VirtualTableHook(pdwInterface, var_onground_offset.GetInt(), (DWORD)gpOldGroundFn);
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
	PlayerHandler* ph = NczPlayerManager::GetInstance()->GetPlayerHandlerByBasePlayer(basePlayer);
	bool new_isOnground = true;

	if(ph->status >= PLAYER_CONNECTED)
	{
		if(*new_m_hGroundEntity == -1) new_isOnground = true;
		else new_isOnground = false;

		for(std::list<OnGroundHookListener*>::iterator it = m_listeners.begin(); it != m_listeners.end(); ++it)
			(*it)->m_hGroundEntityStateChangedCallback(ph->playerClass, new_isOnground);
	}

	gpOldGroundFn(basePlayer, new_m_hGroundEntity);
}

void OnGroundHookListener::RegisterOnGroundHookListener(OnGroundHookListener* listener)
{
	m_listeners.push_back(listener);
}

void OnGroundHookListener::RemoveOnGroundHookListener(OnGroundHookListener* listener)
{
	m_listeners.remove(listener);
}
