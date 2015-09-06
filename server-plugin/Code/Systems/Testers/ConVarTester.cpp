#include "ConVarTester.h"
#include "plugin.h"

/////////////////////////////////////////////////////////////////////////
// ConVarTester
/////////////////////////////////////////////////////////////////////////

ConVarTester::ConVarTester() : 
	BaseSystem(),
	AsyncNczFilteredPlayersList(),
	BaseFramedTester(),
	PlayerDataStructHandler<CurrentConVarRequestT>(),
	Singleton<ConVarTester>()
{
}

ConVarTester::~ConVarTester()
{
}

const char * ConVarTester::GetName()
{
	return "ConVarTester";
}

SlotStatus ConVarTester::GetFilter()
{
	return PLAYER_CONNECTED;
}

void ConVarTester::ProcessTests()
{
	if(!IsActive()) return;
	m_metrics.StartExec();
	CNoCheatZPlugin::GetInstance()->ncz_frame.StartExec();
	if(g_pCVar->FindVar("sv_cheats")->GetBool())
	{
		if(HasVerbose()) Msg("%f : sv_cheats set to 1. Skipping ConVarTest ...\n", Plat_FloatTime());
		return;
	}
	NczPlayer* player = GetNextPlayer();
	if(player)
	{
		ProcessPlayerTest(player);
	}
	m_metrics.EndExec();
	CNoCheatZPlugin::GetInstance()->ncz_frame.EndExec();
}

void ConVarTester::ProcessPlayerTest(NczPlayer* player)
{
	CurrentConVarRequestT* req = GetPlayerDataStruct(player);
	if(req->isSent && !req->isReplyed)
	{
		if(Plat_FloatTime() - 30.0 > req->timeStart)
		{
			player->Kick("ConVar request timed out");
		}
	}
	else if(!req->isSent && req->isReplyed)
	{
		ConVarInfoT* nextConvar = GetNextConvar(req->ruleset->name);
		CIFaceManager::GetInstance()->GetIpluginhelpers()->StartQueryCvarValue(player->GetEdict(), nextConvar->name);
		req->isSent = true;
		req->isReplyed = false;
		req->ruleset = nextConvar;
		req->timeStart = Plat_FloatTime();
		if(HasVerbose()) Msg("%f : Sending %s ConVar request to ent-id %d\n", Plat_FloatTime(), nextConvar->name, player->GetIndex());
	}
	else
	{
		CIFaceManager::GetInstance()->GetIpluginhelpers()->StartQueryCvarValue(player->GetEdict(), "developer");
		req->isSent = true;
		req->isReplyed = false;
		req->ruleset = FindConvarRuleset("developer");
		req->timeStart = Plat_FloatTime();
		if(HasVerbose()) Msg("%f : Sending %s ConVar request to ent-id %d\n", Plat_FloatTime(), "developer", player->GetIndex());
	}
}

ConVarInfoT* ConVarTester::FindConvarRuleset(const char * name)
{
	for(std::list<ConVarInfoT>::iterator it = m_convars_rules.begin(); it != m_convars_rules.end(); ++it)
	{
		if(Helpers::bStriEq((*it).name, name))
		{
			return &*it;
		}
	}
	return nullptr;
}

ConVarInfoT* ConVarTester::GetNextConvar(const char * current_convar_name)
{
	for(std::list<ConVarInfoT>::iterator it = m_convars_rules.begin(); it != m_convars_rules.end(); ++it)
	{
		if(Helpers::bStriEq((*it).name, current_convar_name))
		{
			++it;
			if(it != m_convars_rules.end()) return &(*it);
			else return &(*(m_convars_rules.begin()));
		}
	}
	return &(*(m_convars_rules.begin()));
}

