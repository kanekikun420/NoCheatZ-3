#include "BaseSystem.h"
#include "plugin.h"

/////////////////////////////////////////////////////////////////////////
// BaseSystem
/////////////////////////////////////////////////////////////////////////

std::list<BaseSystem*> BaseSystem::m_systemsList;

BaseSystem::BaseSystem() : m_isActive(false), m_verbose(false), m_isDisabled(false), m_configState(true), m_metrics("basesystem")
{
	m_metrics.Reset();
	BaseSystem::m_systemsList.push_back(this);
}

BaseSystem::~BaseSystem()
{
	BaseSystem::m_systemsList.remove(this);
}

void BaseSystem::UnloadAllSystems()
{
	for(std::list<BaseSystem*>::iterator it = m_systemsList.begin(); it != m_systemsList.end(); ++it)
	{
		(*it)->SetActive(false);
	}
}

void BaseSystem::SetVerbose(bool verbose)
{
	m_verbose = verbose;
}

void BaseSystem::SetActive(bool active)
{
	if(m_isActive == active) return;
	else if(active)
	{
		if(!m_isDisabled)
		{
			if(m_configState)
			{
				if(NczPlayerManager::GetInstance()->GetPlayerCount(PLAYER_CONNECTED) + NczPlayerManager::GetInstance()->GetPlayerCount(PLAYER_IN_TESTS) > 0)
				{
					if(HasVerbose()) Msg("%f : Starting %s\n", Plat_FloatTime(), GetName());
					m_isActive = true;
					Load();
				}
				else
				{
					if(HasVerbose()) Msg("%f : Wont start system %s : No player present\n", Plat_FloatTime(), GetName());
				}
			}
			else
			{
				if(HasVerbose()) Msg("%f : Wont start system %s : Disabled by server configuration file\n", Plat_FloatTime(), GetName());
			}
		}
		else
		{
			Msg("%f : Unable to start %s (Disabled by master configuration)\n", Plat_FloatTime(), GetName());
		}
	}
	else
	{
		if(HasVerbose()) Msg("%f : Stoping %s\n", Plat_FloatTime(), GetName());
		m_isActive = false;
		Unload();
	}
}

void BaseSystem::SetDisabled(bool disabled)
{
	if(disabled && !m_isDisabled)
	{
		SetActive(false);
		m_isDisabled = true;
		Msg("%f : System %s disabled by remote configuration.\n", Plat_FloatTime(), GetName());
	}
	else if(!disabled && m_isDisabled)
	{
		m_isDisabled = false;
	}
}

void BaseSystem::ncz_cmd_fn ( const CCommand &args )
{
	if(args.ArgC() < 2)
		goto error;
	for(std::list<BaseSystem*>::iterator it = m_systemsList.begin(); it != m_systemsList.end(); ++it)
	{
		if(Helpers::bStriEq((*it)->GetName(), args.Arg(1)))
		{
			if(Helpers::bStriEq("enable", args.Arg(2)))
			{
				(*it)->SetConfig(true);
				(*it)->SetActive(true);
			}
			else if(Helpers::bStriEq("disable", args.Arg(2)))
			{
				(*it)->SetConfig(false);
				(*it)->SetActive(false);
			}
			else if(Helpers::bStriEq("verbose", args.Arg(2)))
			{
				if(args.ArgC() > 2)
				{
					if(Helpers::bStriEq("on", args.Arg(3)))
					{
						(*it)->SetVerbose(true);
						return;
					}
					else if(Helpers::bStriEq("off", args.Arg(3)))
					{
						(*it)->SetVerbose(false);
						return;
					}
				}
				Msg("arg %s not found. Try :\non\noff\n\n", args.Arg(3));
				return;
			}
			else if(!(*it)->sys_cmd_fn(args))
			{
				Msg("action %s not found.\nTry : %s\n", args.Arg(2), (*it));
			}

			return;
		}		
	}
	if(Helpers::bStriEq("metrics", args.Arg(1)))
	{
		if(CNoCheatZPlugin::GetInstance()->game_frame.m_avgExecTime == 0.0) Msg("Plugin usage : 0%%\n"); 
		else Msg("Plugin usage : %3.2f%%\n", (CNoCheatZPlugin::GetInstance()->ncz_frame.m_avgExecTime / CNoCheatZPlugin::GetInstance()->game_frame.m_avgExecTime) * 100.0);
		CNoCheatZPlugin::GetInstance()->ncz_frame.PrintResults();
		for(std::list<BaseSystem*>::iterator it = m_systemsList.begin(); it != m_systemsList.end(); ++it)
		{
			Msg("---\n");
			if(CNoCheatZPlugin::GetInstance()->game_frame.m_totalExecTime == 0.0) Msg("%s (0%%)\n", (*it)->GetName());
			else Msg("%s (%3.2f%%)\n", (*it)->GetName(), ((*it)->m_metrics.m_totalExecTime / CNoCheatZPlugin::GetInstance()->ncz_frame.m_totalExecTime) * 100.0);
			(*it)->m_metrics.PrintResults();
		}
		return;
	}
	Msg("System %s not found.\n", args.Arg(1));
error:
	Msg("Usage: ncz system arg1 arg2 ...\n");
	Msg("Systems list :\n");
	for(std::list<BaseSystem*>::iterator it = m_systemsList.begin(); it != m_systemsList.end(); ++it)
	{
		Msg("%s", (*it)->GetName());
		if((*it)->IsActive())
		{
			Msg(" (Running");
			if(CNoCheatZPlugin::GetInstance()->game_frame.m_avgExecTime == 0.0) Msg(" 0%%)\n");
			else Msg(" %3.2f%%)\n", ((*it)->m_metrics.m_avgExecTime / CNoCheatZPlugin::GetInstance()->ncz_frame.m_avgExecTime) * 100.0);
		}
		else if((*it)->IsDisabledByMaster())
		{
			Msg(" (Disabled by master)\n");
		}
		else if(!(*it)->IsEnabledByConfig())
		{
			Msg(" (Disabled by config)\n");
		}
		else
		{
			Msg(" (Sleeping - Waiting for players)\n");
		}
	}
	Msg("Plugin\nPipeline\nLogger\nMetrics\nUpdate");
	Msg("\n");
}
 
ConCommand ncz_cmd( "ncz", BaseSystem::ncz_cmd_fn , "NoCheatZ", 0);
