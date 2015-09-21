#include "SpeedTester.h"

SpeedTester::SpeedTester() :
	BaseSystem(),
	BaseTimedTester(0.1, GAME),
	PlayerRunCommandHookListener(),
	PlayerDataStructHandler<SpeedHolderT>(),
	Singleton<SpeedTester>()
{
	m_name = "SpeedTester";
}

SpeedTester::~SpeedTester()
{
	Unload();
}

SlotStatus SpeedTester::GetFilter()
{
	return PLAYER_IN_TESTS;
}

void SpeedTester::Load()
{
	BaseTimedTester::RegisterFramedTester(this);
	PlayerRunCommandHookListener::RegisterPlayerRunCommandHookListener(this);
}

void SpeedTester::Unload()
{
	BaseTimedTester::RemoveFramedTester(this);
	PlayerRunCommandHookListener::RemovePlayerRunCommandHookListener(this);

	PLAYERS_LOOP_RUNTIME
	{
		ResetPlayerDataStruct(ph->playerClass);
	}
	END_PLAYERS_LOOP
}

void SpeedTester::ProcessPlayerTest(NczPlayer* player)
{
	const double game_time = CIFaceManager::GetInstance()->GetIengine()->Time();
	SpeedHolderT* const pInfo = this->GetPlayerDataStruct(player);
	const size_t newTicks = std::ceil((game_time - pInfo->lastTest) / CIFaceManager::GetInstance()->GetIgamedll()->GetTickInterval());
	const double latency = player->GetChannelInfo()->GetLatency(FLOW_OUTGOING);

	if (!pInfo->ticksLeft && std::fabs(pInfo->previousLatency - latency) <= 0.005)
	{
		++(pInfo->detections);

#		ifdef DEBUG
			Msg("Player %s : Speedhack pre-detection #%d\n", player->GetName(), pInfo->detections);
#		endif

		if (pInfo->detections >= 30 && game_time > pInfo->lastDetectionTime + 30.0)
		{

#		ifdef DEBUG
			Msg("Player %s : Speedhack detected\n", player->GetName());
#		endif

			Detection_SpeedHack pDetection = Detection_SpeedHack();
			pDetection.PrepareDetectionData(pInfo);
			pDetection.PrepareDetectionLog(player, this);
			pDetection.Log();

			pInfo->lastDetectionTime = game_time;
		}
	}
	else if(pInfo->detections)
	{
		--(pInfo->detections);
	}

	if ((pInfo->ticksLeft += newTicks) > std::ceil((1.0 / CIFaceManager::GetInstance()->GetIgamedll()->GetTickInterval()) * 2.0))
	{
		pInfo->ticksLeft = std::ceil((1.0 / CIFaceManager::GetInstance()->GetIgamedll()->GetTickInterval()) * 2.0);
	}
			
	pInfo->previousLatency = latency;
	pInfo->lastTest = game_time;
}

bool SpeedTester::PlayerRunCommandCallback(NczPlayer* player, CUserCmd* pCmd)
{
	size_t& tl = this->GetPlayerDataStruct(player)->ticksLeft;

	if(!tl) return true;

	--tl;
	
	return false;
}

const char * Detection_SpeedHack::GetDataDump()
{
	return Helpers::format("SpeedHolderT {%lu, %lu, %f, %f, %f}",
						GetDataStruct()->ticksLeft, GetDataStruct()->detections, GetDataStruct()->lastDetectionTime, GetDataStruct()->previousLatency, GetDataStruct()->lastTest).c_str();					
}
