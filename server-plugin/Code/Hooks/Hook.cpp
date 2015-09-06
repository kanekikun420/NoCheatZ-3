#include "Hook.h"

DWORD VirtualTableHook(DWORD* classptr, int vtable, DWORD newInterface )
{
		DWORD dwOld, dwStor = 0x0;
		if(!(classptr || vtable || newInterface)) return 0;
	#ifdef WIN32
		if(!VirtualProtect(&classptr[vtable], (vtable * sizeof(void *)) + 4, PAGE_EXECUTE_READWRITE, &dwOld ))
		{
			Assert(false);
			return 0;
		}
	#else

        DWORD psize = sysconf(_SC_PAGESIZE);
		void *p = (void *)((DWORD)(&classptr[vtable]) & ~(psize-1));
		if(mprotect(p, ((vtable * sizeof(void *)) + ((DWORD)(&classptr[vtable]) & (psize-1))), PROT_READ | PROT_WRITE | PROT_EXEC ) < 0)
		{
			Assert(false);
			return 0;
		}
	#endif
		dwStor = classptr[vtable];
	#ifdef WIN32
		*(DWORD*)&(classptr[vtable]) = newInterface;
		VirtualProtect(&classptr[vtable], (vtable * sizeof(void *)) + 4, dwOld, &dwOld);
	#else
		*(DWORD*)&(classptr[vtable]) = newInterface;
		mprotect(p, ((vtable * sizeof(void *)) + ((DWORD)(&classptr[vtable]) & (psize-1))), PROT_READ | PROT_EXEC );
	#endif
		return dwStor;
}
