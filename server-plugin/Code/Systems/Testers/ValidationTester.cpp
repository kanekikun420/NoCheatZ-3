#include "ValidationTester.h"

/////////////////////////////////////////////////////////////////////////
// ValidationTester
/////////////////////////////////////////////////////////////////////////

ValidationTester::ValidationTester() : 
	BaseSystem(),
	NczFilteredPlayersList(),
	BaseTimedTester(),
	PlayerDataStructHandler<bool>(),
	Singleton<ValidationTester>()
{
}

ValidationTester::~ValidationTester()
{
}

const char * ValidationTester::GetName()
{
	return "ValidationTester";
}

SlotStatus ValidationTester::GetFilter()
{
	return PLAYER_CONNECTED;
}

void ValidationTester::SetValidated(NczPlayer* player)
{
	*GetPlayerDataStruct(player) = false;
	if(!HasVerbose()) return;
	Msg("%f : %d validated\n", Plat_FloatTime(), player->GetIndex());
	if(GetJobCount() == 0) SetActive(false);
}

void ValidationTester::ProcessPlayerTest(NczPlayer* player)
{
	m_metrics.StartExec();
	if(!*GetPlayerDataStruct(player))
	{
		if(HasVerbose())
		{
			Msg("%f : %d validated\n", Plat_FloatTime(), player->GetIndex());
		}
		return;
	}
	if(HasVerbose())
	{
		Msg("%f : %d not validated\n", Plat_FloatTime(), player->GetIndex());
		Msg("%f : %d validation time left : %f\n", Plat_FloatTime(), player->GetIndex(), 20.0 - player->GetTimeConnected());
	}

	if(player->GetTimeConnected() < 20.0) return;
	player->Kick();
	m_metrics.EndExec();
}

void ValidationTester::Load()
{
	BaseFramedTester::RegisterFramedTester(this);
	*GetDefaultDataStruct() = false;
}

void ValidationTester::Unload()
{
	BaseFramedTester::RemoveFramedTester(this);

	PLAYERS_LOOP_RUNTIME
	{
		ResetPlayerDataStruct(ph->playerClass);
	}
	END_PLAYERS_LOOP
}

short ValidationTester::GetJobCount()
{
	NczPlayer* player = nullptr;
	short count = 0;
	ResetNextPlayer();
	while((player = GetNextPlayer()) != nullptr)
	{
		if(*GetPlayerDataStruct(player)) ++count;
	}
	return count;
}
