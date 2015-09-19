#include <list>

#include "platform.h"
#include "dbg.h"
#include "convar.h"

#ifdef GNUC
#undef min
#undef max
#endif

#include "Preprocessors.h"
#include "Misc/Helpers.h"
#include "Players/NczPlayerManager.h"

#ifndef BASESYSTEM
#define BASESYSTEM

/////////////////////////////////////////////////////////////////////////
// BaseSystem
/////////////////////////////////////////////////////////////////////////

class BaseSystem
{
public:
	BaseSystem();
	virtual ~BaseSystem();

	/* Permet de détruire tous les systèmes créés jusqu'à présent */
	static void UnloadAllSystems();

	/* Commande console de base "ncz" */
	static	void ncz_cmd_fn ( const CCommand &args );
	/* Commande(s) console du système,
		donne vrai si la commande existe
		Le premier argument est en fait le troisième */
	virtual bool sys_cmd_fn ( const CCommand &args ){return false;};
	virtual const char * cmd_list () {return "enable\ndisable\nor verbose\n";};

	/* Donne le nom du système pour pouvoir être identifié dans la console */
	const char * GetName() const {return m_name;};

	/* Process Load when m_isActive changes to true
	      "    Unload when m_isActive changes to false */
	void SetActive(bool active);

	/* Must be used before all processing operations */
	bool IsActive() const {return m_isActive;};

	/* Retourne vrai si le fichier de configuration dit d'activer ce système */
	bool IsEnabledByConfig() const {return m_configState;};

	void SetConfig(bool enabled){m_configState = enabled;};
	
	/* Used for debugging */
	void SetVerbose(bool verbose);
	bool HasVerbose() const {return m_verbose;};

protected:
	const char* m_name;

private: // called by SetActive()
	virtual void Load() = 0; // Defined by child, attach to callbacks
	virtual void Unload() = 0; // Defined by child, unregister from callbacks

	bool m_isActive;
	bool m_isDisabled;
	bool m_configState;
	bool m_verbose;
	static std::list<BaseSystem*> m_systemsList;
};

#endif
