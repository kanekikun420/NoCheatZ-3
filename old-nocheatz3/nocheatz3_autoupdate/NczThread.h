#include "threadtools.h"

#ifndef NCZ_THREAD_CLASS
#define NCZ_THREAD_CLASS

extern int UpdateThread_job();

class NczThread : public CThread
{
public:
	NczThread();
	~NczThread();

	virtual int Run();

	virtual bool MustBeKilled();
private:
	bool mustbekilled;
};

#endif // NCZ_THREAD_CLASS
