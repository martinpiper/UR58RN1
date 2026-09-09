/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef _RN_MINMAX_H_
#define _RN_MINMAX_H_

#ifndef min
#define min(a,b)			(((a) < (b)) ? (a) : (b))
#endif

#ifndef max
#define max(a,b)			(((a) > (b)) ? (a) : (b))
#endif

#endif
