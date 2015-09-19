#include "Systems/BanRequest.h"

void BanRequest::SetWaitTime(double wait_time)
{
	m_wait_time = wait_time;
}

void BanRequest::AddAsyncBan(NczPlayer* player, int ban_time, const char * kick_message)
{
//	if (Config::GetInstance()->GetConfigData()->kick_ban)
	{
		PlayerBanRequestT* req = new PlayerBanRequestT;
		req->ban_time = ban_time;
		req->userid = player->GetUserid();
		req->request_time = Plat_FloatTime();
		req->kick_message = kick_message;
		strcpy_s(req->player_name, 24, player->GetName());
		strcpy_s(req->steamid, 24, player->GetSteamID());
		strcpy_s(req->ip, 24, player->GetIPAddress());
		m_requests.push_back(req);
	}
}

void BanRequest::Think()
{
retry:
	for(std::list<PlayerBanRequestT*>::iterator it = m_requests.begin(); it != m_requests.end(); ++it)
	{
		if((*it)->request_time + m_wait_time < Plat_FloatTime())
		{
			CIFaceManager::GetInstance()->GetIengine()->ServerCommand(Helpers::format("banid %d %s\n", (*it)->ban_time, (*it)->steamid).c_str());
			CIFaceManager::GetInstance()->GetIengine()->ServerCommand(Helpers::format("kickid %d [NoCheatZ 4] %s\n", (*it)->userid, (*it)->kick_message).c_str());
			if(!Helpers::bStrEq("127.0.0.1", (*it)->ip))
			{
				CIFaceManager::GetInstance()->GetIengine()->ServerCommand(Helpers::format("addip 1440 \"%s\"\n", (*it)->ip).c_str());
				do_writeid = true;
			}
			CIFaceManager::GetInstance()->GetIengine()->ServerExecute();

			ClearRequests((*it)->steamid);
			goto retry;
		}
	}
}

void BanRequest::ClearRequests(const char * SteamID)
{
	for(std::list<PlayerBanRequestT*>::iterator it = m_requests.begin(); it != m_requests.end(); ++it)
	{
		if(Helpers::bStrEq((*it)->steamid, SteamID, 8))
		{
			m_requests.remove(*it);
		}
	}
}
