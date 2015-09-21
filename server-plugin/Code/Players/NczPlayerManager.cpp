#include "NczPlayerManager.h"
#include "Misc/IFaceManager.h"
#include "Misc/Helpers.h"

//---------------------------------------------------------------------------------
// NczPlayerManager
//---------------------------------------------------------------------------------

NczPlayerManager::NczPlayerManager()
{
	_PLAYERS_LOOP_INIT
	{
		ph->playerClass = nullptr;
		ph->status = INVALID;
	}
	END_PLAYERS_LOOP
}

NczPlayerManager::~NczPlayerManager()
{
	CIFaceManager::GetInstance()->GetIevents()->RemoveListener(this);
	PLAYERS_LOOP_RUNTIME
	{
		if(ph->playerClass != nullptr)
		{
			delete ph->playerClass;
			ph->status = INVALID;
		}
	}
	END_PLAYERS_LOOP
}

void NczPlayerManager::LoadPlayerManager()
{
	CIFaceManager::GetInstance()->GetIevents()->AddListener(this, "player_death", true);
	CIFaceManager::GetInstance()->GetIevents()->AddListener(this, "round_end", true);
	CIFaceManager::GetInstance()->GetIevents()->AddListener(this, "round_freeze_end", true);

	//Helpers::FastScan_EntList();
	Helpers::m_EdictList = CIFaceManager::GetInstance()->GetIengine()->PEntityOfEntIndex(0);

	if(Helpers::m_EdictList)
	{
		PLAYERS_LOOP_RUNTIME
		{
			edict_t* pEntity = CIFaceManager::GetInstance()->GetIengine()->PEntityOfEntIndex(x);
			Assert(CIFaceManager::GetInstance()->GetIplayers());
			if(CIFaceManager::GetInstance()->GetIplayers()->GetPlayerInfo(pEntity))
			{
				if(CIFaceManager::GetInstance()->GetIplayers()->GetPlayerInfo(pEntity)->IsFakeClient())
				{
					ph->status = BOT;
					ph->playerClass = new NczPlayer(x);
				}
				else if (CIFaceManager::GetInstance()->GetIplayers()->GetPlayerInfo(pEntity)->IsConnected())
				{
					ph->status = PLAYER_CONNECTED;
					ph->playerClass = new NczPlayer(x);
					ph->playerClass->OnConnect();
				}
			}
		}
		END_PLAYERS_LOOP
	}
}

void NczPlayerManager::ClientConnect(edict_t* pEntity)
{
	short index = CIFaceManager::GetInstance()->GetIengine()->IndexOfEdict(pEntity);
	if(index > 0)
	{
		if(FullHandlersList[index].status == INVALID)
		{
			FullHandlersList[index].playerClass = new NczPlayer(index);
		}
		FullHandlersList[index].status = PLAYER_CONNECTED;
		FullHandlersList[index].playerClass->OnConnect();
		return;
	}
}

void NczPlayerManager::ClientActive(edict_t* pEntity)
{
	short index = CIFaceManager::GetInstance()->GetIengine()->IndexOfEdict(pEntity);
	if(index > 0)
	{
		if(FullHandlersList[index].status == INVALID)
		{
			FullHandlersList[index].playerClass = new NczPlayer(index);
			FullHandlersList[index].status = BOT;
		}
		return;
	}
}

void NczPlayerManager::ClientDisconnect(edict_t* pEntity)
{
	short index = CIFaceManager::GetInstance()->GetIengine()->IndexOfEdict(pEntity);
	if(index > 0)
	{
		if(FullHandlersList[index].status > INVALID)
		{
			if(FullHandlersList[index].playerClass) delete FullHandlersList[index].playerClass;
			FullHandlersList[index].status = INVALID;
			FullHandlersList[index].playerClass = nullptr;
		}
		return;
	}
}

void NczPlayerManager::FireGameEvent(IGameEvent* ev)
{
	Assert(ev);
	Msg("%f - %s\n", Plat_FloatTime(), ev->GetName());

	if(ev->GetInt("userid") > 0)
	{
		PlayerHandler* ph = GetPlayerHandlerByUserId(ev->GetInt("userid"));
		switch(ev->GetName()[7])
		{
			case 'd': // player_death
			{
				if(ph->status <= PLAYER_CONNECTED) return;
				ph->status = PLAYER_CONNECTED;
				return;
			}
		}
	}
	else
	{
		switch(ev->GetName()[6])
		{
			case 'e': // round_end
			{
				PLAYERS_LOOP_RUNTIME
				{
					if(ph->status == PLAYER_IN_TESTS)
						ph->status = PLAYER_CONNECTED;
				}
				END_PLAYERS_LOOP
				ILogger.Flush();
				return;
			}
			case 'f': // round_freeze_end = round_start
			{
				PLAYERS_LOOP_RUNTIME
				{
					if(ph->status == PLAYER_CONNECTED)
					{
						Assert(ph->playerClass);
						if(ph->playerClass->GetPlayerInfo())
						{
							if(ph->playerClass->GetPlayerInfo()->GetTeamIndex() > 1)
								ph->status = PLAYER_IN_TESTS;
						}
					}
				}
				END_PLAYERS_LOOP
				return;
			}
		}
	}
	Assert(false);
}

