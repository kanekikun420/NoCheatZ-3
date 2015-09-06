#include "dbg.h"

#ifndef BASEDATASTRUCT
#define BASEDATASTRUCT

template <typename DataT, int MAX_ELEM>
class BaseDataStructHandler
{
public:
	BaseDataStructHandler()
	{
	};
	~BaseDataStructHandler()
	{
	};

	void ResetAll(DataT* src)
	{
		if(src)
		{
			size_t index = 0;
			for(; index < MAX_ELEM; ++index)
				memcpy(&(m_dataStruct[index]), src, sizeof(DataT));
		}
		else
		{
			memset(m_dataStruct, 0, sizeof(m_dataStruct));
		}
	};

protected:
	DataT* GetDataStruct(int elem)
	{
		Assert(elem < MAX_ELEM && elem >= 0);
		return (DataT*)(&(m_dataStruct[elem]));
	};

	void ResetDataStruct(int elem)
	{
		memset(GetDataStruct(elem), 0, sizeof(DataT));
	};

protected:
	DataT m_dataStruct[MAX_ELEM];
};

#endif
