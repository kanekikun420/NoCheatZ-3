#ifndef NCZ_GLOBALS
#define NCZ_GLOBALS

bool                       check_client_vars;
bool                       check_aimbots;
bool                       aimbots_extra_filter;
bool                       check_bunny;
bool                       check_triggerbot;
bool                       report_cheats;
bool                       kick_ban;
bool                       auto_dem;
bool                       use_banlists;
bool                       use_ncz_banlists;
bool                       recording;
bool                       ncz_active;
bool                       cvarCheckLaunched;
//bool                       web_thread_killed        = false;
bool                       report_ukcmd;
bool                       test_config;
bool                       registered;
//bool                       unregister_asked         = false;
//bool                       needtokill               = false;
bool                       ForceConsistency;
bool                       NoTell_active;
//bool                       isRoundPlaying           = false;
bool                       must_thread_be_killed;
bool                       UseNet;
bool                       FirstGameFrame;
bool                       teleport_lock;
bool                       lockServer;
bool                       requireRestart;
//bool                       allow_status             =  true;

//int                        vax_offset               =     0;
//int                        vay_offset               =     0;
//int                        ground_offset            =     0;
int                        player_count;
int                        t_tickrate; 
int                        tickrate;
//int                        maxplayers               =     0;
int                        frameStage;
int                        frameStage_PVAC;

int               demoticks;
//int               n_tick                   =     0;

float                      nextSecond;
//float                      lastTickrateCheck        =   0.0;
//float                      nextTickrateCheck        =   0.0;
float                      UkCmdCheck;
float                      nextstopcheck;
//float                      next_cvar_check          =  0.0f;
//float                      next_cvar_check_timer    = 20.0f;
//float                      roundPlayingCheck        =  0.0f;
float                      lastNetOff;
int                        disconnectNumber;

std::string                recordFilename;
std::string                gb_admin_id;
std::string                gb_reason_id;
std::string                our_ip;

Cg_NCZ_Queue*              queue_send_report;
Cg_NCZ_Queue*              banlists;
Cg_NCZ_Queue*              disconnectingPlayers;
Cg_NCZ_Queue*              serverMessages;
//Cg_NCZ_Queue*              player_conf              =  NULL;

CGlobalVars*               gpGlobals;

std::map<const char *, const char *>   g_cvars;

std::map<std::string, std::string> g_translate;

edict_t *                  PlayerList;

IVEngineServer *           engine;
IGameEventManager2 *       gameeventmanager;
IPlayerInfoManager *       playerinfomanager;
IServerPluginHelpers *     helpers;
IServerGameDLL *           gamedll;
IServerGameEnts *          gameents;
IServerGameClients *       gameclients;
EntityPropsManager *       entpropsmanager;
IEngineTrace *             enginetrace;

NczThread *TheWebThread;
NczThread *UpdateThread;

#endif