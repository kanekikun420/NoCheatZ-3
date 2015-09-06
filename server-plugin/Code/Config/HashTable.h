// nocheatz/hash

#include <vector>
#include <string>
#include "Misc/Helpers.h"
#include "Misc/Types.h"
#include "Misc/temp_Singleton.h"

#ifndef HASH_TABLE
#define HASH_TABLE

enum HashType
{
	COMMAND = 0,
	SERVER_FILE
};

enum ActionID
{
	UNKNOWN = -1,
	SAFE = 0,
	BLOCK,
	KICK,
	BAN,
};

struct HashInfo
{
	HashType type;
	hash_t hash;
	size_t len;
	ActionID action;
	char first_char;
};

class HashTable : public Singleton<HashTable>
{
public:
	HashTable();
	~HashTable(){};

	bool LoadHashTable();

	void UpdateHashTable(){};
	void GetHash(const char * data, hash_t* hash, size_t len = 0, size_t start = 0);

	ActionID ScanCommandString(const char * commandstring);
private:
	static const hash_t hash_key = 0x13377FFF;
	FILE* m_hashDataFile;
	std::vector<HashInfo> m_hashTable;
	size_t min_len;
	
};

#endif
