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
	vec_t value;
	vec_t abs_value;

	double lastDetectionPrintTime;
	unsigned int detectionsCount;
} EyeAngleT;

typedef struct EyeAngleInfo
{
	unsigned int ignore_last;

	EyeAngleT x;
	EyeAngleT y;
	EyeAngleT z;
} EyeAngleInfoT;

class Detection_EyeAngle : public ReportDetection<EyeAngleInfoT>
{
	typedef ReportDetection<EyeAngleInfoT> hClass;
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
	virtual const char * GetName();
	virtual SlotStatus GetFilter();
	virtual bool PlayerRunCommandCallback(NczPlayer* player, CUserCmd* pCmd);
	virtual void FireGameEvent(IGameEvent *ev);
	virtual void TeleportCallback(NczPlayer* player, Vector const* va, QAngle const* qa, Vector const* vb);
};

#endif
