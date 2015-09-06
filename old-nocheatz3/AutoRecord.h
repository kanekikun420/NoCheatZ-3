#ifndef H_AUTORECORD
#define H_AUTORECORD

struct edict_t;

#define TRUNK_DEMO_AT 241200

class AutoRecord
{
public:
	AutoRecord();
	~AutoRecord();

	void Update();
	void setAutoRecord(bool AutoRecord);
	void registerUnexpectedRecord(const char * DemoName);

	bool isRecording();
	bool isTVConnected();
	edict_t * getTV();

private:
	void StartRecord(const char * DemoName);
	void StopRecord();
	void TrunkRecord();

	long m_ulTickCount;
	const char * m_sDemoName;
	bool m_bIsRecording;
	bool m_bAutoRecord;
};

#endif
