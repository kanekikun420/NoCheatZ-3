//#pragma once
#include "MostUsedPointers.h"

#ifndef C_AUTOSTATUS
#define C_AUTOSTATUS

class AutoStatus : public MostUsedPointers
{
public:
	virtual void SetStatusState(bool allowed){allow_status = allowed;};

protected:
	AutoStatus(void);

	virtual void AutoStatusStart();
	virtual void AutoStatusFrame();
	virtual void AutoStatusAllConTell(const char * fmt, ...);
	virtual ~AutoStatus(void);

private:
	//virtual bool isStarted(){return isStarted;};
	virtual void End();

private:
	bool isStarted;
	bool allow_status;
	int ccl;
	int stage;
	int job_done;
	//float last_auto_status;
};

#endif