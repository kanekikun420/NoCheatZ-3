#include "PlayerRunCommandHookListener.h"
#include "plugin.h"

/////////////////////////////////////////////////////////////////////////
// PlayerRunCommandHookListener
/////////////////////////////////////////////////////////////////////////

PlayerRunCommand_t PlayerRunCommandHookListener::gpOldPlayerRunCommand = nullptr;
DWORD* PlayerRunCommandHookListener::pdwInterface = nullptr;
std::list<PlayerRunCommandHookListener*> PlayerRunCommandHookListener::m_listeners;
CUserCmd PlayerRunCommandHookListener::m_lastCUserCmd[MAX_PLAYERS];

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
	Assert(BasePlayer);

	if(pdwInterface != ( DWORD* )*( DWORD* )BasePlayer)
	{
		pdwInterface = ( DWORD* )*( DWORD* )BasePlayer;

		DWORD OldFunc = VirtualTableHook(pdwInterface, Config::GetInstance()->GetVTable()->Offset_playerRunCommand, (DWORD)nPlayerRunCommand );
		*(DWORD*)&(gpOldPlayerRunCommand) = OldFunc;
	}
}

void PlayerRunCommandHookListener::UnhookPlayerRunCommand()
{
	if(pdwInterface && gpOldPlayerRunCommand)
	{
		VirtualTableHook(pdwInterface, Config::GetInstance()->GetVTable()->Offset_playerRunCommand, (DWORD)gpOldPlayerRunCommand);
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
#ifdef WIN32
	__asm push eax;
	__asm push esi;
	__asm push ecx;
#else
	//__asm("pusha");
#endif
	CNoCheatZPlugin::GetInstance()->game_frame.EndExec();
	CNoCheatZPlugin::GetInstance()->ncz_frame.StartExec();
	PlayerHandler* ph = NczPlayerManager::GetInstance()->GetPlayerHandlerByBasePlayer(This);
	if(ph->status != PLAYER_IN_TESTS) goto callfn;

	for(std::list<PlayerRunCommandHookListener*>::iterator it = m_listeners.begin(); it != m_listeners.end(); ++it)
	{
		if((*it)->PlayerRunCommandCallback(ph->playerClass, pCmd))
		{
			m_lastCUserCmd[ph->playerClass->GetIndex()] = *pCmd;
			pCmd->MakeInert();
			goto callfn;
		}
	}

	m_lastCUserCmd[ph->playerClass->GetIndex()] = *pCmd;

callfn:
	CNoCheatZPlugin::GetInstance()->ncz_frame.EndExec();
	CNoCheatZPlugin::GetInstance()->game_frame.StartExec();
#ifdef WIN32
	__asm pop eax;
	__asm pop esi;
	__asm pop ecx;

	__asm mov eax, dword ptr [pMoveHelper];
	__asm push eax;
	__asm mov esi, dword ptr [pCmd];
	__asm push esi;
	__asm mov ecx, dword ptr [This];
	__asm call gpOldPlayerRunCommand;
#else
	//__asm("popa");

	gpOldPlayerRunCommand(This, pCmd, pMoveHelper);
	/*union {
		void (EmptyClass::*mfpnew)(CUserCmd*, IMoveHelper*);
		void* fn_addr;
	} u;
	u.fn_addr = (void*)gpOldPlayerRunCommand;
	
	(reinterpret_cast<EmptyClass*>(This)->*(u.mfpnew))(pCmd, pMoveHelper);*/

	return;
#endif
}

void PlayerRunCommandHookListener::RegisterPlayerRunCommandHookListener(PlayerRunCommandHookListener* listener)
{
	m_listeners.push_back(listener);
}

void PlayerRunCommandHookListener::RemovePlayerRunCommandHookListener(PlayerRunCommandHookListener* listener)
{
	m_listeners.remove(listener);
}