bool ConVarTester::sys_cmd_fn ( const CCommand &args )
{
	if(Helpers::bStriEq("AddRule", args.Arg(2))) 
		// example : ncz ConVarTester AddRule sv_cheats 0 NO_VALUE
	{
		if(args.ArgC() >= 5)
		{
			ConVarRuleT rule;
			if(args.ArgC() == 6)
			{
				if(Helpers::bStriEq("NO_VALUE", args.Arg(5))) rule = NO_VALUE;
				else if(Helpers::bStriEq("SAME", args.Arg(5))) rule = SAME;
				else if(Helpers::bStriEq("SAME_FLOAT", args.Arg(5))) rule = SAME_FLOAT;
				else if(Helpers::bStriEq("SAME_AS_SERVER", args.Arg(5))) rule = SAME_AS_SERVER;
				else if(Helpers::bStriEq("SAME_FLOAT_AS_SERVER", args.Arg(5))) rule = SAME_FLOAT_AS_SERVER;
				else
				{
					Msg("Arg %s not found.\n", args.Arg(5));
					return true;
				}
			}
			else
			{
				if(Helpers::bStriEq("sv_", args.Arg(3), 0U, 3)) rule = SAME_AS_SERVER;
				else rule = SAME;

				std::string value = args.Arg(4);
				if(value.find('.') != std::string::npos)
				{
					if(rule == SAME_AS_SERVER) rule = SAME_FLOAT_AS_SERVER;
					else rule = SAME_FLOAT;
				}
				else
				{
					if(rule != SAME_AS_SERVER) rule = SAME_FLOAT_AS_SERVER;
					else rule = SAME_FLOAT;
				}
			}
			AddConvarRuleset(args.Arg(3), args.Arg(4), rule, false);
		}
	}
	if(Helpers::bStriEq("ResetRules", args.Arg(2))) 
		// example : ncz ConVarTester ResetRules
	{
		m_convars_rules.
	}
}

void ConVarTester::AddConvarRuleset(const char * name, const char * value, ConVarRuleT rule, bool safe)
{
	const char * sv_value = value;
	if(rule == SAME_AS_SERVER)
	{
		ConVar * sv_cvar = g_pCVar->FindVar(name);
		if(sv_cvar)
		{
			sv_value = sv_cvar->GetString();
		}
	}
	ConVarInfoT ruleset;
	ruleset.name = name;
	ruleset.value = sv_value;
	ruleset.rule = rule;
	ruleset.safe = safe;
	m_convars_rules.push_back(ruleset);
}

