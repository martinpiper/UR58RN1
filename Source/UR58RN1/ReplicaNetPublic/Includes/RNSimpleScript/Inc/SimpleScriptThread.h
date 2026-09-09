/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef __SIMPLESCRIPTTHREAD_H__
#define __SIMPLESCRIPTTHREAD_H__

#include <list>
#include "RNSimpleScript/Inc/OpCodeEnums.h"

class SimpleScript;
class SimpleScriptRegister;
class SimpleScriptCallBack;

class SimpleScriptThread
{
public:
	void SetSimpleScript(SimpleScript *const scriptControl);
	SimpleScript *GetSimpleScript(void) const;

	void SetMemory(SimpleScriptMemory *const memory);

	SimpleScriptMemory *GetMemory(void) const;

	/// Runs a number of cycles for this thread.
	/// \return The number of cycles executed this call.
	int RunCycles(const int numberCycles = 1);

	SimpleScriptRegister *GetRegister(const int reg);

	SimpleScriptRegister *GetProgramCounter(void);

	int GetNumberofRegisters(void) const;

	void SetCallBack(SimpleScriptCallBack *const callBack=0);

	SimpleScriptCallBack *GetCallBack(void) const;

	/**
	 * If there are any signal flags then the thread is paused until all flags are clear.
	 */
	void SetSignal(const unsigned int signal);

	unsigned int GetSignal(void) const;

	void AddSignal(const unsigned int signal);

	unsigned int RemoveSignal(const unsigned int signal);

	void SignalIndexAdd(const unsigned int signalIndex);

	void SignalIndexClear(const unsigned int signalIndex);

	void SetThreadID(const int threadID);

	int GetThreadID(void) const;

	bool GetIsActive(void) const;

	void SetIsActive(const bool active = true);

	void RunFromPC(unsigned int address);

	void SetRemoveOnExit(const bool remove = false);

	bool GetRemoveOnExit(void) const;

	void SetDeleteOnRemove(const bool self = false);

	bool GetDeleteOnRemove(void) const;

	bool PushRegister(SimpleScriptRegister &reg);

	bool PopRegister(SimpleScriptRegister &reg);

	void DumpStatus(void);

	int GetCycles(void)
	{
		return mCycles;
	}

private:
	SimpleScript *mControl;

	SimpleScriptMemory *mMemory;

	SimpleScriptRegister mRegisters[kSimpleScriptThreadRegisters];
	SimpleScriptRegister mProgramCounter;

	std::list<SimpleScriptRegister> mStack;

	SimpleScriptCallBack *mCallBack;

	bool mExceptionNotFlag;

	unsigned int mWaitForSignal;

	int mThreadID;

	bool mRemoveOnExit;
	bool mDeleteOnRemove;

	int mCycles;

// Only the SimpleScript controller can allocate or free script threads
friend class SimpleScript;
protected:
	SimpleScriptThread();
	virtual ~SimpleScriptThread();
};

#endif
