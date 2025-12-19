#include "stdafx.h"


VOID PatchUpdStrings(VOID)
{
	DWORD siz = 0;
	PBYTE ptr = getModBaseSize(MODULE_XAM, &siz);
	//DbgPrint("updater detect patch start %08x size %08x\n", ptr, siz);
	int Str_Patched = 0;
	if ((ptr != NULL) && (siz != 0))
	{
		DWORD i;
		for (i = 0; i < siz; i++)
		{
			if (ptr[i] == '$')
			{
				if (strnicmp("$systemupdate", (char*)&ptr[i], strlen("$systemupdate")) == 0)
				{
					ptr[i + 1] = '$';
					//DbgPrint("patch %s at %08x\n", &ptr[i], &ptr[i]);
					i += strlen("$systemupdate");
					Str_Patched++;
				}
			}
		}
	}
	cprintf("[BladesDL] [PatchUpdStrings] Patched %i strings to $$ystemupdate", Str_Patched);
}


// Ping Patch - NOP the jump for when ping exceeds 30
VOID ApplyPingPatch()
{
	PDWORD ptr = NULL;

	cprintf("[BladesDL] [PingPatch] Removing ping limit for system link play");

	if(XboxKrnlVersion->Build == 1888)
	{
		ptr = (PDWORD)PING_PATCH_ADDR_1888;
	}
	else if(XboxKrnlVersion->Build == 6770)
	{
		ptr = (PDWORD)PING_PATCH_ADDR_6770;
	}
	else
	{
		cprintf("[BladesDL] [PingPatch] Unsupported kernel: %d", XboxKrnlVersion->Build);
		return;
	}

	ptr[0] = 0x60000000;
	doSync(ptr);	
}

VOID PatchBlockLIVE(){
	cprintf("[BladesDL] [PatchBlockLIVE] Applying LiveBlock patch to XAM");

	char* nullStr = "NO.%sNO.NO\0";
	DWORD nullStrSize = 18;

	if(XboxKrnlVersion->Build == 1888)
	{
		// null out xbox live dns tags in xam. This is basically what RGLoader does
		// for its live block setting, except 1888 has fewer strings to patch
		memcpy((LPVOID)0x81885A80, (LPCVOID)nullStr, nullStrSize); // notice.%sxbox.com
		memcpy((LPVOID)0x81885A94, (LPCVOID)nullStr, nullStrSize); // xeds.%sxboxlive.com
		memcpy((LPVOID)0x81885AAC, (LPCVOID)nullStr, nullStrSize); // xetgs.%sxboxlive.com
		memcpy((LPVOID)0x81885AC4, (LPCVOID)nullStr, nullStrSize); // xeas.%sxboxlive.com
		memcpy((LPVOID)0x81885AD8, (LPCVOID)nullStr, nullStrSize); // xemacs.%sxboxlive.com
	}
	else
	{
		cprintf("[BladesDL] [PatchBlockLIVE] Unsupported kernel: %d", XboxKrnlVersion->Build);
	}
}

