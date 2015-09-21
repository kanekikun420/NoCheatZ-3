#include "JumpTester.h"

/*
	Test each player to see if they use any script to help BunnyHop.

	Some players jumps just-in-time without using any script.
	We have to make the difference by using statistics.
*/

JumpTester::JumpTester() :
	BaseSystem(),
	BaseTimedTester(10.0),
	OnGroundHookListener(),
	PlayerRunCommandHookListener(),
	PlayerDataStructHandler<JumpInfoT>(),
	Singleton<JumpTester>()
{
	m_name = "JumpTester";
}

JumpTester::~JumpTester()
{
	Unload();
}

SlotStatus JumpTester::GetFilter()
{
	return PLAYER_IN_TESTS;
}

void JumpTester::Load()
{
	BaseTimedTester::RegisterFramedTester(this);
	OnGroundHookListener::RegisterOnGroundHookListener(this);
	PlayerRunCommandHookListener::RegisterPlayerRunCommandHookListener(this);
}

void JumpTester::Unload()
{
	BaseTimedTester::RemoveFramedTester(this);
	OnGroundHookListener::RemoveOnGroundHookListener(this);
	PlayerRunCommandHookListener::RemovePlayerRunCommandHookListener(this);

	PLAYERS_LOOP_RUNTIME
	{
		ResetPlayerDataStruct(ph->playerClass);
	}
	END_PLAYERS_LOOP
}

void JumpTester::m_hGroundEntityStateChangedCallback(NczPlayer* player, bool new_isOnGround)
{
	JumpInfoT* playerData = GetPlayerDataStruct(player);

	if(new_isOnGround)
	{
		playerData->onGroundHolder.onGround_Tick = CIFaceManager::GetInstance()->GetGlobals()->tickcount;
		playerData->isOnGround = true;
		if(HasVerbose()) Msg("Player %s : Now on ground at %f %d\n", player->GetName(), Plat_FloatTime(), CIFaceManager::GetInstance()->GetGlobals()->tickcount);
		if(playerData->jumpCmdHolder.outsideJumpCmdCount > 10) // Il serait plus judicieux d'utiliser le RMS
		{
			Detection_BunnyHopScript pDetection = Detection_BunnyHopScript();
			pDetection.PrepareDetectionData(playerData);
			pDetection.PrepareDetectionLog(player, this);
			pDetection.Log();
			player->Kick("You have to turn off your BunnyHop Script to play on this server.");
		}
		else if(playerData->jumpCmdHolder.outsideJumpCmdCount == 0 && playerData->perfectBhopsCount > 5)
		{
			Detection_BunnyHopProgram pDetection = Detection_BunnyHopProgram();
			pDetection.PrepareDetectionData(playerData);
			pDetection.PrepareDetectionLog(player, this);
			pDetection.Log();

			player->Ban("[NoCheatZ 4] You have been banned for using BunnyHop on this server.");
		}
		playerData->jumpCmdHolder.outsideJumpCmdCount = 0;
	}
	else
	{
		playerData->onGroundHolder.notOnGround_Tick = CIFaceManager::GetInstance()->GetGlobals()->tickcount;
		++playerData->onGroundHolder.jumpCount;
		playerData->isOnGround = false;
		if(HasVerbose()) Msg("Player %s : Now not on ground at %f %d\n", player->GetName(), Plat_FloatTime(), CIFaceManager::GetInstance()->GetGlobals()->tickcount);
	}
}

void JumpTester::ProcessPlayerTest(NczPlayer* player)
{

}

