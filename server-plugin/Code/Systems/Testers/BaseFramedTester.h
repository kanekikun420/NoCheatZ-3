#include <list>
#include "Players/NczPlayerManager.h"

#ifndef BASEFRAMEDTESTER
#define BASEFRAMEDTESTER

/////////////////////////////////////////////////////////////////////////
// BaseFramedTester
/////////////////////////////////////////////////////////////////////////

class BaseFramedTester
{
public:
	BaseFramedTester();
	virtual ~BaseFramedTester(){};

	/* Appelé par le plugin à chaque frame
	   Permet d'appeler les classes filles qui sont à l'écoute */
	static void OnFrame();

	/* Filtre du testeur, peut/doit être redéfinit dans la classe fille */
	virtual SlotStatus GetFilter(){return PLAYER_CONNECTED;};

protected:
	/* Appelé par OnFrame, sert à iétrer entre les différents joueurs en fonction du filtre */
	virtual void ProcessTests();

	/* Appelé par ProcessTests() lorsque le joueur correspond au filtre */
	virtual void ProcessPlayerTest(NczPlayer* player) = 0;

	/* Permet de se mettre à l'écoute de l'événement, appelé par Load/Unload des testeurs */
	static void RegisterFramedTester(BaseFramedTester* tester);
	static void RemoveFramedTester(BaseFramedTester* tester);

private:
	static std::list<BaseFramedTester*> m_framedTestersList;
};

#endif
