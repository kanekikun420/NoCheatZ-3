#include "MRecipientFilter.h"
#include "nocheatz.h"
#include "interface.h"
#include "filesystem.h"
#include "engine/iserverplugin.h"
#ifndef NCZ_EP1
#include "game/server/iplayerinfo.h"
#else
#include "dlls/iplayerinfo.h"
#endif
#include "eiface.h"
#include "igameevents.h"
#include "convar.h"
#include "Color.h"
#include "Preprocessors.h"

extern IVEngineServer      *engine;
extern IPlayerInfoManager   *playerinfomanager;
extern IServerPluginHelpers   *helpers;

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

MRecipientFilter::MRecipientFilter(void)
{
}

MRecipientFilter::~MRecipientFilter(void)
{
}

int MRecipientFilter::GetRecipientCount() const
{
#ifdef NCZ_CSGO
    return m_Recipients.Count();
#else
	return m_Recipients.Size();
#endif
}

int MRecipientFilter::GetRecipientIndex(int slot) const
{
    if ( slot < 0 || slot >= GetRecipientCount() )
        return -1;

    return m_Recipients[ slot ];
}

bool MRecipientFilter::IsInitMessage() const
{
    return false;
}

bool MRecipientFilter::IsReliable() const
{
    return false;
}

void MRecipientFilter::AddAllPlayers(int maxClients)
{
    m_Recipients.RemoveAll();
    for ( int i = 1; i <= maxClients; i++ )
    {
        edict_t *pPlayer = PEntityOfEntIndex(i);
        if ( !pPlayer || pPlayer->IsFree())
            continue;
        //AddRecipient( pPlayer );
        m_Recipients.AddToTail(i);
    }
}
void MRecipientFilter::AddRecipient( int iPlayer )
{
    // Already in list
    if ( m_Recipients.Find( iPlayer ) != m_Recipients.InvalidIndex() )
        return;

    m_Recipients.AddToTail( iPlayer );
}
