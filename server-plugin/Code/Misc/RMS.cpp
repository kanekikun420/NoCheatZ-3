#include "RMS.h"

RMS::RMS()
{
	rms_array.clear();
	rms_array.resize(0);
}

RMS::RMS(size_t N)
{
	rms_array.resize(N, 0);
}

RMS::~RMS()
{
}

void RMS::Add(T_RMS Value)
{
	rms_array.push_back(fabs(Value));
	rms_array.pop_front();
}

void RMS::SetN(size_t N)
{
	rms_array.resize(N, 0);
}

T_RMS RMS::GetRMS(void)
{
	T_RMS addy = 0;
	for(std::list<T_RMS>::iterator it = rms_array.begin(); it != rms_array.end(); ++it)	addy += (*it) * (*it);
	T_RMS rms = sqrt(addy / (float)(rms_array.size()));
	return rms;
}

T_RMS RMS::GetRMSSigned(void)
{
	T_RMS addy = 0;
	for(std::list<T_RMS>::iterator it = rms_array.begin(); it != rms_array.end(); ++it)	addy += (*it) * (*it);
	T_RMS rms = sqrt(addy / (float)(rms_array.size()));
	if(rms_array.back() > 0.0f) rms *= -1.0f;
	return rms;
}
