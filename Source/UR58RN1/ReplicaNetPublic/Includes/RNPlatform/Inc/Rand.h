/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef _RAND_H_
#define _RAND_H_

namespace RNReplicaNet
{

namespace Rand
{

/// Weak pseudo random generation, Mersenne twister. Similar to rand() this uses RAND_MAX
void SeedFastWeak(const unsigned int seed);
/// Returns a fast weak pseudo random number. If SeedFastWeak() has not been called then SeedFastWeak() will be used, once only, to initialise the generator.
int FastWeak(void);

} //< namespace Rand

} //< namespace RNReplicaNet

#endif //< #ifndef _RAND_H_
