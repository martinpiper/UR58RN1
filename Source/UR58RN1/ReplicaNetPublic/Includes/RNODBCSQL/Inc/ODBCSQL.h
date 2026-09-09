/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef _RNODBCSQL_H_
#define _RNODBCSQL_H_

#define _RNODBCSQL_VERSION 200

#include <map>
#include <list>
#include <string>
#include "RNPlatform/Inc/ThreadClass.h"
#include "RNODBCSQL/Inc/Cursor.h"

namespace RNReplicaNet
{

namespace RNODBCSQL
{

/// Escapes a text string and prepares it for use in an SQL statement.
extern std::string EscapeSQL(const std::string &input);

/// A class that implements a wrapper around ODBC to provide SQL queries with a cursor.
/// When using this library without linking any of the other ReplicaNet libraries it may be necessary to link with RNPlatform.
/// Regardless of the type of database this class aims to provide a consistent SQL interface.
/// It creates some common functions: RN_UNIX_TIMESTAMP() RN_CURRVAL( table name , column name ) RN_INTERVAL_TO_DOUBLE( interval )
/// Note: RN_CURRVAL( table name , column name ) should only be used directly after an insert to find the value of the primary key in the inserted table. It should not be used at other times.
/// Supported databases are: PostgreSQL , MySQL
class ODBCSQL
{
public:
	enum DatabaseType
	{
		kUnknown = 0,
		kMySQL,
		kPostgreSQL
	};

	/// The ctor performs some basic initialisation
	ODBCSQL();

	/// The dtor makes sure everything is tidy
	virtual ~ODBCSQL();

	/// \param DSN The DSN to use.
	/// \param retryAttempts This causes the connection attempt to retry before reporting a failure to connect. The default is to retry ten times.
	bool Open(const char *DSN,const int retryAttempts = 10);

	bool Close(void);

	/// \param retry When true this retries the database operation if it is busy or a table is locked.
	bool Exec(const char *sql,const bool retry = true);

	/// \param retry When true this retries the database operation if it is busy or a table is locked.
	bool Exec(const char *sql,int &rowCount,const bool retry = true);

	/// \param retry When true this retries the database operation if it is busy or a table is locked.
	bool Exec(const char *sql,Cursor &cursor,const bool retry = true);

	/// \param retry When true this retries the database operation if it is busy or a table is locked.
	/// \param rowCount The number of rows updated by the query.
	bool Exec(const char *sql,int &rowCount,Cursor &cursor,const bool retry = true);

	std::string GetLastError(void) const;

	/// Starts a transaction. The SQL commands "START TRANSACTION;" and "BEGIN TRANSACTION;" are exactly matched and use this function.
	bool TransactionBegin(void);

	/// Rollback a transaction. The SQL commands "ROLLBACK;" and "ROLLBACK TRANSACTION;" are exactly matched and use this function.
	bool TransactionRollback(void);

	/// Commit a transaction. The SQL command "COMMIT;" is exactly matched and uses this function.
	bool TransactionCommit(void);

	DatabaseType GetDatabaseType(void);

private:
	bool OpenInternal(void);


	void *mEnv;
	void *mDB;
	void *mStmt;
	std::string mLastError;
	MutexClass mMutex;
	bool mInTrans;
	std::string mLastDSN;
	int mLastRetries;
	bool mDoingRetry;
	DatabaseType mDetectedDatabaseType;
	bool mStopRecursiveOpenInternal;
};

} // namespace RNODBCSQL

} // namespace RNReplicaNet

#endif
