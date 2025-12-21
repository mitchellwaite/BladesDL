#include "stdafx.h"

#define VER_MAJ 420
#define VER_MIN 69
#define TARGET_KERNEL (USHORT)(1888)

#define LHELPER_CON	0
#define LHELPER_XEX 1

#define DEFAULT_XEX		"default.xex"
#define MOUNT_NAME		"dlaunch:"
#define LAUNCH_DATA_ID	'BD67'

typedef struct _ldata{
	DWORD ID;
	DWORD ltype;
	char link[MAX_PATH];
	char dev[MAX_PATH];
	USHORT versionMaj;
	USHORT versionMin;
	USHORT targetKernel;
} ldata, *pldata;