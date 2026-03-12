#include "stdafx.h"

//=============================================================================================================================================
//		Hook used to catch the XB1 emulator load
//		Will toggle the Mem protection on/off to prevent the crash
//=============================================================================================================================================
void toggleMemProtection(char * xex)
{
	if (strncmp(xex, XBOX_XEX, strlen(XBOX_XEX)) == 0)
	{
		//HvxSetState(SET_PROT_ON);
		HvxGetVersions(FREEBOOT_SYSCALL_KEY, SET_PROT_ON);
		g_Protection = PROTECT_ON;
		__dcbst(0, &g_Protection);
		__sync();
	}
	else if (strcmp(xex, DASH_XEX) == 0)
	{
		if (g_Protection)
		{
			//HvxSetState(SET_PROT_OFF);
			HvxGetVersions(FREEBOOT_SYSCALL_KEY, SET_PROT_OFF);
			g_Protection = PROTECT_OFF;
			__dcbst(0, &g_Protection);
			__sync();
		}
	}
}

void toggleFallbackXexKeyOnFailure()
{
	// To be called by the XexpLoadImage and XexpVerifyHeaders hooks to toggle the
	// fallback xex key (when XeCryptBnQwBeSigVerify in the HV returns 0)

	// 38 80 00 54 - li %r4, 0x54
	// 38 80 00 F0 - li %r4, 0xF0
	// My ghetto set of FreeBoot patches puts the fallback li at 0x1620 in HV space
	// It should always be one of those two hex values
	uint32_t li_key_inst = 0;

	int hv_addr = 0;

	if(XboxKrnlVersion->Build == 1888)
	{
		hv_addr = 0x162C;
	}
	else if(XboxKrnlVersion->Build == 6717 || XboxKrnlVersion->Build == 6770)
	{
		// TODO I don't have the patch set for 6770 handy, so i'm assuming
		// the same patch set is in use because the code in the HV is identical
		hv_addr = 0x3DD8;
	}
	else
	{
		// We don't know what address to toggle, yolo
		return;
	}

	ReadHypervisor(&li_key_inst, hv_addr, 0x4);

	if(li_key_inst == 0x38800054)
	{
		// We failed to load the xex with the retail key as fallback.
		// try again with the devkit key
		li_key_inst = 0x388000F0;
		WriteHypervisor(&li_key_inst, hv_addr, 0x4);
	}
	else if(li_key_inst == 0x388000F0)
	{
		// We failed to load the xex with the devkit key as fallback.
		// try again with the retail key
		li_key_inst = 0x38800054;
		WriteHypervisor(&li_key_inst, hv_addr, 0x4);
	}
}

#pragma region OGXFix
VOID __declspec(naked) MemProtToggleSaveVar(VOID)
{
	__asm{
		li r3, MEM_PROT_TOGGLE_VAL
		nop
		nop
		nop
		nop
		nop
		nop
		blr
	}
}

typedef NTSTATUS (*XEXPLOADIMAGEFUN)(LPCSTR xexName, DWORD typeInfo, DWORD ver, PHANDLE modHandle); // XexpLoadImage
XEXPLOADIMAGEFUN XexpLoadImageSave = (XEXPLOADIMAGEFUN)MemProtToggleSaveVar;

NTSTATUS XexpLoadImageHook(LPCSTR xex, DWORD typeInfo, DWORD ver, PHANDLE modHandle)
{
	toggleMemProtection((char *)xex);

	NTSTATUS ret = XexpLoadImageSave(xex, typeInfo, ver, modHandle);

	if( !NT_SUCCESS(ret) )
	{
		toggleFallbackXexKeyOnFailure();
		return XexpLoadImageSave(xex, typeInfo, ver, modHandle);
	}

	return ret;
}

VOID SetupMemoryProtectionToggleHook()
{
	cprintf("[BladesDL] [HOOK] Applying XexpLoadImage Hook...");

	// using this to catch dash.xex and xbox emu loading
	if(XboxKrnlVersion->Build == 1888)
	{
		hookFunctionStart((PDWORD)KERNEL_XEXP_LOAD_IMAGE_ADDR_1888, (PDWORD)XexpLoadImageSave, (DWORD)XexpLoadImageHook);
	}
	else if(XboxKrnlVersion->Build == 6770)
	{
		hookFunctionStart((PDWORD)KERNEL_XEXP_LOAD_IMAGE_ADDR_6770, (PDWORD)XexpLoadImageSave, (DWORD)XexpLoadImageHook);
	}
	else if(XboxKrnlVersion->Build == 6717)
	{
		hookFunctionStart((PDWORD)KERNEL_XEXP_LOAD_IMAGE_ADDR_6717, (PDWORD)XexpLoadImageSave, (DWORD)XexpLoadImageHook);
	}
	else
	{
		cprintf("[BladesDL] [HOOK] Unsupported kernel: %d, skipping memory protection toggle hook", XboxKrnlVersion->Build);
	}
}
#pragma endregion
//=============================================================================================================================================

