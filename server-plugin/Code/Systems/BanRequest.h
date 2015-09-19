#include <list>

#include "Preprocessors.h"
#include "Players/NczPlayer.h"
#include "Misc/temp_Singleton.h"

#ifndef BANREQUEST_H
#define BANREQUEST_H

typedef struct PlayerBanRequest
{
	int ban_time;
	int userid;
	double request_time;
	const char * kick_message;
	char player_name[24];
	char steamid[24];
	char ip[24];

	// FIXME : Ctors
} PlayerBanRequestT;

class BanRequest : public Singleton<BanRequest>
{
public:
	BanRequest() : m_wait_time(10.0), do_writeid(false) {};
	~BanRequest(){for(std::list<PlayerBanRequestT*>::iterator it = m_requests.begin(); it != m_requests.end(); ++it) delete *it;};

	bool NeedWriteID()
	{
		if(do_writeid)
		{
			do_writeid = false;
			return true;
		}
		return false;
	}

	void SetWaitTime(double wait_time);

	void AddAsyncBan(NczPlayer* player, int ban_time, const char * kick_message);

	void Think();

	void ClearRequests(const char * SteamID);

private:
	bool do_writeid;
	double m_wait_time;

	std::list<PlayerBanRequestT*> m_requests; // FIXME : Do not use pointers here
};

#endif
