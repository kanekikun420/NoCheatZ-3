#include "NczThread.h"
#include <stdio.h>

NczThread::NczThread() :
	mustbekilled(false)
{
}

NczThread::~NczThread()
{
}

int NczThread::Run()
{
	mustbekilled = false;
	int res = UpdateThread_job();
	mustbekilled = true;
	return res;
}

bool NczThread::MustBeKilled()
{
	return mustbekilled;
}
