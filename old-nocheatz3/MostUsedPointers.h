//#include "edict.h"
//#include "nocheatz.h"
//#include "game/server/iplayerinfo.h"

#ifndef NULL
#define NULL 0 // C2065
#endif

class Cg_NCZ_Player;
struct edict_t;
class IPlayerInfo;

#ifndef C_MOSTUSEDPOINTERS
#define C_MOSTUSEDPOINTERS

// Class which avoid creating pointers multiple time in the same class.

class MostUsedPointers
{
protected:
	MostUsedPointers()
	{
		MyPlayer = NULL;
		MyInfo = NULL;
		MyEdict = NULL;
	};
	virtual ~MostUsedPointers(){};

protected:
	Cg_NCZ_Player *MyPlayer;
	IPlayerInfo *MyInfo;
	edict_t *MyEdict;
};

#endif