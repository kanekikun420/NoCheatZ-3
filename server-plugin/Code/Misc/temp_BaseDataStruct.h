#include "dbg.h"

#ifndef BASEDATASTRUCT
#define BASEDATASTRUCT

template <typename DataT, int MAX_ELEM>
class BaseDataStructHandler
{
public:
	BaseDataStructHandler()
	{
		for(int x = 0; x < MAX_ELEM; ++x)
			m_dataStruct[x] = DataT();
	};
	~BaseDataStructHandler()
	{
	};

	void ResetAll(const DataT* src)
	{
		if(src)
		{
			for(size_t index = 0; index < MAX_ELEM; ++index)
				m_dataStruct[index] = *src;
		}
		else
		{
			for(int x = 0; x < MAX_ELEM; ++x)
			{
				m_dataStruct[x] = DataT();
			}
		}
	};

protected:
	const DataT& GetDataStruct(const int elem) const
	{
		return m_dataStruct[elem];
	};

	void ResetDataStruct(const int elem)
	{
		m_dataStruct[elem] = DataT();
	};

protected:
	DataT m_dataStruct[MAX_ELEM];
};

#endif
