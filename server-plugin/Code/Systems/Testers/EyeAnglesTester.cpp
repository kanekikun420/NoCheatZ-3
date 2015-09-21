#include "EyeAnglesTester.h"

EyeAnglesTester::EyeAnglesTester(void) :
	BaseSystem(),
	PlayerRunCommandHookListener(),
	PlayerDataStructHandler<EyeAngleInfoT>(),
	IGameEventListener2(),
	Singleton<EyeAnglesTester>()
{
	m_name = "EyeAnglesTester";
}

EyeAnglesTester::~EyeAnglesTester(void)
{
	Unload();
}

SlotStatus EyeAnglesTester::GetFilter()
{
	return PLAYER_IN_TESTS;
}

void EyeAnglesTester::Load()
{
	CIFaceManager::GetInstance()->GetIevents()->AddListener(this, "round_end", true);
	PlayerRunCommandHookListener::RegisterPlayerRunCommandHookListener(this);
}

void EyeAnglesTester::Unload()
{
	PlayerRunCommandHookListener::RemovePlayerRunCommandHookListener(this);
	CIFaceManager::GetInstance()->GetIevents()->RemoveListener(this);

	PLAYERS_LOOP_RUNTIME
	{
		ResetPlayerDataStruct(ph->playerClass);
	}
	END_PLAYERS_LOOP
}

bool EyeAnglesTester::PlayerRunCommandCallback(NczPlayer* player, CUserCmd* pCmd)
{	
	bool drop_cmd = false;

	EyeAngleInfoT* playerData = GetPlayerDataStruct(player);
	playerData->x.abs_value = fabs(    playerData->x.value = pCmd->viewangles.x    );
	playerData->y.abs_value = fabs(    playerData->y.value = pCmd->viewangles.y    );
	playerData->z.abs_value = fabs(    playerData->z.value = pCmd->viewangles.z    );

	if (playerData->x.abs_value > 89.0f || playerData->z.abs_value > 0.0f || playerData->y.abs_value > 180.0f)
	{
#		ifdef DEBUG
			Msg("Player %s : Bad Eye Angles %f, %f, %f\n", player->GetName(), playerData->x.value, playerData->y.value, playerData->z.value);
#		endif
		if(playerData->ignore_last) --(playerData->ignore_last);
		else drop_cmd = true;
	}

	if(drop_cmd)
	{
#		ifdef DEBUG
			Msg("Player %s : Droping command #%d\n", player->GetName(), pCmd->command_number);
#		endif
		if(playerData->x.abs_value > 89.0f)
		{
			++playerData->x.detectionsCount;
			if(playerData->x.lastDetectionPrintTime + 10.0 < Plat_FloatTime())
			{
				playerData->x.lastDetectionPrintTime = Plat_FloatTime();

				Detection_EyeAngleX pDetection = Detection_EyeAngleX();
				pDetection.PrepareDetectionData(playerData);
				pDetection.PrepareDetectionLog(player, this);
				pDetection.Log();
			}
		}
		if(playerData->y.abs_value > 180.0f)
		{
			++playerData->y.detectionsCount;
			if(playerData->y.lastDetectionPrintTime + 10.0 < Plat_FloatTime())
			{
				playerData->y.lastDetectionPrintTime = Plat_FloatTime();

				Detection_EyeAngleY pDetection = Detection_EyeAngleY();
				pDetection.PrepareDetectionData(playerData);
				pDetection.PrepareDetectionLog(player, this);
				pDetection.Log();
			}
		}
		if(playerData->z.abs_value > 0.0f)
		{
			++playerData->z.detectionsCount;
			if(playerData->z.lastDetectionPrintTime + 10.0 < Plat_FloatTime())
			{
				playerData->z.lastDetectionPrintTime = Plat_FloatTime();

				Detection_EyeAngleZ pDetection = Detection_EyeAngleZ();
				pDetection.PrepareDetectionData(playerData);
				pDetection.PrepareDetectionLog(player, this);
				pDetection.Log();
			}
		}
	}
	return drop_cmd;
}

void EyeAnglesTester::FireGameEvent(IGameEvent *ev) // round_end
{
	for(int index = 1; index < MAX_PLAYERS; ++index)
	{
		PlayerHandler* ph = NczPlayerManager::GetInstance()->GetPlayerHandlerByIndex(index);
		if(ph->status > BOT) ++(GetPlayerDataStruct(ph->playerClass)->ignore_last);
	}
}

void EyeAnglesTester::TeleportCallback(NczPlayer* player, Vector const* va, QAngle const* qa, Vector const* vb)
{
#	ifdef DEBUG
		Msg("Player %s : EyeAnglesTester::TeleportCallback\n", player->GetName());
#	endif
	EyeAngleInfoT* playerData = GetPlayerDataStruct(player);
	++playerData->ignore_last;
}

const char * Detection_EyeAngle::GetDataDump()
{
	return Helpers::format("EyeAngleInfo { %d, EyeAngleX { %f, %f, %f, %d }, EyeAngleY { %f, %f, %f, %d }, EyeAngleZ { %f, %f, %f, %d } }\0",
							GetDataStruct()->ignore_last, 
							GetDataStruct()->x.value, GetDataStruct()->x.abs_value, GetDataStruct()->x.lastDetectionPrintTime, GetDataStruct()->x.detectionsCount,
							GetDataStruct()->y.value, GetDataStruct()->y.abs_value, GetDataStruct()->y.lastDetectionPrintTime, GetDataStruct()->y.detectionsCount,
							GetDataStruct()->z.value, GetDataStruct()->z.abs_value, GetDataStruct()->z.lastDetectionPrintTime, GetDataStruct()->z.detectionsCount).c_str();
}

const char * Detection_EyeAngleX::GetDetectionLogMessage()
{
	if(Helpers::IsInt(GetDataStruct()->x.value))
	{
		return "Anti-Aim";
	}
	else
	{
		return "No recoil";
	}
}

const char * Detection_EyeAngleY::GetDetectionLogMessage()
{
	if(Helpers::IsInt(GetDataStruct()->y.value))
	{
		return "Anti-Aim";
	}
	else
	{
		return "No spread";
	}
}

const char * Detection_EyeAngleZ::GetDetectionLogMessage()
{
	if(Helpers::IsInt(GetDataStruct()->z.value))
	{
		return "Anti-Aim";
	}
	else
	{
		return "No spread";
	}
}
