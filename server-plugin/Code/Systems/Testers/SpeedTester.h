#include <string>

#include "BaseTimedTester.h"
#include "Misc/temp_Singleton.h"
#include "Misc/IFaceManager.h"
#include "Misc/Helpers.h"
#include "Systems/Testers/Detections/temp_BaseDetection.h"
#include "Players/NczPlayerManager.h"
#include "Players/temp_PlayerDataStruct.h"
#include "Systems/BaseSystem.h"
#include "Hooks/PlayerRunCommandHookListener.h"

/////////////////////////////////////////////////////////////////////////
// SpeedTester
/////////////////////////////////////////////////////////////////////////

typedef struct SpeedHolder
{
	size_t ticksLeft;
	size_t detections;
	double lastDetectionTime;
	double previousLatency;
	double lastTest;

	SpeedHolder()
	{
		ticksLeft = std::ceil((1.0 / CIFaceManager::GetInstance()->GetIgamedll()->GetTickInterval()) * 2.0);
		detections = 0;
		lastDetectionTime = previousLatency = lastTest = 0.0;
	};
	SpeedHolder(const SpeedHolder& other)
	{
		ticksLeft = other.ticksLeft;
		detections = other.detections;
		lastDetectionTime = other.lastDetectionTime;
		previousLatency = other.previousLatency;
		lastTest = other.lastTest;
	};
} SpeedHolderT;

class Detection_SpeedHack : public LogDetection<SpeedHolderT>
{
	typedef LogDetection<SpeedHolderT> hClass;
public:
	Detection_SpeedHack() : hClass() {};
	~Detection_SpeedHack(){};

	virtual const char * GetDataDump();
	virtual const char * GetDetectionLogMessage()
	{
		return "SpeedHack";
	};
};

class SpeedTester :
	public BaseSystem,
	public BaseTimedTester,
	public PlayerRunCommandHookListener,
	public PlayerDataStructHandler<SpeedHolderT>,
	public Singleton<SpeedTester>
{
public:
	SpeedTester();
	~SpeedTester();

	virtual void Load();
	virtual void Unload();
	virtual bool PlayerRunCommandCallback(NczPlayer* player, CUserCmd* cmd);
	virtual void ProcessPlayerTest(NczPlayer* player);
	virtual SlotStatus GetFilter();
};
