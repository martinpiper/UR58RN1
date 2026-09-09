/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef __PLATFORMHEAP_H__
#define __PLATFORMHEAP_H__

namespace RNReplicaNet
{

/**
 * This base class is used for maintaining blocks of memory
 */
class PlatformHeapBlock
{
public:
	/**
	 * This ctor does nothing exciting
	 */
	PlatformHeapBlock();
	/**
	 * This dtor self removes from PlatformHeap
	 */
	virtual ~PlatformHeapBlock();

	/**
	 * This self registers with PlatformHeap
	 */
	void Register(void);
};

/**
 * This class uses a list of PlatformHeapBlock classes to make sure that all blocks of memory are registered for destruction when PlatformHeap::ForceFree() is called.
 * Each PlatformHeapBlock class is destructed in the reverse order they were added to the stack, it is a LIFO stack. This class can be used to make sure shutdown functions are called and pointers properly cleared.
 */
class PlatformHeap
{
public:
	/**
	 * Deletes all heap blocks on the stack in reverse order. This should only be called when all threads accessing memory registered using this class are stopped.
	 */
	static void ForceFree(void);
};

} // namespace RNReplicaNet

#endif
