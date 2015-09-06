#include <list>

#ifdef GNUC
#include <cmath>
typedef unsigned long size_t;
#endif

#ifndef RMS_CLASS
#define RMS_CLASS

typedef float T_RMS;

class RMS
{
public:
	RMS();
	RMS(size_t N);
	~RMS();

	void Add(T_RMS Value);
	void SetN(size_t N);

	T_RMS GetRMS(void);
	T_RMS GetRMSSigned(void);
private:
	std::list<T_RMS> rms_array;
};

#endif
