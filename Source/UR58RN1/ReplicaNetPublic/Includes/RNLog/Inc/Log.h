/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef _RNLOG_H_
#define _RNLOG_H_
#include <list>
#include <string>
#include <stdio.h>
#include "RNPlatform/Inc/ThreadClass.h"
#include "RNPlatform/Inc/Thread.h"
#include "RNODBCSQL/Inc/ODBCSQL.h"
#include "RNODBCSQL/Inc/Prepare.h"
#include "RNPlatform/Inc/SysTime.h"

namespace RNReplicaNet
{

namespace RNLog
{

/// A logging class that logs to an SQL database table.
class Log : public ThreadClass , public Thread , private SysTime
{
public:
	/// The ctor performs some basic initialisation
	Log(const char *DSN /*= MRNLobbyDefaultDSN*/,const char *tableName = "Logs", size_t maxEntries = 1000,const float flushTime = 0.5f);

	/// The dtor makes sure everything is tidy
	virtual ~Log();

	/// Adds an entry to the database log and if necessary starts the background logging thread.
	/// All strings are escaped with RNODBCSQL::EscapeSQL().
	void AddEntry(const std::string &logText);
	void AddEntry(const char *logText);

	/// Adds a printf formatted string and arguments to the log. After formatting the string is passed to AddEntry(const char *)
	void AddEntryFormat(const char *format, ...);
	/// Used for extending the debug log print with the va_list updated by an initial va_start(ap, format) and va_end(ap) must be called after this function.
	void AddEntryFormat(const char *format, va_list ap);

private:
	virtual int ThreadEntry(void);
	void DoLogWork(void);

	MutexClass mEntriesMutex;
	std::list<std::pair<std::string,std::string> > mEntryQueue;

	RNODBCSQL::ODBCSQL mSQL;
	SysTimeType mLastEntryTime;
	SysTimeType mFlushTime;
	std::string mTableName;
	size_t mMaxEntries;
	volatile bool mDisableThreadStart;

	RNODBCSQL::Prepare mPreparedAddLog;
	RNODBCSQL::Prepare mPreparedDeleteLog;
};

} // namespace RNLog

} // namespace RNReplicaNet

#endif
