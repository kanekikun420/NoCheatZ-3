#include "HashTable.h"
#include <errno.h>

HashTable::HashTable()
{
	m_hashTable.clear();
	m_hashDataFile = nullptr;
	min_len = 0xFFFFFFFF;
}

bool HashTable::LoadHashTable()
{
#ifdef WIN32
	std::string pathToFile = Helpers::getStrGameDir() + "\\addons\\NoCheatZ\\hash.dat";
#else
	std::string pathToFile = Helpers::getStrGameDir() + "/addons/NoCheatZ/hash.dat";
#endif
	m_hashDataFile = fopen(pathToFile.c_str(), "rb");

	if(!m_hashDataFile)
	{
		Warning("[NoCheatZ 4] %f - HashTable::LoadHashTable() - Unable to read %s : %s\n", Plat_FloatTime(), pathToFile.c_str(), strerror(errno));
		return false;
	}

#ifdef DEBUG
	int id1 = 0, id2 = 0;
#endif

	while(!feof(m_hashDataFile))
	{
		HashInfo info;
		fread(&info.type, sizeof(HashType), 1, m_hashDataFile);
		fread(&info.hash, sizeof(hash_t), 1, m_hashDataFile);
		fread(&info.len, sizeof(size_t), 1, m_hashDataFile);
		fread(&info.action, sizeof(ActionID), 1, m_hashDataFile);
		fread(&info.first_char, sizeof(char), 1, m_hashDataFile);
		if(info.len < min_len && info.type == COMMAND) min_len = info.len;
#ifdef DEBUG
		for(std::vector<HashInfo>::iterator it = m_hashTable.begin(); it != m_hashTable.end(); ++it)
		{
			if((*it).hash == info.hash && (*it).action != info.action)
			{
				Assert(id1 == id2);
			}
			++id2;
		}
		id2 = 0;
		++id1;
#endif
		m_hashTable.push_back(info);
	}

	fclose(m_hashDataFile);
	return true;
}

void HashTable::GetHash(const char * data, hash_t* hash, size_t len, size_t start)
{
	*hash = hash_key;
	int_8b lchar;
	hash_t chartohash, indextohash;
	for(size_t x = start, c = 0; data[x] != '\0' && c < len; ++x, ++c)
	{
		for(int_8b y = 0; y < sizeof(hash_t); y += sizeof(int_8b))
		{
			if(data[x] < 'a')
			{
				lchar = (((data[x] + ('a' - 'A')) % ('z' - 'a')) + 'a');
			}
			else
			{
				lchar = ((data[x] % ('z' - 'a')) + 'a');
			}
			chartohash = (lchar << (y*8));
			indextohash = (x << (y*8));
			*hash ^= chartohash & ~indextohash;
		}
	}
}

ActionID HashTable::ScanCommandString(const char * commandstring)
{
	size_t string_len = Helpers::StrLen(commandstring);
	if(string_len < min_len) return UNKNOWN;

	ActionID best_action = UNKNOWN;

	for(size_t x = 0; x < string_len; ++x)
	{
		size_t len_left = string_len - x;
		hash_t curHash = hash_key;
		std::locale loc;
		char tc = std::tolower(commandstring[x], loc);

		for(std::vector<HashInfo>::iterator it = m_hashTable.begin(); it != m_hashTable.end(); ++it)
		{
			if((*it).type != COMMAND) continue;

			if((*it).first_char == tc && len_left >= (*it).len)
			{
				GetHash(commandstring, &curHash, (*it).len, x);
				if(curHash == (*it).hash)
				{
					if((*it).action > best_action) best_action = (*it).action;
				}
			}
		}
	}
	return best_action;
}
