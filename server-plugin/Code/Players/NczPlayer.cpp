#include "NczPlayer.h"

//---------------------------------------------------------------------------------
// NczPlayer
//---------------------------------------------------------------------------------

NczPlayer::NczPlayer(const int index) : cIndex(index), m_time_connected(0.0)
{
	m_edict = CIFaceManager::GetInstance()->GetIengine()->PEntityOfEntIndex(index);
	Assert(m_edict);
	m_userid = CIFaceManager::GetInstance()->GetIengine()->GetPlayerUserId(m_edict);
	Assert(m_userid);
	m_channelinfo = CIFaceManager::GetInstance()->GetIengine()->GetPlayerNetInfo(index);
}

int NczPlayer::GetUserid()
{
	return m_userid;
}

float NczPlayer::GetTimeConnected()
{
	return Plat_FloatTime() - m_time_connected;
}

const char * NczPlayer::GetName()
{
	return GetPlayerInfo()->GetName();
}

IPlayerInfo *NczPlayer::GetPlayerInfo()
{
	return CIFaceManager::GetInstance()->GetIplayers()->GetPlayerInfo(m_edict);
}

const char * NczPlayer::GetSteamID()
{
	return CIFaceManager::GetInstance()->GetIengine()->GetPlayerNetworkIDString(m_edict);
}

const char * NczPlayer::GetIPAddress()
{
	return GetChannelInfo()->GetAddress();
}

WpnShotType NczPlayer::GetWpnShotType()
{
	IPlayerInfo* playerinfo = GetPlayerInfo();

	if(playerinfo)
	{
		const char * wpn_name = playerinfo->GetWeaponName();

		switch(wpn_name[7])
		{
		case 's': //scout & sg550 & smokegrenade
			switch(wpn_name[8])
			{
			case 'c':
				return PISTOL;
			case 'g':
				return AUTO;
			
			default: //m
				return HAND;
			};

		case 'a': //awp & ak47
			switch(wpn_name[8])
			{
			case 'w':
				return PISTOL;
			case 'k':
				return AUTO;
			};
		case 'f': //fiveseven & flashbang
			switch(wpn_name[8])
			{
			case 'i':
				return PISTOL;
			default: //l
				return HAND;
			};
		case 'g': //glock & g3sg1
			switch(wpn_name[8])
			{
			case 'l':
				return PISTOL;
			case '3':
				return AUTO;
			};
		case 'p': //p228
		case 'e': //elite
		case 'u': //usp
		case 'd': //deagle
		case 'x': //xm1014
			return PISTOL;
		case 'm': //m3 & m4a1
			switch(wpn_name[8])
			{
			case '3':
				return PISTOL;
			case '4':
				return AUTO;
			};

		default:
			return HAND;
		};
	}
	return HAND;
}

int NczPlayer::aimingAt()
{
	trace_t trace;
	Ray_t ray;

	edict_t* edict = GetEdict();
	if(!edict) return -1;
	IPlayerInfo* playerinfo = GetPlayerInfo();
	if(!playerinfo) return -1;
	CBotCmd cmd = playerinfo->GetLastUserCommand();

	Vector earPos;
	CIFaceManager::GetInstance()->GetIclients()->ClientEarPosition(edict, &earPos);
	Vector eyePos = earPos;

	QAngle eyeAngles = cmd.viewangles;
	Vector vEnd;
	AngleVectors(eyeAngles, &vEnd);
	vEnd = vEnd * 8192.0f + eyePos;
	
	ray.Init(eyePos,vEnd);
	CIFaceManager::GetInstance()->GetItrace()->TraceRay( ray, (CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_MONSTER | CONTENTS_DEBRIS | CONTENTS_HITBOX), NULL, &trace );
	
	edict_t* target = CIFaceManager::GetInstance()->GetIents()->BaseEntityToEdict(trace.m_pEnt);
	if ( target && !CIFaceManager::GetInstance()->GetIengine()->IndexOfEdict(target) == 0 && !trace.allsolid )
	{
		if(!Helpers::isValidEdict(target)) return -1;
#undef GetClassName
		if(strcmp(target->GetClassName(), "player") == 0)
		{
			IPlayerInfo* targetinfo = CIFaceManager::GetInstance()->GetIplayers()->GetPlayerInfo(target);
			if(targetinfo)
			{
				int ta = targetinfo->GetTeamIndex();
				int tb = playerinfo->GetTeamIndex();
				if( ta != tb )
				{
					if( targetinfo->IsPlayer() && !targetinfo->IsHLTV() && !targetinfo->IsObserver() )
					{
						return CIFaceManager::GetInstance()->GetIengine()->IndexOfEdict(target);
					}
				} 
			}
		}
	}
	return -1;
}

INetChannelInfo* NczPlayer::GetChannelInfo()
{
	return m_channelinfo;
}

edict_t * NczPlayer::GetEdict()
{
	return m_edict;
}

bool NczPlayer::isValidEdict()
{
	return Helpers::isValidEdict(m_edict);
}

void NczPlayer::OnConnect()
{
	m_time_connected = Plat_FloatTime();
}

void NczPlayer::Kick(const char * msg)
{
	//if (Config::GetInstance()->GetConfigData()->kick_ban)
	if(1)
	{
		CIFaceManager::GetInstance()->GetIengine()->ServerCommand(
				Helpers::format(
				"kickid %d [NoCheatZ 4] %s\n", this->GetUserid(), msg).c_str()
											);
	}
}

void NczPlayer::Ban(const char * msg, int minutes)
{
	//if (Config::GetInstance()->GetConfigData()->kick_ban)
	if(1)
	{
		CIFaceManager::GetInstance()->GetIengine()->ServerCommand(Helpers::format("banid %d %s\n", minutes, GetSteamID()).c_str());
		Kick(msg);
		CIFaceManager::GetInstance()->GetIengine()->ServerCommand("writeid\n");
		if(!Helpers::bStrEq("127.0.0.1", GetIPAddress()))
		{
			CIFaceManager::GetInstance()->GetIengine()->ServerCommand(Helpers::format("addip 1440 \"%s\"\n", GetIPAddress()).c_str());
			CIFaceManager::GetInstance()->GetIengine()->ServerCommand("writeip\n");
		}
	}
}
