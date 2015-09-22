#include "igameevents.h"

#ifndef EV_MANAGER_WRAP
#define EV_MANAGER_WRAP

#ifndef EVENT_DEBUG_ID_INIT
#	define EVENT_DEBUG_ID_INIT 42
#endif
#ifndef EVENT_DEBUG_ID_SHUTDOWN
#	define EVENT_DEBUG_ID_SHUTDOWN 13
#endif

class EventManagerWrap : public IGameEventListener2
{
public:
	EventManagerWrap()
	{
		m_debugid = EVENT_DEBUG_ID_INIT;
	};
	virtual ~EventManagerWrap()
	{
		m_debugid = EVENT_DEBUG_ID_SHUTDOWN;
	};

	virtual void FireGameEvent(IGameEvent* ev) = 0;

	virtual int GetEventDebugID()
	{
		return m_debugid;
	};

private:
	int m_debugid;
};

#endif
