/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef __CTHREADCLASS_H__
#define __CTHREADCLASS_H__

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * This is a replacement for sleep() (unix) and Sleep() (windows)
 * \param milliseconds the time to sleep in milliseconds
 */
extern void CurrentThreadSleep(int milliseconds);

typedef void tReplicaNet_SleepHookFunc(int);
/**
 * This enables the base level sleep function to be altered by a hook to an external function.
 * Internally this is a point where the thread can be cancelled. There must not be any mutex object with a current lock when Sleep() is called otherwise a deadlock situation may develop.
 * \param sleepFunction the pointer to the sleep function to use, or if NULL then the default sleep function for the platform is used.
 */
extern void CurrentThreadSleep_SetHook(tReplicaNet_SleepHookFunc *sleepFunction);

#ifdef __cplusplus
}
#endif

#endif
