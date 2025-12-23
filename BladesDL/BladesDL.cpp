//===============================================================================================================================================
//
//		BladesDL - A basic Dashlaunch substitute for Blades kernel (6770 and 1888). Performs some of the basic tasks Dashlaunch would normally provide.
//
// Created by Byrom - https://github.com/Byrom90
//
// Credits:
//			- c0z - Majority of the functions/hooks were backported from an old version of Dashlaunch
//
//===============================================================================================================================================

#include "stdafx.h"
#include "BladesDL.h"

// version and launch helper data structure
extern ldata ldat = {
	LAUNCH_DATA_ID,	// DWORD ID;
	LHELPER_CON,	// DWORD ltype;
	"",				// char link[MAX_PATH];
	"",				// char dev[MAX_PATH];
	VER_MAJ,		// USHORT versionMaj;
	VER_MIN,		// USHORT versionMin;
	TARGET_KERNEL	// USHORT targetKernel;
};

BOOL WINAPI DllMain(HANDLE hInstDLL, DWORD fdwReason, LPVOID lpReserved)
{
	dllHandle = hInstDLL;
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		cprintf("[BladesDL] Loaded!");
		SetupDNSHook();
		SetupMemoryProtectionToggleHook();
		SetupHeaderVerificationToggleHook();
		SetupkeBugCheckExHook();
		SetupXamCheckExecPrivHook();
		PatchUpdStrings();
		ApplyPingPatch();
		ApplyContentPatch();

		cprintf("[BladesDL] Init complete!");
		break;
	case DLL_PROCESS_DETACH:
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}