void ConVarTester::OnQueryCvarValueFinished(NczPlayer* player, EQueryCvarValueStatus eStatus, const char *pCvarName, const char *pCvarValue)
{
	if(!IsActive()) return;
	m_metrics.StartExec();
	CNoCheatZPlugin::GetInstance()->ncz_frame.StartExec();
	if(HasVerbose()) Msg("%f : Received %s ConVar reply for ent-id %d\n", Plat_FloatTime(), pCvarName, player->GetIndex());

	if(g_pCVar->FindVar("sv_cheats")->GetBool())
	{
		if(HasVerbose()) Msg("%f : sv_cheats set to 1. Skipping ...\n", Plat_FloatTime());
		CNoCheatZPlugin::GetInstance()->ncz_frame.EndExec();
		m_metrics.EndExec();
		return;
	}

	ConVarInfoT* ruleset = FindConvarRuleset(pCvarName);
	if(!ruleset)
	{
unexpected:
		if(HasVerbose()) Msg("%f : Unexpected reply. Skipping ...\n", Plat_FloatTime());
		m_metrics.EndExec();
		CNoCheatZPlugin::GetInstance()->ncz_frame.EndExec();
		return;
	}
	CurrentConVarRequest* req = GetPlayerDataStruct(player);

	if(!req->isSent) goto unexpected;
	if(!Helpers::bStriEq(req->ruleset->name, ruleset->name)) goto unexpected;

	req->isReplyed = true;

	switch(eStatus)
	{
	case eQueryCvarValueStatus_ValueIntact:
		{
			if(HasVerbose()) Msg("%f : eQueryCvarValueStatus_ValueIntact\n", Plat_FloatTime());
			if(ruleset->rule == NO_VALUE)
			{
				if(HasVerbose()) Msg("%f : Was expecting eQueryCvarValueStatus_CvarNotFound\n", Plat_FloatTime());
				player->Kick();
			}
			else if(ruleset->rule == SAME)
			{
				if(Helpers::bStrEq(ruleset->value, pCvarValue))
				{
					if(HasVerbose()) Msg("%f : Value %s is correct\n", Plat_FloatTime(), pCvarValue);
				}
				else
				{
					if(HasVerbose()) Msg("%f : Value %s is NOT correct\n", Plat_FloatTime(), pCvarValue);
					Detection_ConVar* pDetection = new Detection_ConVar();
					pDetection->PrepareDetectionData(req);
					pDetection->PrepareDetectionLog(player, this);
					pDetection->Log();
					pDetection->Report();
					BanRequest::GetInstance()->AddAsyncBan(player, 0, "Banned by NoCheatZ 4");
				}
			}
			else if(ruleset->rule == SAME_AS_SERVER)
			{
				if(Helpers::bStrEq(g_pCVar->FindVar(ruleset->name)->GetString(), pCvarValue))
				{
					if(HasVerbose()) Msg("%f : Value %s is correct\n", Plat_FloatTime(), pCvarValue);
				}
				else
				{
					if(HasVerbose()) Msg("%f : Value %s is NOT correct\n", Plat_FloatTime(), pCvarValue);
					Detection_ConVar* pDetection = new Detection_ConVar();
					pDetection->PrepareDetectionData(req);
					pDetection->PrepareDetectionLog(player, this);
					pDetection->Log();
					pDetection->Report();
					BanRequest::GetInstance()->AddAsyncBan(player, 0, "Banned by NoCheatZ 4");
				}
			}
			else if(ruleset->rule == SAME_FLOAT)
			{
				double fcval = atoi(pCvarValue);
				double fsval = atoi(ruleset->value);
				if(fcval == fsval)
				{
					if(HasVerbose()) Msg("%f : Value %s is correct\n", Plat_FloatTime(), pCvarValue);
				}
				else
				{
					if(HasVerbose()) Msg("%f : Value %s is NOT correct\n", Plat_FloatTime(), pCvarValue);
					Detection_ConVar* pDetection = new Detection_ConVar();
					pDetection->PrepareDetectionData(req);
					pDetection->PrepareDetectionLog(player, this);
					pDetection->Log();
					pDetection->Report();
					BanRequest::GetInstance()->AddAsyncBan(player, 0, "Banned by NoCheatZ 4");
				}
			}
			else if(ruleset->rule == SAME_FLOAT_AS_SERVER)
			{
				double fcval = atoi(pCvarValue);
				double fsval = atoi(g_pCVar->FindVar(ruleset->name)->GetString());
				if(fcval == fsval)
				{
					if(HasVerbose()) Msg("%f : Value %s is correct\n", Plat_FloatTime(), pCvarValue);
				}
				else
				{
					if(HasVerbose()) Msg("%f : Value %s is NOT correct\n", Plat_FloatTime(), pCvarValue);
					Detection_ConVar* pDetection = new Detection_ConVar();
					pDetection->PrepareDetectionData(req);
					pDetection->PrepareDetectionLog(player, this);
					pDetection->Log();
					pDetection->Report();
					BanRequest::GetInstance()->AddAsyncBan(player, 0, "Banned by NoCheatZ 4");
				}
			}
			break;
		}

	case eQueryCvarValueStatus_CvarNotFound:
		{
			if(HasVerbose()) Msg("%f : eQueryCvarValueStatus_CvarNotFound\n", Plat_FloatTime());
			if(ruleset->rule != NO_VALUE)
			{
				if(HasVerbose()) Msg("%f : Was expecting eQueryCvarValueStatus_ValueIntact\n", Plat_FloatTime());
				Detection_ConVar* pDetection = new Detection_ConVar();
				pDetection->PrepareDetectionData(req);
				pDetection->PrepareDetectionLog(player, this);
				pDetection->Log();
				pDetection->Report();
				BanRequest::GetInstance()->AddAsyncBan(player, 0, "Banned by NoCheatZ 4");
			}
			break;
		}

	case eQueryCvarValueStatus_NotACvar:
		{
			if(HasVerbose()) Msg("%f : eQueryCvarValueStatus_NotACvar\n", Plat_FloatTime());
			goto unexpected2;
		}

	case eQueryCvarValueStatus_CvarProtected:
		{
			if(HasVerbose()) Msg("%f : eQueryCvarValueStatus_CvarProtected\n", Plat_FloatTime());
			goto unexpected2;
		}

	default:
		{
			if(HasVerbose()) Msg("%f : eQueryCvarValueStatus_UnknownStatus\n", Plat_FloatTime());
			goto unexpected2;
		}
	}
	
	req->isSent = false;
	CNoCheatZPlugin::GetInstance()->ncz_frame.EndExec();
	m_metrics.EndExec();
	return;
unexpected2:
	if(HasVerbose()) Msg("%f : Was expecting eQueryCvarValueStatus_ValueIntact\n", Plat_FloatTime());
	Detection_ConVar* pDetection = new Detection_ConVar();
	pDetection->PrepareDetectionData(req);
	pDetection->PrepareDetectionLog(player, this);
	pDetection->Log();
	pDetection->Report();
	BanRequest::GetInstance()->AddAsyncBan(player, 0, "Banned by NoCheatZ 4");
	CNoCheatZPlugin::GetInstance()->ncz_frame.EndExec();
	m_metrics.EndExec();
}

