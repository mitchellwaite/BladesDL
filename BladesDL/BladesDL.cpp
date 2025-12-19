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


BOOL WINAPI DllMain(HANDLE hInstDLL, DWORD fdwReason, LPVOID lpReserved)
{
	dllHandle = hInstDLL;
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		cprintf("[BladesDL] Loaded!");
		
		if(XboxKrnlVersion->Build == 1888)
		{
			// Xam hooks cause a bugcheck on 1888, so we'll patch the DNS
			// strings like RGLoader and hook the kernel instead
			PatchBlockLIVE();
			SetupLoadImageHook();
		}
		else if(XboxKrnlVersion->Build == 6717)
		{
			SetupDNSHook();
			SetupLoaderPrepHook();
		}
		else
		{
			cprintf("[BladesDL] Unsupported kernel version %d, not applying LoaderPrep hook or DNS patch", XboxKrnlVersion->Build);
		}

		SetupkeBugCheckExHook();
		SetupXamCheckExecPrivHook();
		PatchUpdStrings();
		ApplyPingPatch();

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