#include "Metrics.h"

Metrics::Metrics()
{
	m_name = "unknown";
	Reset();
}

Metrics::Metrics(const char * name)
{
	m_name = name;
	Reset();
}

void Metrics::Reset()
{
	m_startTime = m_endTime = m_avgExecTime = m_minExecTime = m_maxExecTime = m_totalExecTime = 0.0;
	m_execCount = 0;
}

void Metrics::StartExec()
{
	m_startTime = Plat_FloatTime() * 1000.0;
}

void Metrics::EndExec()
{
	if(m_startTime == 0.0f) return;
	double execTime = 0.0;

	m_endTime = Plat_FloatTime() * 1000.0;
	execTime = m_endTime - m_startTime;

	Assert(execTime >= 0.0);

	++m_execCount;
	m_totalExecTime += execTime;

	if(execTime > m_maxExecTime) m_maxExecTime = execTime;
	if((execTime < m_minExecTime) || (m_execCount == 1)) m_minExecTime = execTime;

	m_avgExecTime = m_totalExecTime / (double)(m_execCount);
}

void Metrics::PrintResults()
{
	//Msg("Results for metrics %s :\n", m_name);
	Msg("Average : %f ms\n", m_avgExecTime);
	Msg("Min : %f ms\n", m_minExecTime);
	Msg("Max : %f ms\n", m_maxExecTime);
	Msg("Count : %lu calls\n", m_execCount);
	Msg("Total time spent : %f s\n", m_totalExecTime / 1000.0);
}

double Metrics::GetMinExecTime()
{
	return m_minExecTime;
}
