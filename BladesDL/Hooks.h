#pragma once
#include "stdafx.h"

#ifndef _HOOKS_H
#define _HOOKS_H

VOID SetupMemoryProtectionToggleHook();
VOID SetupDNSHook();
VOID SetupXamCheckExecPrivHook();
VOID SetupkeBugCheckExHook();
VOID SetupHeaderVerificationToggleHook();

// Strings
#define XBOX_XEX		"\\Device\\Harddisk0\\SystemPartition\\Compatibility"
#define DASH_XEX		"\\SystemRoot\\dash.xex"

// Function Addresses
#define XAM_LOADERPREP_ADDR_6770			      0x818DA950 // TitleLoaderPrepareLoadExecutableFile
#define XAM_LOADERPREP_ADDR_6717             0x818EAAE8
#define KERNEL_XEXP_LOAD_IMAGE_ADDR_1888	   0x80065948 // XexpLoadImage

#define KERNEL_XEXP_VERIFY_HEADER_ADDR_1888        0x80063460 // XexpVerifyImageHeaders
#define KERNEL_XEXP_VERIFY_HEADER_ADDR_6717_6770   0x80076BA8 // XexpVerifyXexHeaders, same address in 6717 and 6770

// Ordinals
#define keBugCheckEx_ORD			83
#define XexCheckExecPriv_ORD		404 // 0x194
#define NetDll_XNetDnsLookup_ORD	67

// Hook IDs
#define MEM_PROT_TOGGLE_VAL		1
#define DNSLOOKUPSAVE_VAL		2
#define KEBUGCHECKEXSAVE_VAL	3
#define XEXP_VERIFY_HEADER_VAL	4

#endif // _HOOKS_H
