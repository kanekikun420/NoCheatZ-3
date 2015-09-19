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
		memset(this->m_dataStruct, 0, sizeof(this->m_dataStruct));
	};
	~PlayerDataStructHandler(){};

	DataT* GetDefaultDataStruct()
	{
		return (DataT*)&(this->m_dataStruct[0]);
	};

	DataT* GetPlayerDataStruct(NczPlayer * player)
	{
		Assert(player);
		return (DataT*)(&(this->m_dataStruct[player->GetIndex()]));
	};

	void ResetPlayerDataStruct(NczPlayer * player)
	{
		Assert(player);
		this->m_dataStruct[player->GetIndex()] = *GetDefaultDataStruct();
	};
};

#endif
