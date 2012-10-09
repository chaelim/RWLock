// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

// System includes

// FlushProcessWriteBuffers API supported since Vista
#define NTDDI_VERSION _WIN32_WINNT_VISTA
#include <SDKDDKVer.h>

#define STRICT
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winbase.h>
#include <intrin.h>
#include <stdio.h>
#include <stdint.h>
#include <crtdbg.h>

// Project includes
#include "Common.h"
#include "RWLock.h"
#include "RWLock2.h"