//#include <mutex>
#include "NczPlayer.h"
#include "NczPlayerManager.h"

#ifndef NCZFILTEREDPLAYERLIST
#define NCZFILTEREDPLAYERLIST

/* Donne une simple liste de joueurs aux testeurs pour faciliter les boucles */
class NczFilteredPlayersList
{
public:
	NczFilteredPlayersList();
	virtual ~NczFilteredPlayersList(){};

	virtual SlotStatus GetFilter() = 0;

	virtual NczPlayer* GetNextPlayer(); 
	virtual void ResetNextPlayer();

protected:
	PlayerHandler* m_nextPlayer;
};

/* Donne une liste infinie de joueurs pour le travail asynchrone (convars) */
class AsyncNczFilteredPlayersList : public NczFilteredPlayersList
{
public:
	AsyncNczFilteredPlayersList() : NczFilteredPlayersList() {};
	virtual ~AsyncNczFilteredPlayersList(){};

	// Returns null only if no players are available
	virtual NczPlayer* GetNextPlayer();
};

#endif
