/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef __THREADCLASS_H__
#define __THREADCLASS_H__
#if defined(_PS2)
#include <list>
#endif

#if defined(_XBOX)
#include <xtl.h>			// For XBox builds
#else
#if defined(_WIN32)
#include <windows.h>		// For Windows builds
#endif
#endif

#if defined(RN_UNIX_LIKE)
#include <pthread.h>
#endif

namespace RNReplicaNet
{

class Thread;
const unsigned int kTIME_INFINITE = 0xFFFFFFFF;

/**
 * Creates a mutex object and controls access to the class and related derived classes
 */
class MutexClass
{
public:
	/**
	 * The ctor performs some basic initialisation.
	 */
	MutexClass();

	/**
	 * The dtor makes sure everything is tidy
	 */
	virtual ~MutexClass();

	/**
	 * Tries to lock access to this class. The lock with block until it is able to succeed
	 */
	void Lock(void);

	/**
	 * Unlocks access to this class
	 */
	void UnLock(void);

	enum
	{
		kNoOwnerThread = -1
	};

private:
#if defined(_PS2)
	// A binary semaphore created to lock all critical sections inside any mutex class
	// A mutex per class could be used however the PS2 does not allow many to be created
	static int mMutex;
	// The count of the lock, for recursive locks
	int mCount;
	// The owner thread of this mutex, kNoOwnerThread means no thread currently owns the mutex and the mCount should then be 0
	volatile int mOwnerThread;
	// A linked list of waiting threads for this mutex
	std::list<int> mWaitingThreadIDs;
#else
	void *mMutex;	// A pointer to a mutex/critical section/handle depending on the internal implementation.

#if defined(_WIN32)
	CRITICAL_SECTION mSection;	// Local storage
#endif

#if defined(RN_UNIX_LIKE)
	pthread_mutex_t mLocalMutex;
#endif

#endif
};

/**
 * This is a class that the thread manager can thread with.
 * Entry for a thread starts at ThreadEntry.
 */
class ThreadClass : public MutexClass
{
public:
	/**
	 * The ctor performs some basic initialisation
	 */
	ThreadClass();

	/**
	 * The dtor makes sure everything is tidy
	 */
	virtual ~ThreadClass();

	/**
	 * Sleeps the thread for the class by the number of milliseconds supplied. This uses the CurrentThreadSleep() function.
	 * Internally this is a point where the thread can be cancelled by Thread::Terminate(). The thread must not have any mutex object with a current lock when Sleep() is called otherwise a deadlock situation may develop.
	 * Memory should not be allocated on the heap at this point unless it can be freed by ThreadExiting() or the class dtor.
	 * \param milliseconds the sleep time in milliseconds
	 */
	void Sleep(int milliseconds);

	int GetThreadID(void);

protected:
friend class Thread;
	/**
	 * This exits the thread and can only be used in the context of the thread.
	 * \param returnCode The return code.
	 */
	void DoQuitNow(const int returnCode);

	/**
	 * The thread starts executing from here.
	 */
	virtual int ThreadEntry(void) = 0;

	/**
	 * This is called just before the thread exits, from the context of the thread.
	 */
	virtual void ThreadExiting(const int returnCode);

	/**
	 * If Thread::Terminate() has been called then using this function will exit the thread.
	 */
	void CheckQuit(void);

	static size_t GetNumAllocated(void)
	{
		return mNumAllocated;
	}

private:
	void CallThreadExiting(const int returnCode);

	volatile bool mQuitNow;

	Thread *mBoundThread;
	volatile bool mThreadExitingCalled;

	static volatile size_t mNumAllocated;

	int mThreadID;	// Not the same as any OS ID
};

/**
 * A simple class that handles locking of a class via the MutexClass
 */
class LockingMechanism
{
public:
	/**
	 * This begins the lock of the class
	 * \param lockee A pointer to the class to lock, that inherits from MutexClass
	 */
	LockingMechanism(MutexClass *lockee);

	/**
	 * The dtor unlocks the class automatically
	 */
	virtual ~LockingMechanism();

private:
	MutexClass *mLockee;
};

class CurrentThread
{
public:
	/**
	 * Sleeps the current thread for the class by the number of milliseconds supplied. This uses the CurrentThreadSleep() function.
	 * Internally this is a point where the thread can be cancelled. There must not be any mutex object with a current lock when Sleep() is called otherwise a deadlock situation may develop.
	 * \param milliseconds the sleep time in milliseconds
	 */
	static void Sleep(int milliseconds);
};

/**
 * This define is a quick and easy method to lock a class because when the local variable is destroyed it goes out of scope the class is automatically unlocked
 */
#define THREADSAFELOCK() RNReplicaNet::LockingMechanism _lock(this);
#define THREADSAFELOCKCLASS(x) RNReplicaNet::LockingMechanism _class_lock(&(x));
#define THREADSAFELOCKCLASSp(x) RNReplicaNet::LockingMechanism _class_lockp(x);
#define THREADSAFELOCKCLASSNAMED(x) RNReplicaNet::LockingMechanism _class_lock##x (&(x));
#define THREADSAFELOCKCLASSNAMEDp(x) RNReplicaNet::LockingMechanism _class_lockp##x (x);

} // namespace RNReplicaNet

#endif
