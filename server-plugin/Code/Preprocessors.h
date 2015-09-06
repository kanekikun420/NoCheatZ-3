#ifdef GNUC
#include <cstdlib>
#endif

#ifndef NCZ_PREPROCESSORS
#define NCZ_PREPROCESSORS

#define PIPELINE_ADDRESS "pipeline.nocheatz.com"

/* Choisir quelle version compiler en utilisant les préprocesseurs du compilateur */
#ifdef NCZ_CSS
#define NCZ_VERSION_STR	" 4.0 Alpha CSS"
#define GAMEDIR "cstrike"
#define ENGINE_DLIB_LINUX "engine_srv.so"
#endif
#ifdef NCZ_CSP
#define NCZ_VERSION_STR	" 4.0 Alpha CSPROMOD"
#define GAMEDIR "cspromod"
#define ENGINE_DLIB_LINUX "engine_i486.so"
#endif
#ifdef NCZ_CSGO
#define NCZ_VERSION_STR	" 4.0 Alpha CSGO"
#define GAMEDIR "csgo"
#define ENGINE_DLIB_LINUX "engine_srv.so"
#endif

#ifdef WIN32
#define ENGINE_DLIB "engine.dll"
#else
#define ENGINE_DLIB ENGINE_DLIB_LINUX
#endif

#define NCZ_PLUGIN_NAME			"NoCheatZ"

#define IFACEMANAGER_AUTOINIT

#define IN_ATTACK (1 << 0)
#define IN_JUMP   (1 << 1)

#ifdef NCZ_EP1
#define GET_ARGV(a) engine->Cmd_Argv(a)
#define GET_ARGS engine->Cmd_Args()
#define GET_ARGC engine->Cmd_Argc()
#define FINDCOMMAND(a) g_pCVar->GetCommands()->FindCommand(a)
#define IsFlagSet IsBitSet
#else
#define GET_ARGV(a) args.Arg(a)
#define GET_ARGC args.ArgC()
#define GET_ARGS args.GetCommandString()
#define FINDCOMMAND(a) g_pCVar->FindCommand(a)
#endif

#ifdef GNUC
#include <cstring>
#define strcpy_s(a, b, c) strncpy(a, c, b)
#define nullptr 0
#endif

#endif
