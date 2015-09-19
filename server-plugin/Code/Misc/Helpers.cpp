#include "Helpers.h"

namespace Helpers
{
	edict_t * m_EdictList = nullptr;
	int  m_edictCount = 0;
	int  m_clientMax = 0;

	void toLowerCase(std::string &str)
	{
		std::locale loc;
		const int length = str.length();
		for(int i=0; i < length; ++i)
		{
			str[i] = std::tolower(str[i], loc);
		}
	}

	void xMsg(const char *fmt, ...)
	{
		va_list		argptr;
		static char		string[4096];

		va_start(argptr, fmt);
		vsnprintf(string, sizeof(string), fmt,argptr);
		va_end (argptr);

		std::string finalString = tostring("[" NCZ_PLUGIN_NAME "] ");
		std::string firstString = tostring(string);
		finalString += firstString;

		CIFaceManager::GetInstance()->GetIengine()->LogPrint(finalString.c_str());

	#ifdef WIN32
		firstString.pop_back();
	#else
		firstString = firstString.substr(0, firstString.length()-1);
	#endif
		writeToLogfile(getStrDateTime("%x %X") + " : " + firstString);	
	}

	std::string getStrGameDir()
	{
		char szGamedir[128];
		CIFaceManager::GetInstance()->GetIengine()->GetGameDir(szGamedir, 128);
		return tostring(szGamedir);
	}

	void writeToLogfile(const std::string &text)
	{
		if(CIFaceManager::GetInstance()->GetIengine())
		{
			std::string path = getStrGameDir() + "/logs/NoCheatZ_4_Logs/" + getStrDateTime("NoCheatZ-%d-%b-%Y") + ".log";
			std::ofstream fichier;
			fichier.open(path.c_str(), std::ios::out | std::ios::app);
			if(fichier)
			{
				fichier << text << std::endl;
				//fichier.close();
			}
			else
			{
				Msg("[NoCheatZ 4] Can't write to logfile ...\n");
			}
		}
	}

	std::string getStrDateTime(const char *format)
	{
		time_t rawtime;
		struct tm * timeinfo;
		char date[256];
		time(&rawtime);
		timeinfo = localtime(&rawtime);
		strftime(date, sizeof(date), format, timeinfo);
		date[255] = '\0';
		return tostring(date);
	}

	bool bStrEq(const char *sz1, const char *sz2, size_t start_offset, size_t length)
	{
		Assert(sz1);
		Assert(sz2);
		size_t len = strlen(sz1);
		if(len != strlen(sz2)) return false;


		for(size_t x = start_offset, c = 0; x < len && c < length; ++x, ++c)
			if(sz1[x] != sz2[x]) return false;

		return true;
	}

	bool bStriEq(const char *sz1, const char *sz2, size_t start_offset, size_t length)
	{
		Assert(sz1);
		Assert(sz2);
		size_t len = strlen(sz1);
		if(len != strlen(sz2)) return false;

		char t1, t2;
		for(size_t x = start_offset, c = 0; x < len && c < length; ++x, ++c)
		{
			t1 = sz1[x];
			t2 = sz2[x];
			if(sz1[x] <= 'Z' && sz1[x] >= 'A')
				t1 = sz1[x] + 0x20;
			if(sz2[x] <= 'Z' && sz2[x] >= 'A')
				t2 = sz2[x] + 0x20;
			if(t1 != t2) return false;
		}

		return true;
	}

	bool bBytesEq(const char *sz1, const char *sz2, size_t start_offset, size_t length)
	{
		Assert(sz1);
		Assert(sz2);

		for(size_t x = start_offset, c = 0; c < length; ++x, ++c)
			if(sz1[x] != sz2[c]) return false;

		return true;
	}

	void split(const std::string &s, char delim, std::vector<std::string> &elems)
	{
		std::stringstream ss(s);
		std::string item;
		while (std::getline(ss, item, delim)) {
			elems.push_back(item);
		}
	}

	// At this point, steamid is from a valid human player.
	edict_t * getEdictFromSteamID(const char *SteamID)
	{
		Assert(!bStrEq(SteamID, "STEAM_ID_PENDING"));
		Assert(!bStrEq(SteamID, "STEAM_ID_LAN"));
		Assert(!bStrEq(SteamID, "BOT"));
		const int imax = m_clientMax;
		for(int i = 1; i <= imax; i++)
		{
			//edict_t* pEntity = PEntityOfEntIndex(i);
			edict_t* pEntity = CIFaceManager::GetInstance()->GetIengine()->PEntityOfEntIndex(i);
			// FIXME : Size of steamid is no more 8 digits.
			if(bStrEq(CIFaceManager::GetInstance()->GetIengine()->GetPlayerNetworkIDString(pEntity), SteamID, 8))
			{
				return pEntity;
			}
		}
		return nullptr;
	}

