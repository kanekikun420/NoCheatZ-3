#include "Preprocessors.h"
#include <string>
#ifndef NCZ_EP1
#include "mathlib/vector.h"
#else
#include "vector.h"
#endif
#include "Core.h"
#include "igameevents.h"

class IPlayerInfo;
class INetChannelInfo;
class INetChannel;
class CUserCmd;
struct edict_t;

#ifndef NCZPCLASS
#define NCZPCLASS

#define MAX_PLAYERS 64

enum SlotStatus
{
	INVALID = -1,
	BOT = 0,
	PLAYER_OUT_OF_GAME,
	PLAYER_IN_GAME
};

struct PlayerHandler
{
	NczPlayer* playerClass;
	SlotStatus status;
};

class NczPlayerManager : public IGameEventListener2
{
public:
	NczPlayerManager();
	virtual ~NczPlayerManager();

	virtual PlayerHandler* GetPlayerHandler(int slot) const;

private:
	PlayerHandler FullPlayersList[MAX_PLAYERS];
};

class NczPlayer
{
public:
	NczPlayer(const int index);
	virtual ~NczPlayer();

	virtual int getIndex() const; 
	virtual int getUserid() const;
	virtual int getTimeConnected() const;
	virtual const char * getName() const;
	virtual const char * getSteamID() const;
	virtual const char * getIPAddress() const;
	virtual edict_t * getEdict() const;
	virtual IPlayerInfo * getPlayerInfo() const;
	virtual INetChannelInfo* getChannelInfo() const;
	virtual bool isValidEdict() const;

private:
	const int cIndex;
};
#endif