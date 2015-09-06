#include "NczFilteredPlayersList.h"

NczFilteredPlayersList::NczFilteredPlayersList()
{
	m_nextPlayer = nullptr;
}

NczPlayer* NczFilteredPlayersList::GetNextPlayer()
{
	// Met à jour le prochain pointeur en interne et retourne celui en cours
	NczPlayer* playerStor = m_nextPlayer->playerClass;
	if(playerStor == nullptr) return nullptr; // Liste vide

	for(int index = playerStor->GetIndex()+1; index < MAX_PLAYERS; ++index)
	{
		m_nextPlayer = NczPlayerManager::GetInstance()->GetPlayerHandlerByIndex(index);
		if(m_nextPlayer->status != GetFilter()) continue;
		break;
	}
	if(playerStor == m_nextPlayer->playerClass) return nullptr; // Fin de liste
	return playerStor;
}

void NczFilteredPlayersList::ResetNextPlayer()
{
	// Remet l'itération à zero
	m_nextPlayer = nullptr;
	PLAYERS_LOOP_RUNTIME
	{
		if(ph->status < GetFilter()) continue;
		m_nextPlayer = ph;
		break;
	}
	END_PLAYERS_LOOP
}

NczPlayer* AsyncNczFilteredPlayersList::GetNextPlayer()
{
	// On re-vérifie le pointeur actuel
	if(m_nextPlayer == nullptr) ResetNextPlayer();
	if(m_nextPlayer == nullptr) return nullptr;
	if(m_nextPlayer->status < GetFilter())
	{
		ResetNextPlayer();
		if(m_nextPlayer == nullptr) return nullptr;
	}
	if(m_nextPlayer->status == INVALID) return nullptr;

	NczPlayer* playerStor = m_nextPlayer->playerClass;
	
	int index = m_nextPlayer->playerClass->GetIndex();
	int loop_count = 0;
	while((m_nextPlayer = NczPlayerManager::GetInstance()->GetPlayerHandlerByIndex(++index)) == nullptr && (++loop_count) != MAX_PLAYERS)
	{
		index %= MAX_PLAYERS;
	}
	return playerStor;
}
