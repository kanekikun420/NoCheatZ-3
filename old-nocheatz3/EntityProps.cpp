#include <string>
#include <vector>
#include "EntityProps.h"
#include "nocheatz.h"
#include "eiface.h"
#include "edict.h"

#ifdef NCZ_CSS

extern IServerGameDLL* gamedll;

#undef GetProp // If you are using a lib that make GetProp be GetPropA ...

EntityPropsManager::EntityPropsManager()
{
}

EntityPropsManager::~EntityPropsManager()
{
}


int EntityPropsManager::getIndexInList(const std::string &path)
{
	const int vsize = EntityPropsList.size();
	if(!vsize == 0)
	{
		for (int index = 0; index < vsize; index ++)
		{
			if(EntityPropsList[index].propPath == path)
			{
				return index;
			}
		}
	}
	return -1;
}

bool EntityPropsManager::isPropAlreadyRegistered(const std::string &path)
{
	return (this->getIndexInList(path) > -1);
}

void EntityPropsManager::registerEntityProp(const std::string &path)
{
	if(!isPropAlreadyRegistered(path))
	{
		int offset = this->getPropOffset(path);
		if(offset)
		{
			struct EntityProps entity;
			entity.propOffset = offset;
			entity.propPath = path;
			EntityPropsList.push_back(entity);
		}
	}
	/*else // FIXME : Update offset if it's not the same. Becarfull with the return value of getIndexInList(path).
	{
		if(!(this->getPropOffset(path) == EntityPropsList.at(this->getIndexInList(path)).propOffset))
		{
			int offset = this->getPropOffset(path);
			if(offset)
			{
				struct EntityProps entity;
				entity.propOffset = offset;
				entity.propPath = path;
				EntityPropsList.push_back(entity);
			}
		}
	}*/
}

int EntityPropsManager::getPropOffset(const std::string &path)
{
	// Try to find if we have already this offset
	int iOffset = 0;
	int index = this->getIndexInList(path);
	if(index > -1)
	{
		iOffset = EntityPropsList.at(index).propOffset;
	}
	if(iOffset)
	{
		return iOffset;
	}
	// If not, we have to find it.

	iOffset = 0;
	int i = 0;
	std::string cpath;
	std::vector<std::string> props;

	strSplit(path, ".", &props);

	ServerClass *pAllClasses = gamedll->GetAllServerClasses();
	while(pAllClasses)
	{
		if(pAllClasses->GetName() == props.at(0)) // If we found the class
		{
			const int pSize = props.size(); // The use of const will accelerate the for instruction
			if(pSize > 2) // path containing class.datatable.(...).prop
			{
				SendTable *lastTable = pAllClasses->m_pTable;
				int iProps = 0;
				for(int iPath = 1; iPath < pSize-1; iPath++) // get the last datatable in path
				{
					cpath = props.at(iPath);
					iProps = lastTable->GetNumProps();
					for(i = 0; i < iProps; i++)
					{
						if(lastTable->GetProp(i)->GetName() == props.at(iPath))
						{
							lastTable = lastTable->GetProp(i)->GetDataTable();
							i = iProps;
						}
					}
				}
				iProps = lastTable->m_nProps;
				for(i = 0; i < iProps; i++) // get the prop offset
				{
					if(lastTable->GetProp(i)->GetName() == props.back())
					{
						iOffset = lastTable->GetProp(i)->GetOffset();
						if(iOffset < 0) // The offset must be not negative
						{
							iOffset *= -1;
						}
						return iOffset;
					}
				}
			}
			else // path containing only class.prop
			{
				const int iProps = pAllClasses->m_pTable->GetNumProps();
				for(i = 0; i < iProps; i++)
				{
					//Msg(pAllClasses->m_pTable->GetProp(i)->GetName());
					//Msg("\n");
					if(pAllClasses->m_pTable->GetProp(i)->GetName() == props.back())
					{
						iOffset = pAllClasses->m_pTable->GetProp(i)->GetOffset();
						if(iOffset < 0) // The offset must be not negative
						{
							iOffset *= -1;
						}
						return iOffset;
					}
				}
				break;
			}
		} // End if(pAllClasses->GetName() == props.at(0))
		pAllClasses = pAllClasses->m_pNext;
	} // End while(!pAllClasses)
	return 0;
}

#endif