#pragma region XexpVerifyImageHeaderToggle
VOID __declspec(naked) XexpVerifyImageHeadersSaveVar(VOID)
{
	__asm{
		li r3, XEXP_VERIFY_HEADER_VAL
		nop
		nop
		nop
		nop
		nop
		nop
		blr
	}
}

typedef NTSTATUS (*XEXPVERIFYIMAGEHEADERSFUN)(PHANDLE modHandle); // XexpVerifyImageHeaders
XEXPVERIFYIMAGEHEADERSFUN XexpVerifyImageHeadersSave = (XEXPVERIFYIMAGEHEADERSFUN)XexpVerifyImageHeadersSaveVar;

NTSTATUS XexpVerifyImageHeadersHook(PHANDLE modHandle)
{
	NTSTATUS ret = XexpVerifyImageHeadersSave(modHandle);

	if( !NT_SUCCESS(ret) )
	{
		toggleFallbackXexKeyOnFailure();

		return XexpVerifyImageHeadersSave(modHandle);
	}

	return ret;
}

typedef NTSTATUS (*XEXPVERIFYXEXHEADERSFUN)(QWORD param_1, QWORD param_2, PDWORD param_3); // XexpVerifyXexHeaders
XEXPVERIFYXEXHEADERSFUN XexpVerifyXexHeadersSave = (XEXPVERIFYXEXHEADERSFUN)XexpVerifyImageHeadersSaveVar;

NTSTATUS XexpVerifyXexHeadersHook(QWORD param_1, QWORD param_2, PDWORD param_3)
{
	NTSTATUS ret = XexpVerifyXexHeadersSave(param_1, param_2, param_3);

	if( !NT_SUCCESS(ret) )
	{
		toggleFallbackXexKeyOnFailure();

		return XexpVerifyXexHeadersSave(param_1, param_2, param_3);
	}

	return ret;
}

VOID SetupHeaderVerificationToggleHook()
{
	// using this to catch dash.xex and xbox emu loading
	if(XboxKrnlVersion->Build == 1888)
	{
		cprintf("[BladesDL] [HOOK] Applying XexpVerifyImageHeaders Hook...");
		hookFunctionStart((PDWORD)KERNEL_XEXP_VERIFY_HEADER_ADDR_1888, (PDWORD)XexpVerifyImageHeadersSave, (DWORD)XexpVerifyImageHeadersHook);
	}
	else if(XboxKrnlVersion->Build == 6717 || XboxKrnlVersion->Build == 6770)
	{
		cprintf("[BladesDL] [HOOK] Applying XexpVerifyXexHeaders Hook...");
		hookFunctionStart((PDWORD)KERNEL_XEXP_VERIFY_HEADER_ADDR_6717_6770, (PDWORD)XexpVerifyXexHeadersSave, (DWORD)XexpVerifyXexHeadersHook);
	}
	else
	{
		cprintf("[BladesDL] [HOOK] Unsupported kernel: %d, skipping header verification hook", XboxKrnlVersion->Build);
	}
}
#pragma endregion


//=============================================================================================================================================
//		LIVEBLOCK
//		NetDll_XNetDnsLookup Hook
//=============================================================================================================================================
#pragma region LiveBlock
typedef DWORD(*DNSLOOKUPSAVEFUN)(XNCALLER_TYPE xnc, const char* pszHost, WSAEVENT hEvent, XNDNS** ppxndns);
VOID __declspec(naked) DnsLookupSaveVar(VOID)
{
	__asm {
		li r3, DNSLOOKUPSAVE_VAL
		nop
		nop
		nop
		nop
		nop
		nop
		blr
	}
}
DNSLOOKUPSAVEFUN DnsLookupSave = (DNSLOOKUPSAVEFUN)DnsLookupSaveVar;

