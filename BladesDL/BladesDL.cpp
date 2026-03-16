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
#include "INIReader.h"

static INIReader* reader;

// version and launch helper data structure
extern ldata ldat = {
	LAUNCH_DATA_ID,	// DWORD ID;
	LHELPER_CON,		// DWORD ltype;
	"",					// char link[MAX_PATH];
	"",					// char dev[MAX_PATH];
	VER_MAJ,				// USHORT versionMaj;
	VER_MIN,				// USHORT versionMin;
	TARGET_KERNEL		// USHORT targetKernel;
};

void Mount(char* dev, char* mnt)
{
	ANSI_STRING asDevice, asMount;
    RtlInitAnsiString(&asDevice, dev);
	RtlInitAnsiString(&asMount, mnt);
	ObCreateSymbolicLink(&asMount, &asDevice);
}

BOOL LoadPlugins()
{
	// Mount all the drives
	Mount("\\Device\\Harddisk0\\Partition1", "\\System??\\Hdd:");
	Mount("\\Device\\Mass0", "\\System??\\Usb:");
	Mount("\\Device\\Mass0", "\\System??\\Mass0:");
	Mount("\\Device\\Mass1", "\\System??\\Mass1:");
	Mount("\\Device\\Mass2", "\\System??\\Mass2:");
	Mount("\\Device\\Flash", "\\System??\\sfcx:");

	reader = new INIReader("Mass0:\\launch.ini");
	if(reader->ParseError() < 0) reader = new INIReader("Mass1:\\launch.ini");
	if(reader->ParseError() < 0) reader = new INIReader("Mass2:\\launch.ini");
	if(reader->ParseError() < 0) reader= new INIReader("Hdd:\\launch.ini");
	if(reader->ParseError() < 0) reader= new INIReader("sfcx:\\launch.ini");
	if(reader->ParseError() < 0)
	{
		cprintf("[BladesDL] [LoadPlugins] launch.ini not found");
		return FALSE;
	}

	// We've got 
	string temp = reader->Get("Plugins", "plugin1", "NOTFOUND");
	if(temp!="NOTFOUND" && temp!="none" && temp != ""){
		cprintf("[BladesDL] [LoadPlugins] plugin1: %s", temp.c_str());
		if(XexLoadImage(temp.c_str(),8,0,NULL))
			cprintf("[BladesDL] [LoadPlugins] ERROR: Failed to load %s", temp.c_str());
	}
	temp = reader->Get("Plugins", "plugin2", "NOTFOUND");
	if(temp!="NOTFOUND" && temp!="none" && temp != ""){
		cprintf("[BladesDL] [LoadPlugins] plugin2: %s", temp.c_str());
		if(XexLoadImage(temp.c_str(),8,0,NULL))
			cprintf("[BladesDL] [LoadPlugins] ERROR: Failed to load %s", temp.c_str());
	}
	temp = reader->Get("Plugins", "plugin3", "NOTFOUND");
	if(temp!="NOTFOUND" && temp!="none" && temp != ""){
		cprintf("[BladesDL] [LoadPlugins] plugin3: %s", temp.c_str());
		if(XexLoadImage(temp.c_str(),8,0,NULL))
			cprintf("[BladesDL] [LoadPlugins] ERROR: Failed to load %s", temp.c_str());
	}
	temp = reader->Get("Plugins", "plugin4", "NOTFOUND");
	if(temp!="NOTFOUND" && temp!="none" && temp != ""){
		cprintf("[BladesDL] [LoadPlugins] plugin4: %s", temp.c_str());
		if(XexLoadImage(temp.c_str(),8,0,NULL))
			cprintf("[BladesDL] [LoadPlugins] ERROR: Failed to load %s", temp.c_str());
	}
	temp = reader->Get("Plugins", "plugin5", "NOTFOUND");
	if(temp!="NOTFOUND" && temp!="none" && temp != ""){
		cprintf("[BladesDL] [LoadPlugins] plugin5: %s", temp.c_str());
		if(XexLoadImage(temp.c_str(),8,0,NULL))
			cprintf("[BladesDL] [LoadPlugins] ERROR: Failed to load %s", temp.c_str());
	}

	cprintf("[BladesDL] [LoadPlugins] complete!");
	return TRUE;
}

BOOL WINAPI DllMain(HANDLE hInstDLL, DWORD fdwReason, LPVOID lpReserved)
{
	dllHandle = hInstDLL;
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		cprintf("\n[BladesDL] Loaded!");
		cprintf("[BladesDL] Detected Kernel Version: %d", XboxKrnlVersion->Build);

		SetupDNSHook();
		SetupMemoryProtectionToggleHook();
		SetupHeaderVerificationToggleHook();
		SetupkeBugCheckExHook();
		SetupXamCheckExecPrivHook();
		PatchUpdStrings();
		ApplyPingPatch();
		ApplyContentPatch();

		LoadPlugins();

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