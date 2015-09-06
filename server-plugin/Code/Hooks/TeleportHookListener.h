#include <list>

#include "Hook.h"
#include "Players/NczPlayerManager.h"

#ifndef TELEPORTHOOKLISTENER
#define TELEPORTHOOKLISTENER

/////////////////////////////////////////////////////////////////////////
// CBaseFlex::Teleport(Vector const*, QAngle const*, Vector const*)
/////////////////////////////////////////////////////////////////////////

class CBaseFlex;

typedef void (HOOKFN_EXT *Teleport_t)(CBaseFlex *, Vector const*, QAngle const*, Vector const*);

class TeleportHookListener
{
public:
	TeleportHookListener();
	~TeleportHookListener();

	static void HookTeleport(NczPlayer* player);
	static void UnhookTeleport();

protected:
	static void RegisterTeleportHookListener(TeleportHookListener* listener);
	static void RemoveTeleportHookListener(TeleportHookListener* listener);
	
	virtual void TeleportCallback(NczPlayer* player, Vector const*, QAngle const*, Vector const*) = 0;

private:
#ifdef GNUC
	static void HOOKFN_INT nTeleport(CBaseFlex * basePlayer, Vector const* va, QAngle const* qa, Vector const* vb);
#else
	static void HOOKFN_INT nTeleport(CBaseFlex * basePlayer, void *, Vector const* va, QAngle const* qa, Vector const* vb);
#endif
	
	static std::list<TeleportHookListener*> m_listeners;
	static Teleport_t gpOldTeleportFn;
	static DWORD* pdwInterface;
};

#endif
