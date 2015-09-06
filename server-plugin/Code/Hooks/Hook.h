#include "dbg.h"
#include "Config/Config.h"
#include "Players/NczPlayer.h"

#ifndef HOOK_H
#define HOOK_H

#ifdef WIN32

#include "Misc/include_windows_headers.h"
#define HOOKFN_EXT __thiscall
#define HOOKFN_INT __fastcall

#else

#include <sys/mman.h>
#include <unistd.h>
#define HOOKFN_EXT
#define HOOKFN_INT __attribute__((cdecl))

#endif

DWORD VirtualTableHook(DWORD* classptr, int vtable, DWORD newInterface );

void HookBasePlayer(NczPlayer* player);

class EmptyClass { };

#endif
