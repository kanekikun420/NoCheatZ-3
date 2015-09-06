#include "Types.h"
#include "PlayerManager.h"

/*
Each detection system will inherite from a Base Detection System class and will be called by PlayerManager class (Which is unique).
Iterate between all players in order to test for detections in Process().
*/

#ifndef C_DETECTION_SYSTEM
#define C_DETECTION_SYSTEM

class BaseDetectionSystem
{
public:
	BaseDetectionSystem();
	BaseDetectionSystem(time_z process_interval);
	virtual AttachPlayerManager(Cg_NCZ_PlayerManager * player_manager);
	virtual ~BaseDetectionSystem();
	
	/* Called by PlayerManager in ProcessDetectionsTests() */
	virtual void HandleProcess();
	virtual void On();
	virtual void Off();
	
	virtual void SetProcessInterval(time_z process_interval);
	
	virtual const char * GetName();
	virtual time_z GetProcessInterval();
	virtual BaseDetectionSystem * GetBaseDetectionSystem();
	virtual bool IsActive();
	
	virtual void Process() = 0;
	
	virtual void OnDetection() = 0;

private:
	time_z wProcess_interval; // s
	time_z wLast_time_done; // s
	Cg_NCZ_PlayerManager * wPlayer_manager;
	bool wIs_active;
};

class DetectionTrigger: public BaseDetectionSystem
{
public:
	DetectionTrigger(bool initial_trigger_state);
	virtual ~DetectionTrigger();
	
	virtual void Process() = 0;
	virtual void OnDetection() = 0;
	virtual void Trigger();
	virtual void ResetTriggerCount();
	
	virtual count_z GetTriggerCount();
	virtual const char * GetName();
	
private:
	bool wTrigger_state;
	count_z wTrigger_count;
};

#endif
