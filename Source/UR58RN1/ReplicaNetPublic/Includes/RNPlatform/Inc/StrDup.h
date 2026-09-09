/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef _RN_STRDUP_H_
#define _RN_STRDUP_H_

namespace RNReplicaNet
{

/// Acts likes strdup and used by ReplicaNet to ensure allocated strings use a version of malloc that uses MemoryTracking.h
extern char *StrDup(const char *text);

// For Windows builds that redefine the token (sigh) StrDup to be StrDupA
extern char *StrDupA(const char *text);

} // namespace RNReplicaNet

#endif //#ifndef _RN_STRDUP_H_
