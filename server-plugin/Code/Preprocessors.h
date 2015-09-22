/*
	This file makes it easier to cross-compile between all the games and all the os they use.
	Because I use Makefile with Linux and VisualStudio Compiler with Windows, I think it is easier to have only one place to make sure everything works everywhere for anything ...
	It may be included everywhere.
*/

#ifndef NCZ_PREPROCESSORS
#define NCZ_PREPROCESSORS

#ifdef GNUC
#include <cstdlib>
#endif

//#define PIPELINE_ADDRESS "pipeline.nocheatz.com" // No more pipelining nor master server, the plugin is now independant.

/*
	Scripts that launch the compiler defines what game the plugin must be compiled for. (NCZ_CSS or NCZ_CSP or ...)
	This is where we make sure the plugin will be compatible.

	NCZ_VERSION_STR		Full string version
	GAMEDIR				Root directory name of the server
	ENGINE_DLIB_LINUX	Dynamic library name of the server if used with linux
	ENGINE_DLIB			Library name resulting of the OS we compile for
*/
#ifdef NCZ_CSS // Counter Strike : Source
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
#define ENGINE_HOLES
#endif

#ifdef WIN32
#define ENGINE_DLIB "engine.dll"
#else
#define ENGINE_DLIB ENGINE_DLIB_LINUX
#endif

#define NCZ_PLUGIN_NAME			"NoCheatZ" // Name of the plugin

/*
	Defines if we dynamically load the latest interfaces of the engine or if they are loaded statically.
	Comment this to go static.

	Dynamic = More chance to be loaded if the engine code changes, but more risk of heavy crashes if our code isn't compatible.
	Static = Won't load if any change is made to the source engine, even if this should work.
*/
#define IFACEMANAGER_AUTOINIT

#define IN_ATTACK (1 << 0)
#define IN_JUMP   (1 << 1)

/*
	Use for backward compatibility.
	EP1 (Older version of Source Engine) = Counter-Strike : Promod
*/
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

/*
	gcc is f*cking strict ...
*/
#ifdef GNUC
#include <cstring>
#define strcpy_s(a, b, c) strncpy(a, c, b)
#define nullptr 0
#endif

#define StrLen(x) strlen(x)
#define FStrEq(x,y) (Q_stricmp(x,y) == 0)


#endif
