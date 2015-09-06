
#ifndef NCZ_EVENTS_CLASS
#define NCZ_EVENTS_CLASS

class BaseLocalEvent
{
public:
	BaseLocalEvent();
	virtual ~BaseLocalEvent();

	virtual void TriggerEvent();
	virtual void OnTrigger();
	virtual void Enable();
	virtual void Disable();

	virtual bool isEnabled() const;
private:
	bool active;
	float delay;
};

class LocalEvent : public BaseLocalEvent
{
public:
	LocalEvent();
	virtual ~LocalEvent();

	virtual void TriggerEvent();
	virtual void OnTrigger();
	virtual void Enable();
	virtual void Disable();

	virtual bool isEnabled() const;
private:
	bool active;
	float delay;
};

#endif
