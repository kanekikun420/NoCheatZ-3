#include "Misc/temp_BaseDataStruct.h"
#include "NczPlayer.h"

#ifndef PLAYERDATASTRUCT
#define PLAYERDATASTRUCT

template <typename DataT>
class PlayerDataStructHandler :
	public BaseDataStructHandler<DataT, MAX_PLAYERS>
{
	typedef BaseDataStructHandler<DataT, MAX_PLAYERS> BaseClass;
public:
	PlayerDataStructHandler() : BaseClass()
	{
		memset(m_dataStruct, 0, sizeof(m_dataStruct));
	};
	~PlayerDataStructHandler(){};

	DataT* GetDefaultDataStruct()
	{
		return (DataT*)&(m_dataStruct[0]);
	};

	DataT* GetPlayerDataStruct(NczPlayer * player)
	{
		Assert(player);
		return (DataT*)(&m_dataStruct[player->GetIndex()]);
	};

	void ResetPlayerDataStruct(NczPlayer * player)
	{
		Assert(player);
		m_dataStruct[player->GetIndex()] = *GetDefaultDataStruct();
	};

#ifdef GNUC
	DataT m_dataStruct[MAX_PLAYERS];
#endif
};

#endif
