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
	else if(XboxKrnlVersion->Build == 6717)
	{
		ptr = (PDWORD)PING_PATCH_ADDR_6717;	
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

//
// Content Patches
//

// patch by mojobojo @ xboxhacker
// XamContentGetLicenseMask
// DLC/Addons will appear to be licensed
VOID ApplyXamContentGetLicMaskPatch()
{
	PDWORD ptr = NULL;

	cprintf("[BladesDL] [contpatch] Patching XamContentGetLicenseMask");

	ptr = (PDWORD)resolveFunct(MODULE_XAM, XAM_CONTENT_GET_LIC_MASK_ORD);
	ptr[0] = 0x3960FFFF; // li %r11, 0xFFFF
	ptr[1] = 0x91630000; // stw %r11, 0(%r3)
	ptr[2] = 0x38600000; // li r3, 0
	ptr[3] = 0x4E800020; // blr
	__dcbst(0, ptr);
	__sync();
	__isync();

	return;
}

// patch within XContent::ContentEvaluateLicense
// XBLA will appear to be fully licensed
VOID ApplyXContentContentEvaluateLicensePatch()
{
	PDWORD ptr_rtn = NULL;
	PDWORD ptr_arc = NULL;

	if(XboxKrnlVersion->Build == 1888)
	{
		ptr_rtn = (PDWORD)XAM_CONTENT_EVAL_LIC_ADDR_1888;
		ptr_arc = (PDWORD)XAM_CONTENT_EVAL_LIC_UNLOCK_ADDR_1888;
	}
	else if(XboxKrnlVersion->Build == 6717)
	{
		ptr_rtn = (PDWORD)XAM_CONTENT_EVAL_LIC_ADDR_6717;
		ptr_arc = (PDWORD)XAM_CONTENT_EVAL_LIC_UNLOCK_ADDR_6717;
	}
	else if(XboxKrnlVersion->Build == 6770)
	{
		ptr_rtn = (PDWORD)XAM_CONTENT_EVAL_LIC_ADDR_6770;
		ptr_arc = (PDWORD)XAM_CONTENT_EVAL_LIC_UNLOCK_ADDR_6770;
	}
	else
	{
		cprintf("[BladesDL] [contpatch] Unsupported kernel, unable to patch XContent::ContentEvaluateLicense");
		return;
	}

	cprintf("[BladesDL] [contpatch] Patching XContent::ContentEvaluateLicense return value");

	ptr_rtn[0] = 0x38600000; // li r3, 0 (ERROR_SUCCESS)

	__dcbst(0, ptr_rtn);
	__sync();
	__isync();

	cprintf("[BladesDL] [contpatch] Patching XContent::ContentEvaluateLicense arcade licensed flag");

	ptr_arc[0] = 0x39600001; // li r11, 1
	ptr_arc[1] = 0x917F0008; // stw r11, 8(r31)

	__dcbst(0, ptr_arc);
	__sync();
	__isync();

	return;
}

// patch within XContent::GetLicenseMask
// XBLA will appear to be fully licensed
VOID ApplyXcontentGetLicenseMaskPatch()
{
	PDWORD ptr = NULL;

	if(XboxKrnlVersion->Build == 1888)
	{
		ptr = (PDWORD)XCONTENT_GET_LIC_MASK_ADDR_1888;
	}
	else if(XboxKrnlVersion->Build == 6717)
	{
		ptr = (PDWORD)XCONTENT_GET_LIC_MASK_ADDR_6717;
	}
	else if(XboxKrnlVersion->Build == 6770)
	{
		ptr = (PDWORD)XCONTENT_GET_LIC_MASK_ADDR_6770;
	}
	else
	{
		cprintf("[BladesDL] [contpatch] Unsupported kernel, unable to patch XContent::GetLicenseMask");
		return;
	}

	cprintf("[BladesDL] [contpatch] Patching XContent::GetLicenseMask");

	ptr[0] = 0x3960FFFF; // li %r11, 0xFFFF
	ptr[1] = 0x91630000; // stw %r11, 0(%r3)
	ptr[2] = 0x38600000; // li r3, 0
	ptr[3] = 0x4E800020; // blr
	__dcbst(0, ptr);
	__sync();
	__isync();
}

// Patch within XContent::EvaluateContent
// Ignore the result from XContent::DeviceGetInfo or XContent::DeviceGetSerialNumber depending on the OS level
VOID ApplyXcontentEvaluateContentPatch()
{
	PDWORD ptr = NULL;

	if(XboxKrnlVersion->Build == 1888)
	{
		ptr = (PDWORD)XAM_EVAL_CONTENT_SERIAL_NUMBER_ADDR_1888;
	}
	else if(XboxKrnlVersion->Build == 6717)
	{
		ptr = (PDWORD)XAM_EVAL_CONTENT_SERIAL_NUMBER_ADDR_6717;
	}
	else if(XboxKrnlVersion->Build == 6770)
	{
		ptr = (PDWORD)XAM_EVAL_CONTENT_SERIAL_NUMBER_ADDR_6770;
	}
	else
	{
		cprintf("[BladesDL] [contpatch] Unsupported kernel, unable to patch XContent::EvaluateContent");
		return;
	}

	cprintf("[BladesDL] [contpatch] Patching XContent::EvaluateContent");

	ptr[0] = 0x60000000; // nop
	__dcbst(0, ptr);
	__sync();
	__isync();
}

// patch out XContent::VerifyLicensee to always return true
VOID ApplyXcontentVerifyLicenseePatch()
{
	PDWORD ptr = NULL;

	if(XboxKrnlVersion->Build == 1888)
	{
		ptr = (PDWORD)XAM_XCONTENT_VERIFY_LICENSEE_ADDR_1888;
	}
	else if(XboxKrnlVersion->Build == 6717)
	{
		ptr = (PDWORD)XAM_XCONTENT_VERIFY_LICENSEE_ADDR_6717;
	}
	else if(XboxKrnlVersion->Build == 6770)
	{
		ptr = (PDWORD)XAM_XCONTENT_VERIFY_LICENSEE_ADDR_6770;
	}
	else
	{
		cprintf("[BladesDL] [contpatch] Unsupported kernel, unable to patch XContent::VerifyLicensee");
		return;
	}

	cprintf("[BladesDL] [contpatch] Patching XContent::VerifyLicensee");

	ptr[0] = 0x38600001; // li r3, 1
	ptr[1] = 0x4E800020; // blr
	__dcbst(0, ptr);
	__sync();
	__isync();
}

// Patch the error path of XContent::VerifySignature to always return "ERROR_SUCCESS"
VOID ApplyXcontentVerifySignaturePatch()
{
	PDWORD ptr = NULL;

	if(XboxKrnlVersion->Build == 1888)
	{
		ptr = (PDWORD)XAM_XCONTNET_VERIFY_SIGNATURE_ADDR_1888;
	}
	else if(XboxKrnlVersion->Build == 6717)
	{
		ptr = (PDWORD)XAM_XCONTNET_VERIFY_SIGNATURE_ADDR_6717;
	}
	else if(XboxKrnlVersion->Build == 6770)
	{
		ptr = (PDWORD)XAM_XCONTNET_VERIFY_SIGNATURE_ADDR_6770;
	}
	else
	{
		cprintf("[BladesDL] [contpatch] Unsupported kernel, unable to patch XContent::VerifySignature");
		return;
	}

	cprintf("[BladesDL] [contpatch] Patching XContent::VerifySignature");

	ptr[0] = 0x38600000; // li r3, 0
	__dcbst(0, ptr);
	__sync();
	__isync();
}

VOID ApplyContentPatch()
{
	// Patch XamContentGetLicenseMask 
	ApplyXamContentGetLicMaskPatch();

	// Patch XContent::ContentEvaluateLicense
	ApplyXContentContentEvaluateLicensePatch();

	// XContent::GetLicenseMask
	ApplyXcontentGetLicenseMaskPatch();

	// Patch within XContent::EvaluateContent to ignore
	// result from XContent::DeviceGetSerialNumber
	ApplyXcontentEvaluateContentPatch();

	// XContent::VerifyLicensee
	ApplyXcontentVerifyLicenseePatch();

	// XContent::VerifySignature
	ApplyXcontentVerifySignaturePatch();

	return;
}