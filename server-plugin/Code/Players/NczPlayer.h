#include <string>
#include <list>

#include "inetchannelinfo.h"
#include "inetchannel.h"
#include "edict.h"

#ifdef GNUC
#undef min
#undef max
#endif

#include "Preprocessors.h"
#include "Misc/Helpers.h"
#include "Misc/IFaceManager.h"

class IPlayerInfo;
class CUserCmd;

#ifndef NCZPCLASS
#define NCZPCLASS

// 64 players + SourceTV + Server itself
#define MAX_PLAYERS 66

/* Permet de connaître l'état d'un slot du serveur rapidement */
enum SlotStatus
{
	INVALID = -1, // Slot not used
	BOT = 0,
	PLAYER_CONNECTED, // Spectator or dead or ...
	PLAYER_IN_TESTS, // Playing the round
};

enum WpnShotType
{
	HAND,
	AUTO,
	PISTOL
};

class NczPlayer
{
public:
	NczPlayer(const int index);
	~NczPlayer(){};

	const int GetIndex(){ return cIndex; }; 
	int GetUserid() const;
	edict_t * GetEdict();
	IPlayerInfo * GetPlayerInfo();
	INetChannelInfo* GetChannelInfo();
	const char * GetName();
	const char * GetSteamID();
	const char * GetIPAddress();
	WpnShotType GetWpnShotType();
	int aimingAt(); // Retourne index de la cible présente sur le viseur

	float GetTimeConnected();

	bool isValidEdict();

	void OnConnect();

	void Kick(const char * msg = "Kicked by NoCheatZ 4");
	void Ban(const char * msg = "Banned by NoCheatZ 4", int minutes = 0);

private:
	const int cIndex;
	int m_userid;
	edict_t * m_edict;
	INetChannelInfo* m_channelinfo;
	float m_time_connected;
};

#endif
