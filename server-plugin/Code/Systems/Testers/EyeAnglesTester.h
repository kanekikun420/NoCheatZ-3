#include "igameevents.h"

#ifdef GNUC
#undef min
#undef max
#endif

#include "Systems/BaseSystem.h"
#include "Systems/Testers/Detections/temp_BaseDetection.h"
#include "Hooks/PlayerRunCommandHookListener.h"
#include "Hooks/TeleportHookListener.h"
#include "Players/temp_PlayerDataStruct.h"
#include "Players/NczFilteredPlayersList.h"
#include "Misc/temp_Singleton.h"
#include "Misc/IFaceManager.h"

#ifndef EYEANGLESTESTER
#define EYEANGLESTESTER

typedef struct EyeAngle
{
	vec_t value; // Raw value of the angle
	vec_t abs_value; // Abs value so it's easier to test

	double lastDetectionPrintTime;
	unsigned int detectionsCount;

	EyeAngle()
	{
		value = abs_value = 0.0;
		lastDetectionPrintTime = 0.0;
		detectionsCount = 0;
	};
	EyeAngle(const EyeAngle& other)
	{
		value = other.value;
		abs_value = other.abs_value;
		lastDetectionPrintTime = other.lastDetectionPrintTime;
		detectionsCount = other.detectionsCount;
	};
} EyeAngleT;

typedef struct EyeAngleInfo
{
	unsigned int ignore_last; // Ignore values potentially not initialized by the engine

	EyeAngleT x;
	EyeAngleT y;
	EyeAngleT z;

	EyeAngleInfo()
	{
		ignore_last = 0;
		x = y = z = EyeAngle();
	};
	EyeAngleInfo(const EyeAngleInfo& other)
	{
		ignore_last = other.ignore_last;
		x = other.x;
		y = other.y;
		z = other.z;
	};
} EyeAngleInfoT;

class Detection_EyeAngle : public LogDetection<EyeAngleInfoT>
{
	typedef LogDetection<EyeAngleInfoT> hClass;
public:
	Detection_EyeAngle() : hClass() {};
	~Detection_EyeAngle(){};

	virtual const char * GetDataDump();
};

class Detection_EyeAngleX : public Detection_EyeAngle
{
public:
	Detection_EyeAngleX(){};
	~Detection_EyeAngleX(){};

	virtual const char * GetDetectionLogMessage();
};

class Detection_EyeAngleY : public Detection_EyeAngle
{
public:
	Detection_EyeAngleY(){};
	~Detection_EyeAngleY(){};

	virtual const char * GetDetectionLogMessage();
};

class Detection_EyeAngleZ : public Detection_EyeAngle
{
public:
	Detection_EyeAngleZ(){};
	~Detection_EyeAngleZ(){};

	virtual const char * GetDetectionLogMessage();
};

class EyeAnglesTester :
	public BaseSystem,
	public PlayerRunCommandHookListener,
	public TeleportHookListener,
	public PlayerDataStructHandler<EyeAngleInfoT>,
	public IGameEventListener2,
	public Singleton<EyeAnglesTester>
{
public:
	EyeAnglesTester();
	virtual ~EyeAnglesTester();

	virtual void Load();
	virtual void Unload();
	virtual SlotStatus GetFilter();
	virtual bool PlayerRunCommandCallback(NczPlayer* player, CUserCmd* pCmd);
	virtual void FireGameEvent(IGameEvent *ev);
	virtual void TeleportCallback(NczPlayer* player, Vector const* va, QAngle const* qa, Vector const* vb);
};

#endif
