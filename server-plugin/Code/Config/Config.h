#include "Misc/temp_Singleton.h"

#ifndef CONFIG
#define CONFIG

typedef struct ServerConfig
{
	// General
	bool kick_ban;
	short max_chars_name;
	bool secure;
	bool print_metrics;

} ServerConfigT;

typedef struct VTable
{
	unsigned int Offset_playerRunCommand;
	unsigned int Offset_m_hGroundEntityChanged;
	unsigned int Offset_teleport;
	unsigned int Offset_setTransmit;
	unsigned int Offset_shouldTransmit;
	unsigned int Offset_updateTransmit;
} VTableT;

typedef struct RemoteConfig
{
	VTableT fnOffsets;
	ServerConfigT remoteServerConfig;
} RemoteConfigT;

class Config : public Singleton<Config>
{
public:
	Config();
	~Config(){};

	void LoadConfig();
	void LoadServerConfig();

	void UpdateVTable();
	void UpdateRemoteConfig();

	ServerConfig* GetConfigData() {return (ServerConfig*)(&m_configData);};
	ServerConfig* GetRemoteConfigData() {return (ServerConfig*)(&m_remoteConfig.remoteServerConfig);};
	VTable* GetVTable() {return (VTable*)(&(m_remoteConfig.fnOffsets));};

private:
	ServerConfigT m_configData;
	RemoteConfigT m_remoteConfig;
};

#endif // CONFIG
