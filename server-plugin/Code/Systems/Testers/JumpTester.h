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

/////////////////////////////////////////////////////////////////////////
// JumpTester
/////////////////////////////////////////////////////////////////////////

typedef struct OnGroundHolder
{
	int onGround_Tick;
	int notOnGround_Tick;

	int jumpCount;

	OnGroundHolder()
	{
		onGround_Tick = notOnGround_Tick = jumpCount = 0;
	};
	OnGroundHolder(const OnGroundHolder& other)
	{
		onGround_Tick = other.onGround_Tick;
		notOnGround_Tick = other.notOnGround_Tick;
		jumpCount = other.jumpCount;
	};
} OnGroundHolderT;

typedef struct JumpCmdHolder
{
	bool lastJumpCmdState;

	int JumpDown_Tick;
	int JumpUp_Tick;

	int outsideJumpCmdCount; // Jumps made while the player doesn't touch the ground

	JumpCmdHolder()
	{
		lastJumpCmdState = false;
		JumpDown_Tick = JumpUp_Tick = outsideJumpCmdCount = 0;
	};
	JumpCmdHolder(const JumpCmdHolder& other)
	{
		lastJumpCmdState = other.lastJumpCmdState;
		JumpDown_Tick = other.JumpDown_Tick;
		JumpUp_Tick = other.JumpUp_Tick;
		outsideJumpCmdCount = other.outsideJumpCmdCount;
	};
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

	JumpInfo()
	{
		onGroundHolder = OnGroundHolder();
		jumpCmdHolder = JumpCmdHolder();
		total_bhopCount = goodBhopsCount = perfectBhopsPercent = perfectBhopsCount = 0;
		isOnGround = false;
	};
	JumpInfo(const JumpInfo& other)
	{
		onGroundHolder = other.onGroundHolder;
		jumpCmdHolder = other.jumpCmdHolder;
		total_bhopCount = other.total_bhopCount;
		goodBhopsCount = other.goodBhopsCount;
		perfectBhopsPercent = other.perfectBhopsPercent;
		perfectBhopsCount = other.perfectBhopsCount;
		isOnGround = other.isOnGround;
	};
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

class Detection_BunnyHopProgram : public LogDetection<JumpInfoT>
{
	typedef LogDetection<JumpInfoT> hClass;
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
	virtual bool PlayerRunCommandCallback(NczPlayer* player, CUserCmd* cmd);
	virtual void m_hGroundEntityStateChangedCallback(NczPlayer* player, bool new_isOnGround);
	virtual void ProcessPlayerTest(NczPlayer* player);
	virtual SlotStatus GetFilter();
};
