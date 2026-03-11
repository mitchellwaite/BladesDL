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
#define XAM_CONTENT_EVAL_LIC_ADDR_6717    
#define XAM_CONTENT_EVAL_LIC_ADDR_6770    

// XContent::ContentEvaluateLicense (set arcade licensed bit)
#define XAM_CONTENT_EVAL_LIC_UNLOCK_ADDR_1888	   0x818C9898
#define XAM_CONTENT_EVAL_LIC_UNLOCK_ADDR_6717
#define XAM_CONTENT_EVAL_LIC_UNLOCK_ADDR_6770

// XContent::GetLicenseMask
#define XCONTENT_GET_LIC_MASK_ADDR_1888   0x818C9DD0
#define XCONTENT_GET_LIC_MASK_ADDR_6717   0x818D9328
#define XCONTENT_GET_LIC_MASK_ADDR_6770   0x818C9328
#endif // _PATCHES_H
