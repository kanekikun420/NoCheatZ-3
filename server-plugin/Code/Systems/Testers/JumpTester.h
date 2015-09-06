#include <string>

#include "BaseTimedTester.h"
#include "Misc/temp_Singleton.h"
#include "Misc/IFaceManager.h"
#include "Misc/Helpers.h"
#include "Systems/Testers/Detections/temp_BaseDetection.h"
#include "Players/NczPlayerManager.h"
#include "Players/temp_PlayerDataStruct.h"
#include "Systems/BaseSystem.h"
#include "Hooks/OnGroundHookListener.h"
#include "Hooks/PlayerRunCommandHookListener.h"
#include "Systems/Pipeline/Pipeline.h"

/////////////////////////////////////////////////////////////////////////
// JumpTester
/////////////////////////////////////////////////////////////////////////

typedef struct OnGroundHolder
{
	int onGround_Tick;
	int notOnGround_Tick;

	int jumpCount;
} OnGroundHolderT;

typedef struct JumpCmdHolder
{
	bool lastJumpCmdState;

	int JumpDown_Tick;
	int JumpUp_Tick;

	int outsideJumpCmdCount;
} JumpCmdHolderT;

typedef struct JumpInfo
{
	OnGroundHolderT onGroundHolder;
	JumpCmdHolderT jumpCmdHolder;
	int total_bhopCount;

	int goodBhopsCount;
	int perfectBhopsPercent;
	int perfectBhopsCount;

	bool isOnGround;
} JumpInfoT;

class Detection_BunnyHopScript : public LogDetection<JumpInfoT>
{
	typedef LogDetection<JumpInfoT> hClass;
public:
	Detection_BunnyHopScript() : hClass() {};
	~Detection_BunnyHopScript(){};

	virtual const char * GetDataDump();
	virtual const char * GetDetectionLogMessage()
	{
		return "BunnyHop Script";
	};
};

class Detection_BunnyHopProgram : public ReportDetection<JumpInfoT>
{
	typedef ReportDetection<JumpInfoT> hClass;
public:
	Detection_BunnyHopProgram() : hClass() {};
	~Detection_BunnyHopProgram(){};

	virtual const char * GetDataDump();
	virtual const char * GetDetectionLogMessage()
	{
		return "BunnyHop Program";
	};
};

class JumpTester :
	public BaseSystem,
	public BaseTimedTester,
	public OnGroundHookListener,
	public PlayerRunCommandHookListener,
	public PlayerDataStructHandler<JumpInfoT>,
	public Singleton<JumpTester>
{
public:
	JumpTester();
	~JumpTester();

	virtual void Load();
	virtual void Unload();
	virtual const char * GetName();
	virtual bool PlayerRunCommandCallback(NczPlayer* player, CUserCmd* cmd);
	virtual void m_hGroundEntityStateChangedCallback(NczPlayer* player, bool new_isOnGround);
	virtual void ProcessPlayerTest(NczPlayer* player);
	virtual SlotStatus GetFilter();
};
