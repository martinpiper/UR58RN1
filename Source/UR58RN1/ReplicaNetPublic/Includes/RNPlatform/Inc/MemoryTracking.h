// Memory tracking APIs have been deprecated.
// This file is still the first header file included by all ReplicaNet code to allow custom memory hooks (new/delete/malloc/free/etc) to be easily added to builds.

// Supported by most modern compilers
#pragma once

// Parse preprocessor platform defines
#include "RNPlatform/Inc/PlatformConfig.h"

#ifdef _WIN32
#ifdef _CRTDBG_MAP_ALLOC

#include <stdlib.h>
#include <crtdbg.h>

#ifndef RNPLATFORM_DO_NOT_DEFINE_DEBUG_NEW
#ifndef DEBUG_NEW
#define DEBUG_NEW new(_NORMAL_BLOCK , __FILE__ , __LINE__)
#define new DEBUG_NEW
#endif //< ifndef DEBUG_NEW
#endif //< ifndef RN_DO_NOT_DEFINE_DEBUG_NEW

#endif //< ifdef _CRTDBG_MAP_ALLOC
#endif //< ifdef _WIN32
