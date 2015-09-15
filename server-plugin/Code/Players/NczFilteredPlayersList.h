//#include <mutex>
#include "NczPlayer.h"
#include "NczPlayerManager.h"

#ifndef NCZFILTEREDPLAYERLIST
#define NCZFILTEREDPLAYERLIST

/*
	Gives a list to Systems.
	May be used to iterate over all players in the filter specifyed
	in one frame.
*/
class NczFilteredPlayersList
{
public:
	NczFilteredPlayersList();
	virtual ~NczFilteredPlayersList(){};

	// Systems use this to know which playerlist to test
	virtual SlotStatus GetFilter() = 0;

	virtual NczPlayer* GetNextPlayer(); 
	virtual void ResetNextPlayer();

protected:
	PlayerHandler* m_nextPlayer;
};

/*
	Gives a list to Systems.
	May be used to iterate over all players in the filter specifyed
	in multiple frames.
	NOT THREAD SAFE.
*/
class AsyncNczFilteredPlayersList : public NczFilteredPlayersList
{
public:
	AsyncNczFilteredPlayersList() : NczFilteredPlayersList() {};
	virtual ~AsyncNczFilteredPlayersList(){};

	// Returns null only if no players are available
	virtual NczPlayer* GetNextPlayer();
};

#endif
