#include <list>

#include "Hook.h"
#include "Players/NczPlayerManager.h"
#include "plugin.h"

#ifndef ONGROUNDHOOKLISTENER
#define ONGROUNDHOOKLISTENER

/////////////////////////////////////////////////////////////////////////
// CBasePlayer::NetworkStateChanged_m_hGroundEntity(void*)
/////////////////////////////////////////////////////////////////////////

class CBasePlayer;

typedef void (HOOKFN_EXT *GroundEntity_t)(CBasePlayer *, int*);

class OnGroundHookListener
{
public:
	OnGroundHookListener();
	~OnGroundHookListener();

	/*
		Hook and unhook functions.
		FIXME : May not works well with others plugins ...
	*/
	static void HookOnGround(NczPlayer* player);
	static void UnhookOnGround();

protected:
	static void RegisterOnGroundHookListener(OnGroundHookListener* listener);
	static void RemoveOnGroundHookListener(OnGroundHookListener* listener);
	
	virtual void m_hGroundEntityStateChangedCallback(NczPlayer* player, bool new_isOnGround) = 0;

private:
#ifdef GNUC
	static void HOOKFN_INT nNetworkStateChanged_m_hGroundEntity(CBasePlayer* basePlayer, int * new_m_hGroundEntity);
#else
	static void HOOKFN_INT nNetworkStateChanged_m_hGroundEntity(CBasePlayer* basePlayer, void*, int * new_m_hGroundEntity);
#endif
	
	static std::list<OnGroundHookListener*> m_listeners;
	static GroundEntity_t gpOldGroundFn;
	static DWORD* pdwInterface;
};

#endif
