/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef _SCANPATH_H_
#define _SCANPATH_H_
#include <string>
#include <list>
#include <stddef.h>
// MPi: TODO: Make cross platform
#include <windows.h>
#include "RNPlatform/Inc/ThreadClass.h"
#include "RNPlatform/Inc/Thread.h"

namespace RNReplicaNet
{

namespace RNLobby
{

/// This scans a supplied path and stores the results.
class ScanPath : private ThreadClass , private Thread
{
public:
	struct FileChecksum
	{
		int mValue[8];
	};
	/// An entry in the list updated by ScanPath::Start()
	struct Entry
	{
		Entry();
		std::string mName;		///< The name of the entry
		unsigned int mSize;		///< The size of the entry. This can be the length of the file or the size of the directory.
		bool mIsDirectory;
		bool mIsLink;
		bool mIsFile;
// MPi: TODO: Make cross platform
		FILETIME mLastWriteTime;
		FileChecksum mChecksum;
	};

	ScanPath();
	virtual ~ScanPath();


	/// Starts scanning a path using an optional new thread. If a scan is already running the previous scan is stopped.
	/// \param path The path to start scanning from.
	/// \param result The std::list<RNReplicaNet::RNLobby::ScanPath::Entry> to use for storing results of the scan. The result is not cleared when the scan starts allowing results on other scans to be appended to the std::list. To access the result before the scan is finished use pairs of ScanPath::Lock()/UnLock() to lock access to the result variable using the thread mutex.
	/// \param recurse By default recursive scanning of the input path is disabled.
	/// \param justFiles By default only file names are added to the results of the scan.
	/// \param multiThread By default the scan does not use an extra thread. Using true will start a thread to start scanning and Start() will return immediately.
	/// \return Success returns true, failure returns false.
	bool Start(const char *path,std::list<Entry> &result,const bool recurse = false,const bool justFiles = true,const bool multiThread = false);

	/// Stops the current scan if it is running.
	/// \return Success returns true, failure returns false.
	bool Stop(void);

	/// Tests the completion state of the last scan.
	/// \return A completed scan returns true, a pending scan returns false.
	bool GetCompleted(void);

	/// A virtual callback that is triggered when the scanning status is updated.
	/// \return True allows the scan to continue. False terminates the scan as quickly as possible.
	virtual bool CallbackUpdate(const size_t numFiles,const size_t numFolders);

	/// A virtual callback that is triggered when the scanning is complete.
	virtual void CallbackComplete(void);

	/// A utility function to write the entries to a file.
	static bool Write(const char *path,const std::list<Entry> &entries);

	/// A utility function to read the entries from a file.
	static bool Read(const char *path,std::list<Entry> &entries);

private:

	int ThreadEntry(void);
	int RealThreadEntry(const std::string path,std::list<Entry> &results,const bool recurse,const bool justFiles);

	std::string mPath;
	std::list<Entry> *mResult;
	bool mRecurse;
	bool mJustFiles;
	HANDLE mhSearch;
};

// Can be used for STL less<> tests
bool operator< (const ScanPath::Entry& a,const ScanPath::Entry& b);

} // namespace RNLobby

} // namespace RNReplicaNet

#endif