bool JumpTester::PlayerRunCommandCallback(NczPlayer* player, CUserCmd* pCmd)
{
	bool drop_cmd = false;

	JumpInfoT* playerData = GetPlayerDataStruct(player);

	if((playerData->jumpCmdHolder.lastJumpCmdState == false) && (pCmd->buttons & IN_JUMP))
	{
		playerData->jumpCmdHolder.JumpDown_Tick = CIFaceManager::GetInstance()->GetGlobals()->tickcount;
		if(playerData->isOnGround)
		{
			int diff = abs(playerData->jumpCmdHolder.JumpDown_Tick - playerData->onGroundHolder.onGround_Tick);
			if(diff < 10)
			{
				++playerData->total_bhopCount;
#				ifdef DEBUG
					Msg("Player %s : total_bhopCount = %d\n", player->GetName(), playerData->total_bhopCount);
#				endif
				if(diff < 3 && diff > 0)
				{
					++playerData->goodBhopsCount;
#					ifdef DEBUG
						Msg("Player %s : goodBhopsCount = %d\n", player->GetName(), playerData->goodBhopsCount);
#					endif
					drop_cmd = true;
				}
				if(diff == 0)
				{
					++playerData->perfectBhopsCount;
#					ifdef DEBUG
						Msg("Player %s : perfectBhopsCount = %d\n", player->GetName(), playerData->perfectBhopsCount);
#					endif
					drop_cmd = true;
				}
			}

			if(HasVerbose()) Msg("Player %s : %f %d : Now using jump button (delta %d)\n", player->GetName(), Plat_FloatTime(), CIFaceManager::GetInstance()->GetGlobals()->tickcount, diff);
		}
		else
		{
			++playerData->jumpCmdHolder.outsideJumpCmdCount;
			if(HasVerbose()) Msg("Player %s : %f %d : Now using jump button (outside count %d)\n", player->GetName(), Plat_FloatTime(), CIFaceManager::GetInstance()->GetGlobals()->tickcount, playerData->jumpCmdHolder.outsideJumpCmdCount);
		}
		playerData->jumpCmdHolder.lastJumpCmdState = true;
	}
	else if((playerData->jumpCmdHolder.lastJumpCmdState == true) && ((pCmd->buttons & IN_JUMP) == 0))
	{
		playerData->jumpCmdHolder.lastJumpCmdState = false;
		playerData->jumpCmdHolder.JumpUp_Tick = CIFaceManager::GetInstance()->GetGlobals()->tickcount;
		if(HasVerbose()) Msg("Player %s : %f %d : Now not using jump button\n", player->GetName(), Plat_FloatTime(), CIFaceManager::GetInstance()->GetGlobals()->tickcount);
	}
#	ifdef DEBUG
		if(drop_cmd) Msg("Player %s : UserCmd will be dropped\n", player->GetName());
#	endif
	return drop_cmd;
}

const char * Detection_BunnyHopScript::GetDataDump()
{
	return Helpers::format("BunnyHopInfoT { OnGroundHolderT { %d, %d, %d }, JumpCmdHolderT { %d, %d, %d, %d }, %d, %d, %d, %d }\0",
							GetDataStruct()->onGroundHolder.onGround_Tick, GetDataStruct()->onGroundHolder.notOnGround_Tick, GetDataStruct()->onGroundHolder.jumpCount,
							GetDataStruct()->jumpCmdHolder.lastJumpCmdState, GetDataStruct()->jumpCmdHolder.JumpDown_Tick, GetDataStruct()->jumpCmdHolder.JumpUp_Tick, GetDataStruct()->jumpCmdHolder.outsideJumpCmdCount,
							GetDataStruct()->total_bhopCount,
							GetDataStruct()->goodBhopsCount,
							GetDataStruct()->perfectBhopsPercent,
							GetDataStruct()->perfectBhopsCount,
							GetDataStruct()->isOnGround).c_str();
}

const char * Detection_BunnyHopProgram::GetDataDump()
{
	return Helpers::format("BunnyHopInfoT { OnGroundHolderT { %d, %d, %d }, JumpCmdHolderT { %d, %d, %d, %d }, %d, %d, %d, %d }\0",
							GetDataStruct()->onGroundHolder.onGround_Tick, GetDataStruct()->onGroundHolder.notOnGround_Tick, GetDataStruct()->onGroundHolder.jumpCount,
							GetDataStruct()->jumpCmdHolder.lastJumpCmdState, GetDataStruct()->jumpCmdHolder.JumpDown_Tick, GetDataStruct()->jumpCmdHolder.JumpUp_Tick, GetDataStruct()->jumpCmdHolder.outsideJumpCmdCount,
							GetDataStruct()->total_bhopCount,
							GetDataStruct()->goodBhopsCount,
							GetDataStruct()->perfectBhopsPercent,
							GetDataStruct()->perfectBhopsCount,
							GetDataStruct()->isOnGround).c_str();
}
