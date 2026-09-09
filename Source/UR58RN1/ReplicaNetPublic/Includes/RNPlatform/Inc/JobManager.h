/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef _RNPLATFORM_JOBMANAGER_H_
#define _RNPLATFORM_JOBMANAGER_H_

#include <stddef.h>	// For size_t
#include <vector>
#include <list>
#include <map>
#include <set>
#include "RNPlatform/Inc/ThreadClass.h"
#include "RNPlatform/Inc/Thread.h"
#include "RNPlatform/Inc/PlatformInfo.h"

namespace RNReplicaNet
{

class Job;
class Worker;

/**
 * This JobManager class handles pool of threads that queue and run jobs.
 */
class JobManager : public ThreadClass , public Thread
{
friend class Worker;
friend class Job;
public:
	/**
	 * The ctor performs some basic initialisation
	 */
	JobManager();

	/**
	 * The dtor makes sure the entire job queue is terminated.
	 */
	virtual ~JobManager();

	void SetPoolSize(const size_t poolSize = PlatformInfo::GetNumberOfCores());

	bool Start(void);

	bool Stop(void);

	/**
	 * Queues a jobs to run. If PlatformInfo::IsThreaded() returns false then AddPoolJob() will immediately run the job and return.
	 * Once the Job finishes running then it is automatically deleted. Before Job::Run() finishes the user should call out to save important Job data, or communicate the results of a calculation.
	 */
	bool AddPoolJob(Job *job);

	int CancelJobs(void* const job);

	volatile int GetNumberOfJobsRemaining(void);

private:
	int ThreadEntry(void);
	void ThreadExiting(const int returnCode);
	void Tidy(void);

	std::list<Job*> mJobsAdded;
//	std::list<Job*> mJobsRunning;
	std::multimap<void*, Job*> mJobReference;

	std::vector<Worker> mWorkers;

	volatile int mNumJobsRemaining;
};

class Worker : public ThreadClass , public Thread
{
friend class JobManager;
public:
	/**
	 * The ctor performs some basic initialisation
	 */
	Worker();

	/**
	 * The dtor makes sure worker finishes.
	 */
	virtual ~Worker();

	int ThreadEntry(void);
	void ThreadExiting(const int returnCode);

private:
	JobManager *mParent;
	Job *volatile mFromIdle;
};

class Job
{
friend class JobManager;
friend class Worker;
public:
	/**
	 * The ctor performs some basic initialisation
	 */
	Job();

	/**
	 * The dtor makes sure the entire job queue is terminated.
	 */
	virtual ~Job();

	virtual void Run(ThreadClass *thread) = 0;

	bool IsCancelled(void);

	void SetReference(void *const ref)
	{
		mReference = ref;
	}

private:
	JobManager *mManager;
	std::list<Job*>::iterator mJobsAddedIter;
//	std::set<Job*> mJobsRunningIter;
	std::multimap<void*, Job*>::iterator mJobReferenceIter;
	void *mReference;
	volatile bool mCancelled;
};

} // namespace RNReplicaNet

#endif