void ConVarTester::Load()
{
	AddConvarRuleset("developer", "0", SAME);
	AddConvarRuleset("sv_cheats", "0", SAME_AS_SERVER);
	AddConvarRuleset("sv_accelerate", "0", SAME_AS_SERVER);
	AddConvarRuleset("sv_showimpacts", "0", SAME_AS_SERVER);
	AddConvarRuleset("sv_showlagcompensation", "0", SAME_AS_SERVER);
	AddConvarRuleset("host_framerate", "0", SAME_FLOAT_AS_SERVER);
	AddConvarRuleset("host_timescale", "0", SAME_FLOAT_AS_SERVER);
	AddConvarRuleset("r_visualizetraces", "0", SAME_AS_SERVER);
	AddConvarRuleset("mat_normalmaps", "0", SAME_AS_SERVER);
	AddConvarRuleset("mp_playerid", "0", SAME_AS_SERVER);
	AddConvarRuleset("net_fakeloss", "0", SAME);
	AddConvarRuleset("net_fakelag", "0", SAME);
	AddConvarRuleset("net_fakejitter", "0", SAME);
	AddConvarRuleset("r_drawothermodels", "1", SAME);
	AddConvarRuleset("r_shadowwireframe", "0", SAME);
	AddConvarRuleset("r_avglight", "1", SAME);
	AddConvarRuleset("r_novis", "0", SAME);
	AddConvarRuleset("r_drawparticles", "1", SAME);
	AddConvarRuleset("r_drawopaqueworld", "1", SAME);
	AddConvarRuleset("r_drawtranslucentworld", "1", SAME);
	AddConvarRuleset("r_drawmodelstatsoverlay", "0", SAME);
	AddConvarRuleset("r_skybox", "1", SAME);
	AddConvarRuleset("r_aspectratio", "0", SAME);
	AddConvarRuleset("r_drawskybox", "1", SAME);
	AddConvarRuleset("r_showenvcubemap", "0", SAME);
	AddConvarRuleset("r_drawlights", "0", SAME);
	AddConvarRuleset("r_drawrenderboxes", "0", SAME);
	AddConvarRuleset("mat_wireframe", "0", SAME);
	AddConvarRuleset("mat_drawwater", "1", SAME);
	AddConvarRuleset("mat_loadtextures", "1", SAME);
	AddConvarRuleset("mat_showlowresimage", "0", SAME);
	AddConvarRuleset("mat_fillrate", "0", SAME);
	AddConvarRuleset("mat_proxy", "0", SAME);
	AddConvarRuleset("mem_force_flush", "0", SAME);
	AddConvarRuleset("fog_enable", "1", SAME);
	AddConvarRuleset("cl_pitchup", "89", SAME);
	AddConvarRuleset("cl_pitchdown", "89", SAME);
	AddConvarRuleset("cl_bobcycle", "0.8", SAME_FLOAT);
	AddConvarRuleset("cl_leveloverviewmarker", "0", SAME);
	AddConvarRuleset("snd_visualize", "0", SAME);
	AddConvarRuleset("snd_show", "0", SAME);
	AddConvarRuleset("openscript", "", NO_VALUE);
	AddConvarRuleset("openscript_version", "", NO_VALUE);
	AddConvarRuleset("ms_sv_cheats", "", NO_VALUE);
	AddConvarRuleset("ms_r_drawothermodels", "", NO_VALUE);
	AddConvarRuleset("ms_chat", "", NO_VALUE);
	AddConvarRuleset("ms_aimbot", "", NO_VALUE);
	AddConvarRuleset("wallhack", "", NO_VALUE);
	AddConvarRuleset("cheat_chat", "", NO_VALUE);
	AddConvarRuleset("cheat_chams", "", NO_VALUE);
	AddConvarRuleset("cheat_dlight", "", NO_VALUE);
	AddConvarRuleset("SmAdminTakeover", "", NO_VALUE);
	AddConvarRuleset("ManiAdminTakeover", "", NO_VALUE);
	AddConvarRuleset("ManiAdminHacker", "", NO_VALUE);
	AddConvarRuleset("byp_svc", "", NO_VALUE);
	AddConvarRuleset("byp_speed_hts", "", NO_VALUE);
	AddConvarRuleset("byp_speed_hfr", "", NO_VALUE);
	AddConvarRuleset("byp_render_rdom", "", NO_VALUE);
	AddConvarRuleset("byp_render_mwf", "", NO_VALUE);
	AddConvarRuleset("byp_render_rdp", "", NO_VALUE);
	AddConvarRuleset("byp_fake_lag", "", NO_VALUE);
	AddConvarRuleset("byp_fake_loss", "", NO_VALUE);

	CurrentConVarRequestT* default_req = GetDefaultDataStruct();
	default_req->isSent = false;
	default_req->isReplyed = false;
	default_req->ruleset = FindConvarRuleset("developer");
	default_req->timeStart = 0.0;

	BaseFramedTester::RegisterFramedTester(this);
}

void ConVarTester::Unload()
{
	BaseFramedTester::RemoveFramedTester(this);

	PLAYERS_LOOP_RUNTIME
	{
		ResetPlayerDataStruct(ph->playerClass);
	}
	END_PLAYERS_LOOP

	m_convars_rules.clear();
}

const char * Detection_ConVar::GetDataDump()
{
	return Helpers::format("CurrentConVarRequest { %d, %d, %f, ConVarInfo { %s, %s, %d} }\0",
							GetDataStruct()->isSent, GetDataStruct()->isReplyed, GetDataStruct()->timeStart,
							GetDataStruct()->ruleset->name, GetDataStruct()->ruleset->value, GetDataStruct()->ruleset->rule).c_str();
}

const char * Detection_ConVar::GetDetectionLogMessage()
{
	return Helpers::format("%s ConVar Bypasser", GetDataStruct()->ruleset->name).c_str();
}