	// At this point, steamid if from a valid human player.
	int getIndexFromSteamID(const char *SteamID)
	{
		Assert(!bStrEq(SteamID, "STEAM_ID_PENDING"));
		Assert(!bStrEq(SteamID, "STEAM_ID_LAN"));
		Assert(!bStrEq(SteamID, "BOT"));
		//return 
		return CIFaceManager::GetInstance()->GetIengine()->IndexOfEdict(getEdictFromSteamID(SteamID));
	}

	/*
	Use engineserver interface instead

	edict_t * PEntityOfEntIndex(const int iEntIndex)
	{
		Assert(m_EdictList);
		return (edict_t *)(m_EdictList + iEntIndex);
	}*/

	const unsigned char * FastScan_Internal(SigInst instructions)
	{
		ptr pAddr = CIFaceManager::GetInstance()->GetIFace(instructions.base_mod);
		if(!pAddr) return 0;
		const unsigned char * base_addr = nullptr;
		size_t base_len = 0;
	
#ifdef WIN32
		MEMORY_BASIC_INFORMATION mem;
		if(!VirtualQuery(pAddr, &mem, sizeof(mem))) return 0;

		base_addr = (const unsigned char *)mem.AllocationBase;
		IMAGE_DOS_HEADER *dos = (IMAGE_DOS_HEADER*)mem.AllocationBase;
		IMAGE_NT_HEADERS *pe = (IMAGE_NT_HEADERS*)((unsigned long)dos+(unsigned long)dos->e_lfanew);
		if(pe->Signature != IMAGE_NT_SIGNATURE) return 0;
		base_len = (size_t)pe->OptionalHeader.SizeOfImage;

#else
		Dl_info info;
		struct stat buf;

		if(!dladdr(pAddr, &info)) return 0;
		if(!info.dli_fbase || !info.dli_fname) return 0;
		if(stat(info.dli_fname, &buf) != 0) return 0;

		base_addr = (unsigned char *)info.dli_fbase;
		base_len = buf.st_size;
#endif

		const unsigned char * cur_addr = base_addr;
		const unsigned char * occurance_addr = cur_addr;
		unsigned int sig_pos = 0;
		bool in_occurance = false;
		for(; (DWORD)cur_addr < (DWORD)base_addr + (DWORD)base_len; ++cur_addr)
		{
			if(instructions.mask[sig_pos] == 'x')
			{
				if(instructions.sig[sig_pos] == *(unsigned char *)(cur_addr))
				{
					in_occurance = true;
					if(sig_pos == 0) occurance_addr = cur_addr;
					++sig_pos;
				}
				else if(in_occurance)
				{
					in_occurance = false;
					cur_addr = occurance_addr;
					sig_pos = 0;
				}
			}
			else if(instructions.mask[sig_pos] == '?' && in_occurance)
			{
				++sig_pos;
			}
			if(sig_pos == instructions.sig_len)
			{
				occurance_addr += instructions.final_address_offset;
				break;
			}
		}
		if(!in_occurance)
		{
			printf("Sigscan failed.\n");
			return 0;
		}
		else
		{
			return occurance_addr;
		}
	}

	void FastScan_EntList()
	{
		if(m_EdictList) return;
		SigInst EntSig;
		EntSig.base_mod = EngineServer;
#ifdef WIN32
		EntSig.sig = (unsigned char *)"\x55\x8B\xEC\x56\x8B\x75\x08\x3B\x35\x10\x11\x5E\x10\x72\x0E\x56\x68\x30\x2D\x33"
									  "\x10\xE8\xC6\xEF\x01\x00\x83\xC4\x08\x8B\x0D\x14\x11\x5E\x10\x8D\x04\xB6\x8D\x04"
									  "\x81\x5E\x5D\xC3";
		EntSig.mask =			"xxxxxxxx???????x????"
								"?x????xxxxx????xxxxx"
								"xxxx";
		EntSig.final_address_offset = 31;
		EntSig.sig_len = 44;
#else
		EntSig.sig = (unsigned char *)"\x55\x89\xE5\x53\x83\xEC\x14\x8B\x5D\x08\x3B\x1D\x00\xFC\x2F\x00\x72\x10\x89\x5C"
									  "\x24\x04\xC7\x04\x24\x4A\x91\x22\x00\xE8\x4E\x4E\x04\x00\x8B\x15\x04\xFC\x2F\x00"
									  "\x8D\x04\x9B\x83\xC4\x14\x5B\x8D\x04\x82\x5D\xC3";
		EntSig.mask =			"xxxxxxxxxxx???????x?"
								"??xx?????x????xx????"
								"xxxxxxxxxxxx";
		EntSig.final_address_offset = 36;
		EntSig.sig_len = 52;
#endif
		edict_t * *const *const addr = (edict_t * *const *const)FastScan_Internal(EntSig);
		if(!addr) return;
		m_EdictList = **addr;
		m_edictCount = *(int*)((*addr)-0x4);
	}

