#pragma once
#include "stdafx.h"

#ifndef _PATCHES_H
#define _PATCHES_H

VOID PatchUpdStrings(VOID);
VOID ApplyPingPatch(VOID);
VOID ApplyContentPatch(VOID);

#define PING_PATCH_ADDR_6770 0x81947D38
#define PING_PATCH_ADDR_6717 0x81957A10
#define PING_PATCH_ADDR_1888 0x819B9048

// XamContentGetLicenseMask
#define XAM_CONTENT_GET_LIC_MASK_ORD	0x266

// XContent::ContentEvaluateLicense return value (mr r3, r28)
#define XAM_CONTENT_EVAL_LIC_ADDR_1888	   0x818C98E0
#define XAM_CONTENT_EVAL_LIC_ADDR_6717    0x818D9068
#define XAM_CONTENT_EVAL_LIC_ADDR_6770    0x818C9068

// XContent::ContentEvaluateLicense (set arcade licensed bit, replaces cmpldi cr6, r11, 0)
#define XAM_CONTENT_EVAL_LIC_UNLOCK_ADDR_1888	   0x818C9898
#define XAM_CONTENT_EVAL_LIC_UNLOCK_ADDR_6717      0x818D901C
#define XAM_CONTENT_EVAL_LIC_UNLOCK_ADDR_6770      0x818C901C

// Patch nop in XContent::EvaluateContent to ignore result of XContent::DeviceGetSerialNumber (or DeviceGetInfo in 1888)
#define XAM_EVAL_CONTENT_SERIAL_NUMBER_ADDR_1888   0x818C9990
#define XAM_EVAL_CONTENT_SERIAL_NUMBER_ADDR_6717   0x818D9134
#define XAM_EVAL_CONTENT_SERIAL_NUMBER_ADDR_6770   0x818C9134

// Patch XContent::VerifyLicensee to always return true
#define XAM_XCONTENT_VERIFY_LICENSEE_ADDR_1888     0x818C9788
#define XAM_XCONTENT_VERIFY_LICENSEE_ADDR_6717     0x818D8EA0
#define XAM_XCONTENT_VERIFY_LICENSEE_ADDR_6770     0x818C8EA0

// Patch XContent::VerifySignature to return true upon failure
#define XAM_XCONTNET_VERIFY_SIGNATURE_ADDR_1888    0x818C7AFC
#define XAM_XCONTNET_VERIFY_SIGNATURE_ADDR_6717    0x818D687C
#define XAM_XCONTNET_VERIFY_SIGNATURE_ADDR_6770    0x818C687C

// XContent::GetLicenseMask
#define XCONTENT_GET_LIC_MASK_ADDR_1888   0x818C9DD0
#define XCONTENT_GET_LIC_MASK_ADDR_6717   0x818D9328
#define XCONTENT_GET_LIC_MASK_ADDR_6770   0x818C9328
#endif // _PATCHES_H
