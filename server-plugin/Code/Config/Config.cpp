#include "Config.h"

Config::Config()
{
	m_configData.kick_ban = true;
	m_configData.max_chars_name = 24;
	m_configData.secure = false;

#ifdef WIN32
	m_remoteConfig.fnOffsets.Offset_m_hGroundEntityChanged = 177;
	m_remoteConfig.fnOffsets.Offset_playerRunCommand = 418;
	m_remoteConfig.fnOffsets.Offset_teleport = 112;
#else
	m_remoteConfig.fnOffsets.Offset_m_hGroundEntityChanged = 179;
	m_remoteConfig.fnOffsets.Offset_playerRunCommand = 419;
	m_remoteConfig.fnOffsets.Offset_teleport = 113;
#endif

	m_remoteConfig.remoteServerConfig.kick_ban = false;
}