NTSTATUS DnsLookupHook(XNCALLER_TYPE xnc, const char* pszHost, WSAEVENT hEvent, XNDNS** ppxndns)
{
	const char* LiveBlockList[] = {
	"xemacs.xboxlive.com",
	"xeas.xboxlive.com",
	"xetgs.xboxlive.com",
	"xexds.xboxlive.com",
	"piflc.xboxlive.com",
	"siflc.xboxlive.com",
	"msac.xboxlive.com",
	"xlink.xboxlive.com",
	"xuacs.xboxlive.com",
	"sts.xboxlive.com",
	"xam.xboxlive.com",
	"notice.xbox.com",
	"macs.xbox.com",
	"rad.msn.com"
	};

	for (int i = 0; i < ARRAYSIZE(LiveBlockList); i++) {
		if (strcmpi(pszHost, LiveBlockList[i]) == 0)
		{
			cprintf("[BladesDL] [LIVEBLOCK] Lookup address %s is on our 'LiveBlock' block list!", LiveBlockList[i]);
			return DnsLookupSave(xnc, "live.block\0", hEvent, ppxndns);
		}

	}

	return DnsLookupSave(xnc, pszHost, hEvent, ppxndns);
}

VOID SetupDNSHook()
{
	cprintf("[BladesDL] [HOOK] Applying LiveBlock Hook...");
	hookFunctionStartOrd(MODULE_XAM, NetDll_XNetDnsLookup_ORD, (PDWORD)DnsLookupSave, (DWORD)DnsLookupHook);
}
#pragma endregion
//=============================================================================================================================================

//=============================================================================================================================================
//		SOCKPATCH
//		Always allows executables to use insecure sockets
//=============================================================================================================================================
#pragma region Sockpatch
BOOL XamCheckExecPriv(DWORD priv)
{
	BOOL ret = XexCheckExecutablePrivilege(priv);

	if (priv == PRIV_INSECURE_SOCKS)
		ret = TRUE;

	return ret;
}

VOID SetupXamCheckExecPrivHook()
{
	cprintf("[BladesDL] [HOOK] Applying CheckExecPriv Hook...");
	hookImpStub(MODULE_XAM, MODULE_KERNEL, XexCheckExecPriv_ORD, (DWORD)XamCheckExecPriv);
}
#pragma endregion
//=============================================================================================================================================

//=============================================================================================================================================
//		FATALREBOOT
//		Reboot on crashes instead of freeze - UNTESTED
//=============================================================================================================================================
#pragma region FatalReboot
typedef VOID(*KEBUGCHECKEXFUN)(DWORD r3, DWORD r4, DWORD r5, DWORD r6, DWORD r7);

#define HAL_HARD_REBOOT			2
#define HAL_SOFT_POWEROFF		5
#define HAL_SOFT_REBOOT			6 // many jtag's won't work with this one...

VOID __declspec(naked) keBugCheckExSaveVar(VOID)
{
	__asm {
		li r3, KEBUGCHECKEXSAVE_VAL
		nop
		nop
		nop
		nop
		nop
		nop
		blr
	}
}
KEBUGCHECKEXFUN keBugCheckExSave = (KEBUGCHECKEXFUN)keBugCheckExSaveVar;
// OPT_FATAL_NOFREEZE bit is set when freeze is avoided
// OPT_FATAL_REBOOT when above is set, this will cause the box to reboot instead of shutoff
VOID keBugCheckExHook(DWORD r3, DWORD r4, DWORD r5, DWORD r6, DWORD r7)
{
	cprintf("\n*** Fatal System Error: 0x%08lx\n                       (0x%p,0x%p,0x%p,0x%p)\n\n", r3, r4, r5, r6, r7);
	//if (getOpt(OPT_FATAL_NOFREEZE))
	//{
	//	if (getOpt(OPT_FATAL_REBOOT))
	//		HalReturnToFirmware(HAL_HARD_REBOOT);
	//	else
	//		HalReturnToFirmware(HAL_SOFT_POWEROFF);
	//}
	//else
		//keBugCheckExSave(r3, r4, r5, r6, r7);
	HalReturnToFirmware(HAL_HARD_REBOOT);
}

VOID SetupkeBugCheckExHook()
{
	cprintf("[BladesDL] [HOOK] Applying FatalReboot Hook...");
	hookFunctionStartOrd(MODULE_KERNEL, keBugCheckEx_ORD, (PDWORD)keBugCheckExSave, (DWORD)keBugCheckExHook);
}
#pragma endregion
//=============================================================================================================================================