#ifndef CSSRPG_SIGSCAN_H
#define CSSRPG_SIGSCAN_H

#ifdef WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <tlhelp32.h>
#endif

/* Forward Declarations */
class CBaseEntity;
class Vector;
class QAngle;
//enum MoveType_t;

#ifdef WIN32
class NCZ_SigScan
{
	/* Private Variables */
	static unsigned char *base_addr;
	static size_t base_len;

	unsigned char *sig_str;
	char *sig_mask;
	unsigned long sig_len;

	/* Private Functions */
	void parse_sig(char *sig);
	void* find_sig(void);

public:
	/* Public Variables */
	char sig_name[64];
	bool is_set; /* if the scan was successful or not */
	void *sig_addr;

	/* Public Functions */
	NCZ_SigScan(void): sig_str(NULL), sig_addr(NULL), sig_len(0) {}
	~NCZ_SigScan(void);

	static bool GetDllMemInfo(void);
	void Init(char *name, char *sig);

	unsigned long hextoul(char* hex);
};

unsigned int mistrcmp(char *str1, char* str2);

void init_sigs(void);
#endif

#ifndef WIN32 /* !WIN32 */
void init_lsym_funcs(void);
#endif

/* TempEnts Hacked Pointer */
//extern class ITempEntsSystem* tempents;
//void Initialize_TE_Pointer(class IEffects *effects);

/* Hacked Functions */
void CBaseEntity_Teleport(CBaseEntity *cbe, const Vector *newPosition, const QAngle *newAngles, const Vector *newVelocity);

#endif