PlayerHandler* NczPlayerManager::GetPlayerHandlerByIndex(int slot, SlotStatus filter)
{
	if(filter == INVALID)
	{
		if(FullHandlersList[slot].status != INVALID)
		{
			return (PlayerHandler*)(&FullHandlersList[slot]);
		}
		else
		{
			return (PlayerHandler*)(&FullHandlersList[0]); // Always INVALID
		}
	}
	else
	{
		if(FullHandlersList[slot].status == filter)
		{
			return (PlayerHandler*)(&FullHandlersList[slot]);
		}
		else
		{
			return (PlayerHandler*)(&FullHandlersList[0]); // Always INVALID
		}
	}
}

PlayerHandler* NczPlayerManager::GetPlayerHandlerByUserId(int userid, SlotStatus filter)
{
	Assert(userid > 0);
	return GetPlayerHandlerByIndex(Helpers::getIndexFromUserID(userid), filter);
}

PlayerHandler* NczPlayerManager::GetPlayerHandlerByBasePlayer(void* BasePlayer, SlotStatus filter)
{
	Assert(BasePlayer);
	void* tBasePlayer = nullptr;
	if(filter == INVALID)
	{
		PLAYERS_LOOP_RUNTIME
		{
			tBasePlayer = reinterpret_cast<void*>(ph->playerClass->GetEdict()->GetUnknown()->GetBaseEntity());
			if(tBasePlayer == BasePlayer) return ph;
		}
		END_PLAYERS_LOOP
	}
	else
	{
		PLAYERS_LOOP_RUNTIME
		{
			if(ph->status <= filter) continue;
		
			tBasePlayer = reinterpret_cast<void*>(ph->playerClass->GetEdict()->GetUnknown()->GetBaseEntity());
			if(tBasePlayer == BasePlayer) return ph;
		}
		END_PLAYERS_LOOP
	}
	
	return (&FullHandlersList[0]);
}

PlayerHandler* NczPlayerManager::GetPlayerHandlerBySteamID(const char * steamid, SlotStatus filter)
{
	const char *tSteamid;
	if(filter == INVALID)
	{
		PLAYERS_LOOP_RUNTIME
		{
			tSteamid = ph->playerClass->GetSteamID();
			if(Helpers::bStrEq(tSteamid, steamid)) return ph;
		}
		END_PLAYERS_LOOP
	}
	else
	{
		PLAYERS_LOOP_RUNTIME
		{
			if(ph->status != filter) continue;

			tSteamid = ph->playerClass->GetSteamID();
			if(Helpers::bStrEq(tSteamid, steamid)) return ph;
		}
		END_PLAYERS_LOOP
	}
		
	return (&FullHandlersList[0]);
}

PlayerHandler* NczPlayerManager::GetPlayerHandlerByEdict(edict_t * pEdict, SlotStatus filter)
{
	edict_t * tEdict;
	if(filter == INVALID)
	{
		PLAYERS_LOOP_RUNTIME
		{
			tEdict = ph->playerClass->GetEdict();
			if(tEdict == pEdict) return ph;
		}
		END_PLAYERS_LOOP
	}
	else
	{
		PLAYERS_LOOP_RUNTIME
		{
			if(ph->status != filter) continue;

			tEdict = ph->playerClass->GetEdict();
			if(tEdict == pEdict) return ph;
		}
		END_PLAYERS_LOOP
	}
		
	return (&FullHandlersList[0]);
}

PlayerHandler* NczPlayerManager::GetPlayerHandlerByName(const char * playerName, SlotStatus filter)
{
	const char * tName;
	if(filter == INVALID)
	{
		PLAYERS_LOOP_RUNTIME
		{
			tName = ph->playerClass->GetName();
			if(Helpers::bStrEq(tName, playerName)) return ph;
		}
		END_PLAYERS_LOOP
	}
	else
	{
		PLAYERS_LOOP_RUNTIME
		{
			if(ph->status != filter) continue;

			tName = ph->playerClass->GetName();
			if(Helpers::bStrEq(tName, playerName)) return ph;
		}
		END_PLAYERS_LOOP
	}
	return (&FullHandlersList[0]);
}

short NczPlayerManager::GetPlayerCount(SlotStatus filter)
{
	short count = 0;
	if(filter == INVALID)
	{
		PLAYERS_LOOP_RUNTIME
			if(ph->status != filter) ++count;
		END_PLAYERS_LOOP
	}
	else
	{
		PLAYERS_LOOP_RUNTIME
			if(ph->status == filter) ++count;
		END_PLAYERS_LOOP
	}
	return count;
}
