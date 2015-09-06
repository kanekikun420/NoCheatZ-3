#include "BaseFramedTester.h"

/////////////////////////////////////////////////////////////////////////
// BaseFramedTester
/////////////////////////////////////////////////////////////////////////

std::list<BaseFramedTester*> BaseFramedTester::m_framedTestersList;

BaseFramedTester::BaseFramedTester()
{
}

void BaseFramedTester::ProcessTests()
{
	PLAYERS_LOOP_RUNTIME
	{
		if(ph->status == GetFilter())
		{
			ProcessPlayerTest(ph->playerClass);
		}
	}
	END_PLAYERS_LOOP
}

void BaseFramedTester::RegisterFramedTester(BaseFramedTester* tester)
{
	m_framedTestersList.push_back(tester);
}

void BaseFramedTester::RemoveFramedTester(BaseFramedTester* tester)
{
	m_framedTestersList.remove(tester);
}

void BaseFramedTester::OnFrame()
{
	for (std::list<BaseFramedTester*>::iterator it = m_framedTestersList.begin(); it != m_framedTestersList.end(); ++it)
	{
		(*it)->ProcessTests();
	}
}
