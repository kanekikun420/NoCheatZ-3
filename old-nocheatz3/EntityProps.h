#include <string>
#include <vector>
#include "edict.h"
#include "eiface.h"
#include "nocheatz.h"

#ifndef EntityPropsManagerCLASS
#define EntityPropsManagerCLASS

struct EntityProps
{
	std::string propPath; // Unique identifier
	int propOffset; // Offset of prop
};

class EntityPropsManager
{
public:
	// Inits

	EntityPropsManager();
	~EntityPropsManager();

	// Methods

	bool isPropAlreadyRegistered(const std::string &path);

	template<class T>
	T & getPropValue(const std::string &path, edict_t *pPlayer)
	{
		int offset = this->getPropOffset(path);
		CBaseEntity *pBase = pPlayer->GetUnknown()->GetBaseEntity();
		T * prop = NULL;

		if(!offset) // Try to find the offset if it's NULL
		{
			this->registerEntityProp(path);
			offset = this->getPropOffset(path);
			if(!offset)
			{
				return *prop;
			}
		}

		prop = reinterpret_cast<T *>(reinterpret_cast<char *>(pBase) + offset);
		return *prop;
	}

	// Functions

	void registerEntityProp(const std::string &path);

private:
	// Methods

	int getPropOffset(const std::string &path);

	int getIndexInList(const std::string &path);

	// Vars

	std::vector<struct EntityProps> EntityPropsList;

};

#endif