	/*
	Use engineserver interface instead

	int IndexOfEdict(const edict_t *pEdict)
	{
		Assert(pEdict);
		Assert(m_EdictList);
		return (int)(pEdict - m_EdictList);
	}
	*/

	int GetPlayerCount()
	{
		int count = 0;
		for(int index = 1; index <= 64; ++index)
		{
			edict_t * pEdict = CIFaceManager::GetInstance()->GetIengine()->PEntityOfEntIndex(index);
			if(!isValidEdict(pEdict)) continue;
			const char * steamid = CIFaceManager::GetInstance()->GetIengine()->GetPlayerNetworkIDString(pEdict);
			if(!steamid) continue;
			if(steamid[0] == 'B') continue;
			++count;
		}
		return count;
	}

	std::string format(const char *fmt, ...)
	{
		va_list		argptr;
		static char		string[4096];

		va_start(argptr, fmt);
		Q_vsnprintf(string, sizeof(string), fmt,argptr);
		va_end (argptr);

		return tostring(string);
	}

	bool isValidEdict(const edict_t * entity)
	{
		return entity != nullptr && !entity->IsFree();
	}

	template<typename T>
	std::string tostring(const T & toConvert)
	{
		// FIXME : Use locale
		std::stringstream convertion;
		convertion << toConvert;
		return convertion.str();
	}

	float fabs(const float n)
	{
		float x = n;
		if(x < 0)
		{
			x = x * -1.0;
		}
		return x;
	}

	int abs(const int value)
	{
		//if((value>>31) & 1) return (~value)+1; Unsafe
		if(value < 0) return value*-1;
		return value;
	}

	bool isOdd(const int value)
	{
		if(value & 1) return true;
		return false;
	}

	int getIndexFromUserID(const int userid)
	{
		for(int i = 0; i <= CIFaceManager::GetInstance()->GetIengine()->GetEntityCount(); i++)
		{
			if(CIFaceManager::GetInstance()->GetIengine()->GetPlayerUserId(m_EdictList+i) == userid)
			{
				return i;
			}
		}
		return -1;
	}

	bool IsInt(const double value)
	{
		double n;
		if(std::modf(value, &n) == 0.0f) return true;
		return false;
	}
}

void Helpers::tell(edict_t *pEntity, const std::string& message)
{
	Assert(isValidEdict(pEntity));
	IPlayerInfo *player =  CIFaceManager::GetInstance()->GetIplayers()->GetPlayerInfo(pEntity);
	if (player)
	{
		if (player->IsConnected())
		{
			MRecipientFilter filter;
			filter.AddRecipient(CIFaceManager::GetInstance()->GetIengine()->IndexOfEdict(pEntity));
			bf_write *pBuffer = CIFaceManager::GetInstance()->GetIengine()->UserMessageBegin( &filter, 3 );
			pBuffer->WriteByte( 0 );
			pBuffer->WriteString(message.c_str());
		}
	}
}

void Helpers::noTell(const edict_t *pEntity, const std::string& msg)
{
	MRecipientFilter filter;
	bf_write *pBuffer = CIFaceManager::GetInstance()->GetIengine()->UserMessageBegin( &filter, 3 );
	for (int i=1; i <= CIFaceManager::GetInstance()->GetGlobals()->maxClients; i++)
	{
		IPlayerInfo *player = CIFaceManager::GetInstance()->GetIplayers()->GetPlayerInfo(CIFaceManager::GetInstance()->GetIengine()->PEntityOfEntIndex(i));
		if(CIFaceManager::GetInstance()->GetIengine()->PEntityOfEntIndex(i) == pEntity) continue;

		if (player)
		{
			if (player->IsConnected())
			{
				filter.AddRecipient(i);
			}
		}
	}
	pBuffer->WriteByte( 0 );
	pBuffer->WriteString(msg.c_str());
	CIFaceManager::GetInstance()->GetIengine()->MessageEnd();
}

void Helpers::chatmsg(const std::string& msg)
{
	MRecipientFilter filter;
	bf_write *pBuffer = CIFaceManager::GetInstance()->GetIengine()->UserMessageBegin( &filter, 3 );
	for (int i=1; i <= CIFaceManager::GetInstance()->GetGlobals()->maxClients; i++)
	{
		IPlayerInfo *player = CIFaceManager::GetInstance()->GetIplayers()->GetPlayerInfo(CIFaceManager::GetInstance()->GetIengine()->PEntityOfEntIndex(i));

		if (player)
		{
			if (player->IsConnected())
			{
				filter.AddRecipient(i);
			}
		}
	}
	pBuffer->WriteByte( 0 );
	pBuffer->WriteString(msg.c_str());
	CIFaceManager::GetInstance()->GetIengine()->MessageEnd();
}
