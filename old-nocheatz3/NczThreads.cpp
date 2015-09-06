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
	int res =  WebThread();
	mustbekilled = true;
	return res;
}

bool NczThread::MustBeKilled()
{
	return mustbekilled;
}
