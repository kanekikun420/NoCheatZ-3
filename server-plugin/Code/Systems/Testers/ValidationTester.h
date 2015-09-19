#include "BaseTimedTester.h"
#include "Misc/temp_Singleton.h"
#include "Systems/BaseSystem.h"
#include "Players/NczPlayerManager.h"
#include "Players/NczFilteredPlayersList.h"
#include "Players/temp_PlayerDataStruct.h"
#include "Misc/IFaceManager.h"
#include "Misc/Helpers.h"

/////////////////////////////////////////////////////////////////////////
// ValidationTester
/////////////////////////////////////////////////////////////////////////

class ValidationTester :
	public BaseSystem,
	public NczFilteredPlayersList,
	public BaseTimedTester,
	public PlayerDataStructHandler<bool>,
	public Singleton<ValidationTester>
{
public:
	ValidationTester();
	~ValidationTester();

	virtual void Load();
	virtual void Unload();
	virtual void SetValidated(NczPlayer* player);
	virtual void ProcessPlayerTest(NczPlayer* player);
	virtual SlotStatus GetFilter();

	short GetJobCount();
};
