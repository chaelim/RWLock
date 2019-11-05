// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

// System includes

#define NTDDI_VERSION _WIN32_WINNT_VISTA
#include <SDKDDKVer.h>

#define STRICT
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winbase.h>
#include <intrin.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <crtdbg.h>

// STL headers
#include <list>

// Project includes
#include "Random\randomc.h"
#include <Common.h>
#include <RWLock.h>
#include <RWLock2.h>

