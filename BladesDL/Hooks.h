#pragma once
#include "stdafx.h"

#ifndef _HOOKS_H
#define _HOOKS_H

VOID SetupLoaderPrepHook();
VOID SetupDNSHook();
VOID SetupXamCheckExecPrivHook();
VOID SetupkeBugCheckExHook();
VOID SetupLoadImageHook();

#endif // _HOOKS_H
