#include "platform.h"
#include "dbg.h"

#ifndef METRICS_CLASS
#define METRICS_CLASS

class Metrics
{
public:
	Metrics();
	Metrics(const char * name);
	~Metrics(){};

	void StartExec();
	void EndExec();

	void Reset();

	double GetMinExecTime();

	void PrintResults();

	const char * m_name;
	double m_startTime;
	double m_endTime;
	double m_avgExecTime;
	double m_totalExecTime;
	double m_minExecTime;
	double m_maxExecTime;
	unsigned long m_execCount;
};

#endif
