#include "ShotTester.h"

/*
	Test each player to see if they use any script to help them fire more bullets (= RapidFire)

	Some old mouses can also make multiple clicks because of an electronic issue, not because the player itself use a script.
	We have to make the difference by using statistics.
*/

ShotTester::ShotTester(void) :
	BaseSystem(),
	Logger(),
	PlayerRunCommandHookListener(),
	PlayerDataStructHandler<ShotStatsT>(),
	Singleton<ShotTester>()
{
	m_name = "ShotTester";
}

ShotTester::~ShotTester(void)
{
	Unload();
}

SlotStatus ShotTester::GetFilter()
{
	return PLAYER_IN_TESTS;
}

void ShotTester::Load()
{
	PlayerRunCommandHookListener::RegisterPlayerRunCommandHookListener(this);
}

void ShotTester::Unload()
{
	PlayerRunCommandHookListener::RemovePlayerRunCommandHookListener(this);

	PLAYERS_LOOP_RUNTIME
	{
		ResetPlayerDataStruct(ph->playerClass);
	}
	END_PLAYERS_LOOP
}

void TriggerStat(ShotStatHandlerT* handler, double up_time, double down_time, size_t clicks)
{
	++(handler->n);
	handler->avg_time = (handler->avg_time + (up_time - down_time) / (double)(handler->n));
	handler->ratio = ((double)(handler->n) / (double)(clicks)) * 100.0;
}

void OutputStat(ShotStatHandlerT* handler)
{
	Msg(Helpers::format("{ count %lu, ratio %3.2f, avg %3.2f }", handler->n, handler->ratio, handler->avg_time).c_str());
}

bool ShotTester::PlayerRunCommandCallback(NczPlayer* player, CUserCmd* pCmd)
{	
	bool drop_cmd = false;

	ShotStatsT* playerData = GetPlayerDataStruct(player);
	CUserCmd* lastcmd = PlayerRunCommandHookListener::GetLastUserCmd(player);

	if((pCmd->buttons & IN_ATTACK) && !(lastcmd->buttons & IN_ATTACK))
	{
		if(HasVerbose()) Msg("Player %s : %f : IN_ATTACK button down.\n", player->GetName(), Plat_FloatTime());
		playerData->down_time = Plat_FloatTime();
	}
	else if((lastcmd->buttons & IN_ATTACK) && !(pCmd->buttons & IN_ATTACK))
	{
		playerData->up_time = Plat_FloatTime();
		TriggerStat(&(playerData->clicks), playerData->up_time, playerData->down_time, playerData->clicks.n);
		if(HasVerbose()) Msg("Player %s : %f : IN_ATTACK button up.\n", player->GetName(), Plat_FloatTime());

		if(playerData->up_time - playerData->down_time <= SHORT_TIME)
		{
			if(Plat_FloatTime() - playerData->last_detection > 1.0)
			{
				if(playerData->row == 1)
				{
					if(playerData->on_target.ratio > 25.0 && playerData->clicks.n > 50)
					{
						Detection_TriggerBot pDetection = Detection_TriggerBot();
						pDetection.PrepareDetectionData(playerData);
						pDetection.PrepareDetectionLog(player, this);
						pDetection.Log();
					}
				}
				playerData->row = 0;
			}
			if(playerData->row > 10)
			{
				if(playerData->short_clicks.ratio > 40.0 && playerData->clicks.n > 50)
				{
					Detection_AutoPistol pDetection = Detection_AutoPistol();
					pDetection.PrepareDetectionData(playerData);
					pDetection.PrepareDetectionLog(player, this);
					pDetection.Log();
				}
			}
			playerData->last_detection = Plat_FloatTime();
			++(playerData->row);

			TriggerStat(&(playerData->short_clicks), playerData->up_time, playerData->down_time, playerData->clicks.n);
			if(player->GetWpnShotType() == HAND) TriggerStat(&(playerData->with_hand), playerData->up_time, playerData->down_time, playerData->clicks.n);
			else if(player->GetWpnShotType() == PISTOL) TriggerStat(&(playerData->with_pistol), playerData->up_time, playerData->down_time, playerData->clicks.n);
			else TriggerStat(&(playerData->with_auto), playerData->up_time, playerData->down_time, playerData->clicks.n);
			if(player->aimingAt() > 0) TriggerStat(&(playerData->on_target), playerData->up_time, playerData->down_time, playerData->clicks.n);
			drop_cmd = true;

			if(HasVerbose())
			{
				Msg("%f - clicks ", Plat_FloatTime());
				OutputStat(&(playerData->clicks));
				Msg(", short_clicks ");
				OutputStat(&(playerData->short_clicks));
				Msg(", with_hand ");
				OutputStat(&(playerData->with_hand));
				Msg(", with_pistol ");
				OutputStat(&(playerData->with_pistol));
				Msg(", with_auto ");
				OutputStat(&(playerData->with_auto));
				Msg(", on_target ");
				OutputStat(&(playerData->on_target));
				Msg(", row %d\n", playerData->row);
			}
		}
	}
	return drop_cmd;
}


const char * ShotDetection::GetDataDump()
{
	return Helpers::format("ShotStatsT { %f, %f, Clicks { %lu, %f, %f }, ShortClicks { %lu, %f, %f }, WithHand { %lu, %f, %f }, WithPistol { %lu, %f, %f }, WithAuto { %lu, %f, %f }, OnTarget { %lu, %f, %f }, %lu, %f }",
							GetDataStruct()->up_time, GetDataStruct()->down_time, 
							GetDataStruct()->clicks.n, GetDataStruct()->clicks.ratio, GetDataStruct()->clicks.avg_time, 
							GetDataStruct()->short_clicks.n, GetDataStruct()->short_clicks.ratio, GetDataStruct()->short_clicks.avg_time, 
							GetDataStruct()->with_hand.n, GetDataStruct()->with_hand.ratio, GetDataStruct()->with_hand.avg_time, 
							GetDataStruct()->with_pistol.n, GetDataStruct()->with_pistol.ratio, GetDataStruct()->with_pistol.avg_time, 
							GetDataStruct()->with_auto.n, GetDataStruct()->with_auto.ratio, GetDataStruct()->with_auto.avg_time, 
							GetDataStruct()->on_target.n, GetDataStruct()->on_target.ratio, GetDataStruct()->on_target.avg_time, 
							GetDataStruct()->row, GetDataStruct()->last_detection).c_str();
}
