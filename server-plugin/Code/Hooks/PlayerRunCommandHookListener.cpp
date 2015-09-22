#include "PlayerRunCommandHookListener.h"
#include "plugin.h"

/////////////////////////////////////////////////////////////////////////
// PlayerRunCommandHookListener
/////////////////////////////////////////////////////////////////////////

PlayerRunCommand_t PlayerRunCommandHookListener::gpOldPlayerRunCommand = nullptr;
DWORD* PlayerRunCommandHookListener::pdwInterface = nullptr;
std::list<PlayerRunCommandHookListener*> PlayerRunCommandHookListener::m_listeners;
CUserCmd PlayerRunCommandHookListener::m_lastCUserCmd[MAX_PLAYERS];

//https://www.sourcemodplugins.org/vtableoffsets
// CCSPlayer::PlayerRunCommand(CUserCmd*, IMoveHelper*)

#ifdef NCZ_CSS
#	ifdef GNUC
#		define DEFAULT_RUNCOMMAND_OFFSET "420"
#	else
#		define DEFAULT_RUNCOMMAND_OFFSET "419"
#	endif
#else
#	ifdef NCZ_CSGO
#		ifdef GNUC
#			define DEFAULT_RUNCOMMAND_OFFSET "467"
#		else
#			define DEFAULT_RUNCOMMAND_OFFSET "466"
#		endif
#	else
#		ifdef NCZ_CSP

#		endif
#	endif
#endif

ConVar var_runcommand_offset = ConVar( "ncz_runcommand_offset",	DEFAULT_RUNCOMMAND_OFFSET);

PlayerRunCommandHookListener::PlayerRunCommandHookListener()
{
	memset(m_lastCUserCmd, 0, sizeof(CUserCmd)*MAX_PLAYERS);
}

PlayerRunCommandHookListener::~PlayerRunCommandHookListener()
{
}

CUserCmd* PlayerRunCommandHookListener::GetLastUserCmd(NczPlayer* player)
{
	return &(m_lastCUserCmd[player->GetIndex()]);
}

void PlayerRunCommandHookListener::HookPlayerRunCommand(NczPlayer* player)
{
	IServerUnknown * p1 = player->GetEdict()->GetUnknown();
	CBaseEntity * p2 = p1->GetBaseEntity();
	CCSPlayer *BasePlayer = reinterpret_cast<CCSPlayer *>(p2);

	if(pdwInterface != ( DWORD* )*( DWORD* )BasePlayer)
	{
		pdwInterface = ( DWORD* )*( DWORD* )BasePlayer;

#		ifdef DEBUG
		Msg("PlayerRunCommandHookListener::HookPlayerRunCommand(NczPlayer* player)\n- var_runcommand_offset.GetInt() = %d\n- BasePlayer = %X\n- pdwInterface = %X\n", var_runcommand_offset.GetInt(), BasePlayer, pdwInterface);
#		endif

		DWORD OldFunc = VirtualTableHook(pdwInterface, var_runcommand_offset.GetInt(), (DWORD)nPlayerRunCommand );
		*(DWORD*)&(gpOldPlayerRunCommand) = OldFunc;
	}
}

void PlayerRunCommandHookListener::UnhookPlayerRunCommand()
{
	if(pdwInterface && gpOldPlayerRunCommand)
	{
		VirtualTableHook(pdwInterface, var_runcommand_offset.GetInt(), (DWORD)gpOldPlayerRunCommand);
		pdwInterface = nullptr;
		gpOldPlayerRunCommand = nullptr;
	}
}

#ifdef GNUC
void PlayerRunCommandHookListener::nPlayerRunCommand(void* This, CUserCmd* pCmd, IMoveHelper* pMoveHelper)
#else
void HOOKFN_INT PlayerRunCommandHookListener::nPlayerRunCommand(void* This, void*, CUserCmd* pCmd, IMoveHelper* pMoveHelper)
#endif
{
	PlayerHandler* ph = NczPlayerManager::GetInstance()->GetPlayerHandlerByBasePlayer(This);

	if(ph->status == PLAYER_IN_TESTS)
	{
		for(std::list<PlayerRunCommandHookListener*>::iterator it = m_listeners.begin(); it != m_listeners.end(); ++it)
		{
			if((*it)->PlayerRunCommandCallback(ph->playerClass, pCmd))
			{
				pCmd->MakeInert();
				break;
			}
		}
	}

	m_lastCUserCmd[ph->playerClass->GetIndex()] = *pCmd;

	gpOldPlayerRunCommand(This, pCmd, pMoveHelper);
}

void PlayerRunCommandHookListener::RegisterPlayerRunCommandHookListener(PlayerRunCommandHookListener* listener)
{
	m_listeners.push_back(listener);
}

void PlayerRunCommandHookListener::RemovePlayerRunCommandHookListener(PlayerRunCommandHookListener* listener)
{
	m_listeners.remove(listener);
}
