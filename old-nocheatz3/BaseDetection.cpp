#include "BaseDetection.h"
#include "Preprocessors.h"

//////////////////////////////////////////////////
~BaseDetectionSystem::BaseDetectionSystem(){}
//////////////////////////////////////////////////

BaseDetectionSystem*::GetBaseDetectionSystem()
{
	return this;
}

BaseDetectionSystem::BaseDetectionSystem()
{
	SetProcessInterval(1.0/66.0);
	On();
	wPlayer_manager = NULL;
	wLast_time_done = 0.0f;
}

BaseDetectionSystem::BaseDetectionSystem(time_z process_interval)
{
	SetProcessInterval(process_interval);
	On();
	wPlayer_manager = NULL;
	wLast_time_done = 0.0f;
}

void BaseDetectionSystem::On()
{
	wIs_active = true;
}

void BaseDetectionSystem::Off()
{
	wIs_active = false;
}

void BaseDetectionSystem::SetProcessInterval(time_z process_interval)
{
	wProcess_interval = process_interval;
}

const char * BaseDetectionSystem::GetName()
{
	return "BaseDetectionSystem";
}

time_z BaseDetectionSystem::GetProcessInterval()
{
	return wProcess_interval;
}

void BaseDetectionSystem::HandleProcess()
{
	if(!IsActive()) return;
	if(GetTime() - wLast_time_done >= this->GetProcessInterval())
	{
		// Additionnaly add metrics
		Process();
		wLast_time_done = GetTime();
	}
}

bool BaseDetectionSystem::IsActive()
{
	return wIs_active;
}

void BaseDetectionSystem::AttachPlayerManager(Cg_NCZ_PlayerManager * player_manager)
{
	if(playerManager) wPlayer_manager = playerManager;
}

//////////////////////////////////////////////////
~DetectionTrigger::DetectionTrigger(){}
//////////////////////////////////////////////////

DetectionTrigger::DetectionTrigger(bool initial_trigger_state)
{
	wTrigger_state = initial_trigger_state;
	ResetTriggerCount();
}

void DetectionTrigger::Trigger()
{
	wTrigger_state = !wTrigger_state;
	++wTrigger_count;
}

const char * DetectionTrigger::GetName()
{
	return "DetectionTrigger";
}

count_z DetectionTrigger::GetTriggerCount()
{
	return wTrigger_count;
}

void DetectionTrigger::ResetTriggerCount()
{
	wTrigger_count = 0;
}



