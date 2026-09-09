/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef __SIMPLESCRIPT_H__
#define __SIMPLESCRIPT_H__
#include <set>

#include "RNSimpleScript/Inc/SimpleScriptMemory.h"
#include "RNSimpleScript/Inc/SimpleScriptRegister.h"
#include "RNSimpleScript/Inc/SimpleScriptThread.h"
class SimpleScriptMemory;
class SimpleScriptRegister;
class SimpleScriptThread;
class SimpleScriptCallBack;


/**
 * A simple basic like scripting language.
 */
class SimpleScript
{
public:
	SimpleScript();
	virtual ~SimpleScript();

	SimpleScriptMemory *LoadProgram(const char *filename);

	/**
	 * All threads allocated by this class will be freed when this class is freed.
	 */
	SimpleScriptThread *AllocateThread(SimpleScriptMemory *const memory);

	void SetCallBack(SimpleScriptCallBack *const callBack = 0);

	SimpleScriptCallBack *GetCallBack(void) const;

	void SetMemory(SimpleScriptMemory *const memory);

	SimpleScriptMemory *GetMemory(void) const;

	void AddThread(SimpleScriptThread *const thread);

	/// Runs each thread for a maximum of the specified number of cycles.
	/// param forCycles The maximum number of cycles to run each thread for.
	/// \return The number of threads running.
	int RunThreads(int forCycles = 1);
	SimpleScriptThread *GetThreadFromID(const int threadID) const;
	void RemoveThread(const int threadID);
	void RemoveAndDeleteThread(const int threadID);

	void SetSignalForThreads(const unsigned int signal);
	void ClearSignalForThreads(const unsigned int signal);

	int GetKnownThreadIDs(int *const array,const int maxEntries);

	int GetTotalCycles(void) const
	{
		return mTotalCycles;
	}

private:
	void AddThreadToTidy(SimpleScriptThread *const thread);

	SimpleScriptCallBack *mCallBack;
	SimpleScriptMemory *mMemory;

	SimpleScriptThread **mThreads;
	int mThreadArraySize;
	SimpleScriptThread **mThreadsToTidy;
	int mThreadArraySizeToTidy;

	int mThreadID;

	int mTotalCycles;
protected:
friend class SimpleScriptThread;
	std::set<SimpleScriptThread *> mAllocatedThreads;
	std::set<SimpleScriptThread *> mRemoved;
};

/**
 * A callback class
 */
class SimpleScriptCallBack
{
public:
	SimpleScriptCallBack() {};
	virtual ~SimpleScriptCallBack() {};

	virtual bool MemoryException(SimpleScriptThread *const /*thread*/)
	{
		return false;
	}

	/**
	 * Returning true will not execute any further instructions during this context switch.
	 */
	virtual bool ThreadExternalCall(SimpleScriptThread *const /*thread*/,const int /*value*/)
	{
		return false;
	}

	virtual bool ThreadException(SimpleScriptThread *const /*thread*/)
	{
		return false;
	}

	virtual bool ThreadTerminated(SimpleScriptThread *const /*thread*/)
	{
		return false;
	}
};

#endif
