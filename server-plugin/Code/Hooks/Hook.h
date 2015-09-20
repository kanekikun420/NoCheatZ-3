#ifndef HOOK_H
#define HOOK_H

#include "dbg.h"
#include "Players/NczPlayer.h"

#ifdef WIN32
#	include "Misc/include_windows_headers.h"
#	define HOOKFN_EXT __thiscall
#	define HOOKFN_INT __fastcall
#else
#	include <sys/mman.h>
#	include <unistd.h>
#	define HOOKFN_EXT
#	define HOOKFN_INT __attribute__((cdecl))
#endif

DWORD VirtualTableHook(DWORD* classptr, const int vtable, const DWORD newInterface );

class EmptyClass { };

#endif
