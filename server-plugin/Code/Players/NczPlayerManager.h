#include "igameevents.h"
#include "Misc/temp_Singleton.h"
#include "Systems/Logger.h"
#include "NczPlayer.h"

#ifndef NCZPLAYERMANAGER
#define NCZPLAYERMANAGER

struct PlayerHandler
{
	NczPlayer* playerClass;
	SlotStatus status;
};

class CCSPlayer;

/* Distribue et met à jour l'état des slots du serveur */
class NczPlayerManager : public IGameEventListener2, public Singleton<NczPlayerManager>
{
public:
	NczPlayerManager();
	~NczPlayerManager();

	/* Force la mise à jour des slots en scannant la mémoire pour EdictList
	   S'inscrit aux événements pour mettre à jour les slots en temps réel */
	void LoadPlayerManager();

	/* Get player by filter. If filter is INVALID, we don't filter */
	PlayerHandler* GetPlayerHandlerByIndex(int slot, SlotStatus filter = INVALID);
	PlayerHandler* GetPlayerHandlerByUserId(int userid, SlotStatus filter = INVALID);
	PlayerHandler* GetPlayerHandlerByBasePlayer(void* BasePlayer, SlotStatus filter = INVALID);
	PlayerHandler* GetPlayerHandlerBySteamID(const char * steamid, SlotStatus filter = INVALID);
	PlayerHandler* GetPlayerHandlerByEdict(edict_t * pEdict, SlotStatus filter = INVALID);
	PlayerHandler* GetPlayerHandlerByName(const char * playerName, SlotStatus filter = INVALID);
	
	short GetPlayerCount(SlotStatus filter = INVALID);

	void ClientConnect(edict_t* pEntity); // Bots don't call this ...
	void ClientActive(edict_t* pEntity); // ... they call this at first
	void ClientDisconnect(edict_t *pEntity);
	void FireGameEvent(IGameEvent* ev);

private:
	PlayerHandler FullHandlersList[MAX_PLAYERS+1];
	double lastSpawnTime[MAX_PLAYERS];
};

/* Utilisé en interne pour initialiser le tableau, des petites fonctions
  Ajout d'une case supplémentaire à FullHandlersList pour pouvoir quitter proprement la boucle PLAYERS_LOOP_RUNTIME */
#define _PLAYERS_LOOP_INIT { \
		int x = 0; \
		PlayerHandler* ph = &(FullHandlersList[x]); \
		for(; x < MAX_PLAYERS+1; ++x, ph = &(FullHandlersList[x]))

/* Boucle classique pour les utilisations externes
   Donne la variable ph et x dans la boucle */
#define PLAYERS_LOOP_RUNTIME { \
		int x = 1; \
		PlayerHandler* ph = NczPlayerManager::GetInstance()->GetPlayerHandlerByIndex(x); \
		for(; ph->status != INVALID; ++x, ph = NczPlayerManager::GetInstance()->GetPlayerHandlerByIndex(x))

#define END_PLAYERS_LOOP }

#endif
