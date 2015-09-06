#include "BaseFramedTester.h"
#include "Misc/IFaceManager.h"

#ifndef BASETIMEDTESTER
#define BASETIMEDTESTER


enum TimeBase
{
	GAME = 0,
	SERVER
};

class BaseTimedTester : public BaseFramedTester
{
public:
	BaseTimedTester();
	BaseTimedTester(double time, TimeBase timeBase = SERVER);
	~BaseTimedTester(){};

	double GetTime();

	/* Combien de secondes attendre avant de lancer un nouveau cycle */
	void SetJobPeriod(double time){m_jobMinPeriod = time;};

	/* Nouvelle version prenant en compte le temps entre les cycles */
	virtual void ProcessTests();

	virtual void ProcessPlayerTest(NczPlayer* player) = 0;

protected:
	double m_lastTestDone;
	double m_jobMinPeriod;
	TimeBase m_timeBase;
};

#endif
