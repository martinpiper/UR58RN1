/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef __THREAD_H__
#define __THREAD_H__

#include <stddef.h>	// For size_t
#if defined(RN_UNIX_LIKE)
#include <pthread.h>
#endif

namespace RNReplicaNet
{

class ThreadClass;
const int kReplicaNetPreferredProcessorOSChoice = -1;
/**
 * This Thread class handles creating a class that can be automatically threaded.
 * A threaded class inherits methods from ThreadClass.
 */
class Thread
{
public:
	/**
	 * The ctor performs some basic initialisation
	 */
	Thread();

	/**
	 * The dtor makes sure the thread is terminated. NOTE: If this class and the ThreadClass are part of the same class hierarchy then Terminate() must be explicitly called and not rely on this destructor.
	 */
	virtual ~Thread();

	/**
	 * This begins the thread in the specified ThreadClass class. This resets the GetTerminated() flag to false but does not reset GetEverTerminated().
	 * Execution begins from ThreadClass::ThreadEntry()
	 * \param threaded_class the pointer to the class that inherits from ThreadClass
	 */
	void Begin(ThreadClass *threaded_class);

	/**
	 * Terminates the thread of the class bound to this Thread. The caller must not be in the context of the running thread.
	 */
	void Terminate(void);

	/**
	 * Returns true if Terminate() was used after the last Begin().
	 */
	bool GetTerminated(void);

	/**
	 * Returns true if Terminate() was used at all and is not reset by Begin().
	 */
	bool GetEverTerminated(void);

	/**
	 * This is a static method where the class starts its thread from
	 * \param thread_class the threaded class pointer
	 */
	static int CallThreadEntry(ThreadClass *thread_class);

	/**
	 * Gets the execution status of the thread
	 * \return return true if the thread is running
	 */
	bool GetIsRunning(void) const;

	/**
	 * Sets the preferred processor this thread will execute on. This function is OS dependant.
	 * \param processor The preferred processor. The default value is kReplicaNetPreferredProcessorOSChoice which allows the OS to choose a processor.
	 */
	void SetPreferredProcessor(int processor = kReplicaNetPreferredProcessorOSChoice);

	/**
	 * Gets the current processor number being used from the context of the calling thread, not the running context of this class since this is a static member.
	 */
	static int GetCurrentProcessorNumber(void);

	/**
	 * Returns the value set by SetPreferredProcessor().
	 */
	int GetPreferredProcessor(void);

	static size_t GetNumAllocated(void)
	{
		return mNumAllocated;
	}
	static size_t GetNumActive(void)
	{
		return mNumActive;
	}

private:
	friend class ThreadClass;
	ThreadClass *mThreadClass;
#if defined(_WIN32)
	void *mThreadHandle;
//	unsigned long mThreadHandle;
#endif
#if defined(RN_UNIX_LIKE)
	pthread_t mThreadHandle;
#endif
#if defined (_PS2)
	int mThreadHandle;
#endif

	int mProcessor;	// The preferred processor to run this thread on

	volatile bool mIsRunning;
	volatile bool mTerminateCalled;
	volatile bool mTerminateCalledNoReset;

	static size_t mNumAllocated;
	static size_t mNumActive;
};

} // namespace RNReplicaNet

#endif
