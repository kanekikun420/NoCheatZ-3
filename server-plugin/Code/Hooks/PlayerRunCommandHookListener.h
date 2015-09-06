#include <list>

#include "Hook.h"
#include "Players/NczPlayerManager.h"
#include "Misc/IFaceManager.h"

#ifndef PLAYERRUNCOMMANDHOOKLISTENER
#define PLAYERRUNCOMMANDHOOKLISTENER

/////////////////////////////////////////////////////////////////////////
// CCSPlayer::PlayerRunCommand(CUserCmd*, IMoveHelper*)
/////////////////////////////////////////////////////////////////////////

class CUserCmd
{
public:
        CUserCmd()
        {
                Reset();
        }

        virtual ~CUserCmd() { };

        void Reset()
        {
                command_number = 0;
                tick_count = 0;
                viewangles.Init();
                forwardmove = 0.0f;
                sidemove = 0.0f;
                upmove = 0.0f;
                buttons = 0;
                impulse = 0;
                weaponselect = 0;
                weaponsubtype = 0;
                random_seed = 0;
                mousedx = 0;
                mousedy = 0;
        }

        CUserCmd& operator =( const CUserCmd& src )
        {
                if ( this == &src )
                        return *this;

                command_number                = src.command_number;
                tick_count                        = src.tick_count;
                viewangles                        = src.viewangles;
                forwardmove                        = src.forwardmove;
                sidemove                        = src.sidemove;
                upmove                                = src.upmove;
                buttons                                = src.buttons;
                impulse                                = src.impulse;
                weaponselect                = src.weaponselect;
                weaponsubtype                = src.weaponsubtype;
                random_seed                        = src.random_seed;
                mousedx                                = src.mousedx;
                mousedy                                = src.mousedy;
                return *this;
        }

        CUserCmd( const CUserCmd& src )
        {
                *this = src;
        }

        void GetChecksum( void ) const
        {
        }

        // Allow command, but negate gameplay-affecting values
        void MakeInert( void )
        {
                viewangles = vec3_angle;
                forwardmove = 0.f;
                sidemove = 0.f;
                upmove = 0.f;
                buttons = 0;
                impulse = 0;
        }

        // For matching server and client commands for debugging
        int                command_number;
        
        // the tick the client created this command
        int                tick_count;
        
        // Player instantaneous view angles.
        QAngle        viewangles;
        // Intended velocities
        //        forward velocity.
        float        forwardmove;
        // sideways velocity.
        float        sidemove;
        // upward velocity.
        float        upmove;
        // Attack button states
        int                buttons;                
        // Impulse command issued.
        byte impulse;
        // Current weapon id
        int                weaponselect;        
        int                weaponsubtype;

        int                random_seed;        // For shared random functions

        short        mousedx;                // mouse accum in x from create move
        short        mousedy;                // mouse accum in y from create move
};

class CCSPlayer;
class IMoveHelper;

//typedef void (HOOKFN_EXT *PlayerRunCommand_t)(void*, CUserCmd*, IMoveHelper*);
typedef void (HOOKFN_EXT *PlayerRunCommand_t)(void*, CUserCmd*, IMoveHelper*);

class PlayerRunCommandHookListener
{
public:
	PlayerRunCommandHookListener();
	~PlayerRunCommandHookListener();

	static void HookPlayerRunCommand(NczPlayer* player);

	static void UnhookPlayerRunCommand();
	static CUserCmd* GetLastUserCmd(NczPlayer* player);

protected:
	static void RegisterPlayerRunCommandHookListener(PlayerRunCommandHookListener* listener);
	static void RemovePlayerRunCommandHookListener(PlayerRunCommandHookListener* listener);

	virtual bool PlayerRunCommandCallback(NczPlayer* player, CUserCmd* cmd) = 0;

private:
#ifdef GNUC
	static void HOOKFN_INT nPlayerRunCommand(void* This, CUserCmd* pCmd, IMoveHelper* pMoveHelper);
#else
	static void HOOKFN_INT nPlayerRunCommand(void* This, void*, CUserCmd* pCmd, IMoveHelper* pMoveHelper);
#endif
	static std::list<PlayerRunCommandHookListener*> m_listeners;
	static CUserCmd m_lastCUserCmd[MAX_PLAYERS];
	static PlayerRunCommand_t gpOldPlayerRunCommand;
	static DWORD* pdwInterface;
};

#endif
