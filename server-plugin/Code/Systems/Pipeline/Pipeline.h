#include "Preprocessors.h"

#include "http.h"
#include "Systems/BaseSystem.h"
#include "Misc/temp_Singleton.h"
#include "Misc/Types.h"
#include "Misc/temp_BaseQueue.h"

#ifndef PIPELINE
#define PIPELINE

typedef enum JobType
{
	BANLIST = 0,
	REPORT,
	UPDATE_CONFIG,
	UPDATE_PLUGIN
} JobTypeT;

typedef struct JobInfo
{
	JobTypeT jobType;
	ptr data;
} JobInfoT;

class Pipeline : public BaseSystem, public Singleton<Pipeline>
{
public:
	Pipeline();
	~Pipeline();

	virtual void Load();
	virtual void Unload();
	virtual int Run();

	BaseQueue<JobInfoT> Job;
};

#endif
