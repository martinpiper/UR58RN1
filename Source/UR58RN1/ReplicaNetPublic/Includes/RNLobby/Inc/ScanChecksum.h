/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef _SCANCHECKSUM_H_
#define _SCANCHECKSUM_H_
#include <string>
#include <list>
#include <stddef.h>
// MPi: TODO: Make cross platform
#include <windows.h>
#include "RNPlatform/Inc/ThreadClass.h"
#include "RNPlatform/Inc/Thread.h"
#include "RNLobby/Inc/ScanPath.h"

namespace RNReplicaNet
{

namespace RNLobby
{

/// This scans a supplied path and stores the results.
class ScanChecksum : private ThreadClass , private Thread
{
public:
	ScanChecksum();
	virtual ~ScanChecksum();


	/// Starts scanning the paths supplied using an optional new thread and generates checksum information for each file. If a scan is already running the previous scan is stopped.
	/// \param paths The paths to scan and produce checksum information for.
	/// \param multiThread By default the scan does not use an extra thread. Using true will start a thread to start scanning and Start() will return immediately.
	/// \param chunkSize The number of bytes to use for a file buffer when accessing files. By default this uses a megabyte.
	/// \return Success returns true, failure returns false.
	bool Start(std::list<ScanPath::Entry> &paths,const bool multiThread = false,const size_t chunkSize = 1024*1024);

	bool Start(ScanPath::Entry &entry,const size_t chunkSize = 1024*1024);

	/// Stops the current scan if it is running.
	/// \return Success returns true, failure returns false.
	bool Stop(void);

	/// Tests the completion state of the last scan.
	/// \return A completed scan returns true, a pending scan returns false.
	bool GetCompleted(void);

	/// A virtual callback that is triggered when the scanning status is updated.
	/// \return True allows the scan to continue. False terminates the scan as quickly as possible.
	virtual bool CallbackUpdate(const size_t numFiles,const ScanPath::Entry &currentEntry,const size_t currentBytesScanned);

	/// A virtual callback that is triggered when the scanning is complete.
	virtual void CallbackComplete(void);

private:

	int ThreadEntry(void);
	int RealThreadEntry(std::list<ScanPath::Entry> &paths,const size_t chunkSize);

	std::list<ScanPath::Entry> *mPaths;
	size_t mChunkSize;
	FILE *mFP;
	void *mBuffer;
};

} // namespace RNLobby

} // namespace RNReplicaNet

#endif
