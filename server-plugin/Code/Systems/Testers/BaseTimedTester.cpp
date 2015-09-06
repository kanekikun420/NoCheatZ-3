#include "BaseTimedTester.h"

/////////////////////////////////////////////////////////////////////////
// BaseTimedTester
/////////////////////////////////////////////////////////////////////////

BaseTimedTester::BaseTimedTester() : BaseFramedTester(), m_lastTestDone(0.0), m_jobMinPeriod(1.0)
{
	m_timeBase = SERVER;
}

BaseTimedTester::BaseTimedTester(double time, TimeBase timeBase) : BaseFramedTester(), m_lastTestDone(0.0), m_jobMinPeriod(time)
{
	m_timeBase = timeBase;
}

double BaseTimedTester::GetTime()
{
	if(m_timeBase == GAME)
	{
		return (double)(CIFaceManager::GetInstance()->GetIengine()->Time());
	}
	else
	{
		return Plat_FloatTime();
	}
}

void BaseTimedTester::ProcessTests()
{
	if((Plat_FloatTime() - m_lastTestDone) < m_jobMinPeriod) return;
	m_lastTestDone = Plat_FloatTime();

	PlayerHandler* h = nullptr;
	for(int x = 1; x <= MAX_PLAYERS; ++x)
	{
		h = NczPlayerManager::GetInstance()->GetPlayerHandlerByIndex(x);
		if(h->status == GetFilter())
		{
			ProcessPlayerTest(h->playerClass);
		}
	}
}
