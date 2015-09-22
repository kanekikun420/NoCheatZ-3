#include "MRecipientFilter.h"

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
        edict_t *pPlayer = Helpers::PEntityOfEntIndex(i);
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
