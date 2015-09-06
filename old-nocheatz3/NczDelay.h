#include "eiface.h"
#include <map>
#include <string>

class IVEngineServer;
extern IVEngineServer *engine;

#ifndef NCZ_DELAY_CLASS
#define NCZ_DELAY_CLASS

class NczBaseDelay
{
public:

	NczBaseDelay(float Time)
	{
		AddTime = Time;
		EndTime = this->GetTime() + AddTime;
	};

	NczBaseDelay()
	{
		AddTime = 0.0;
		EndTime = 0.0;
		theFunc = 0;
	};

	virtual ~NczBaseDelay(){};

	virtual float GetTime(void)
	{
		if(engine)
		{
			return engine->Time();
		}
		return 0.0;
	};

	virtual void SetP(bool (*fptr)(void))
	{
		theFunc = fptr;
	};

	virtual void SetTime(float Time)
	{
		AddTime = Time;
		EndTime = this->GetTime() + AddTime;
	};

	virtual void LaunchFunc()
	{
		if(theFunc)
		{
			theFunc();
		}
	};

	virtual bool IsEnd(void)
	{
		if(this->GetTime() >= EndTime)
		{
			return true;
		}
		return false;
	};

	float EndTime;
	float AddTime;
	bool (*theFunc)(void);
};

class NczDelay : public NczBaseDelay
{
public:
	NczDelay(float Time, bool (*fptr)(void))
	{
		theFunc = fptr;
		EndTime = this->GetTime() + Time;
		fLaunched = false;
	};

	virtual ~NczDelay(){};

	virtual void Update(void)
	{
		if((this->IsEnd()) && (!fLaunched))
		{
			theFunc();
			fLaunched = true;
		}
	};

private:
	bool fLaunched;
};

class NczDeamonDelay : public NczBaseDelay
{
public:
	NczDeamonDelay(float Time, bool (*fptr)(void))
	{
		theFunc = fptr;
		AddTime = Time;
		EndTime = this->GetTime() + AddTime;
	};

	virtual ~NczDeamonDelay(){};

	virtual void Update(void)
	{
		if(this->IsEnd())
		{
			EndTime += AddTime;
			theFunc();
		}
	};
};

enum DelayType
{
	SINGLE_DELAY,
	DEAMON_DELAY
};

class NczDelayManager
{
public:
	NczDelayManager();
	~NczDelayManager();

	void CreateDelay(std::string DelayName, DelayType Type, float Interval, bool (*fptr)(void))
	{
		if(!IsDelayExists(DelayName, Type))
		{
			if(Type == SINGLE_DELAY)
			{
				Delays[DelayName] = new NczDelay(Interval, fptr);
			}
			else
			{
				DeamonDelays[DelayName] = new NczDeamonDelay(Interval, fptr);
			}
		}
	};

	void Update(void)
	{
		std::map<std::string, NczDelay*>::const_iterator db(Delays.begin()), de(Delays.end());

	};

	void DeleteDelay(std::string DelayName, DelayType Type)
	{
		if(!IsDelayExists(DelayName, Type))
		{
			if(Type == SINGLE_DELAY)
			{
				delete Delays[DelayName];
				Delays.erase(Delays.find(DelayName));
			}
			else
			{
				delete DeamonDelays[DelayName];
				DeamonDelays.erase(DeamonDelays.find(DelayName));
			}
		}
	};

	bool IsDelayExists(std::string DelayName, DelayType Type)
	{
		if(Type == SINGLE_DELAY)
		{
			std::map<std::string, NczDelay*>::const_iterator db(Delays.find(DelayName)), de(Delays.end());
			if(db != de)
			{
				return true;
			}
		}
		else
		{
			std::map<std::string, NczDeamonDelay*>::const_iterator db(DeamonDelays.find(DelayName)), de(DeamonDelays.end());
			if(db != de)
			{
				return true;
			}
		}
		return false;
	};

private:
	std::map<std::string, NczDeamonDelay*> DeamonDelays;
	std::map<std::string, NczDelay*> Delays;
};
#endif // NCZ_DELAY_CLASS
