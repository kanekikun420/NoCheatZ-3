#include "BaseFramedTester.h"
#include "Misc/temp_Singleton.h"
#include "Systems/Testers/Detections/temp_BaseDetection.h"
#include "Systems/BaseSystem.h"
#include "Players/NczPlayerManager.h"
#include "Players/NczFilteredPlayersList.h"
#include "Players/temp_PlayerDataStruct.h"
#include "Misc/IFaceManager.h"
#include "Misc/Helpers.h"
#include "Systems/BanRequest.h"

/////////////////////////////////////////////////////////////////////////
// ConVarTester
/////////////////////////////////////////////////////////////////////////

typedef enum ConVarRule
{
	SAME = 0,
	SAME_FLOAT,
	SAME_AS_SERVER,
	SAME_FLOAT_AS_SERVER,
	NO_VALUE
} ConVarRuleT;

typedef struct ConVarInfo
{
	char name[64];
	char value[64];
	ConVarRuleT rule;
	bool safe;

	ConVarInfo()
	{
		*name='\0';
		*value='\0';
		rule=SAME;
		safe=false;
	};
	ConVarInfo(const char* pname, const char* pvalue, ConVarRuleT prule, bool psafe)
	{
		strcpy(name, pname);
		strcpy(value, pvalue);
		rule=prule;
		safe=psafe;
	};
	ConVarInfo(const ConVarInfo& other)
	{
		strcpy(name, other.name);
		strcpy(value, other.value);
		rule=other.rule;
		safe=other.safe;
	};
} ConVarInfoT;

typedef struct CurrentConVarRequest
{
	bool isSent;
	bool isReplyed;
	double timeStart;
	ConVarInfoT* ruleset;

	CurrentConVarRequest(){isSent=false;isReplyed=false;timeStart=0.0;ruleset=nullptr;};
	CurrentConVarRequest(const CurrentConVarRequest& other){isSent=other.isSent;isReplyed=other.isReplyed;timeStart=other.timeStart;ruleset=other.ruleset;};
} CurrentConVarRequestT;

class Detection_ConVar : public LogDetection<CurrentConVarRequestT>
{
	typedef LogDetection<CurrentConVarRequestT> hClass;
public:
	Detection_ConVar() : hClass() {};
	~Detection_ConVar(){};

	virtual const char * GetDataDump();
	virtual const char * GetDetectionLogMessage();
};

class ConVarTester :
	public BaseSystem,
	public AsyncNczFilteredPlayersList,
	public BaseFramedTester,
	public PlayerDataStructHandler<CurrentConVarRequestT>,
	public Singleton<ConVarTester>
{
public:
	ConVarTester();
	~ConVarTester();

	virtual void Load();
	virtual void Unload();

	/* Nouvelle version de la fonction qui va faire en sorte de ne tester qu'un seul joueur par frame */
	virtual void ProcessTests();

	virtual void ProcessPlayerTest(NczPlayer* player);

	virtual SlotStatus GetFilter();

	virtual bool sys_cmd_fn ( const CCommand &args );

	void OnQueryCvarValueFinished(NczPlayer* player, EQueryCvarValueStatus eStatus, const char *pCvarName, const char *pCvarValue);

private:
	ConVarInfoT* GetNextConvar(const char * current_convar_name);
	void AddConvarRuleset(const char * name, const char * value, ConVarRuleT rule, bool safe = true);
	ConVarInfoT* FindConvarRuleset(const char * name);

	std::list<ConVarInfoT> m_convars_rules;
};
