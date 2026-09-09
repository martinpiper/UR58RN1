/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef _RNODBCSQL_PREPARE_H_
#define _RNODBCSQL_PREPARE_H_
#include <map>
#include <list>
#include <string>
#include "RNPlatform/Inc/ThreadClass.h"

namespace RNReplicaNet
{

namespace RNODBCSQL
{

/// Handles preparing statements for SQL queries.
class Prepare
{
public:
	/// Set() should be used to set the statement to use
	Prepare();

	/// Prepares a new statement to be completed later on by adding values.
	/// \param statement Value parameters to be filled in are either '?' or a label starting with '@'. The label text must be alphanumeric and is case sensitive.
	explicit Prepare(const char *statement);
	explicit Prepare(const std::string &statement);

	virtual ~Prepare();

	/// Prepares a new statement to be completed later on by adding values.
	/// \param statement Value parameters to be filled in are either '?' or a label starting with '@'. The label text must be alphanumeric and is case sensitive.
	void Set(const char *statement);
	void Set(const std::string &statement);

	/// Begins the statement value addition, if this is a new instance then this function call can be skipped.
	Prepare& Begin(void);

	/// Adds various value types to the statement. The string types "std::string" and "char *" are automatically escaped before being added to the statement.
	/// \param value The value to add.
	/// \param label An optional label to replace in the statement, by default no label is provided and the first '?' parameter is replaced. If a label is provided then all matching labels in the statement are replaced.
	template<typename T> Prepare& Val(T value , const char *label = 0);

	/// Gets the resulting SQL string with all parameters filled in. Before returning Begin() is also called ready for any further values.
	/// \return The resultant SQL or null if parameters are missing.
	const char *GetSQL(void);

	/// Useful for ODBCSQL::Exec()
	operator const char *()
	{
		return GetSQL();
	}

private:
	/// \return True indicates the parameter is a quoted string.
	bool ReplaceNext(const char *escapedValue);
	/// \return True indicates the parameter is a quoted string.
	bool ReplaceLabel(const std::string &label , const std::string &escapedValue);
	/// \return True indicates the parameter is a quoted string.
	bool ReplaceLabel(const char *label , const char *escapedValue);

	std::string mOriginal;
	std::string mBuilt;
	bool mFirst;
};

} // namespace RNODBCSQL

} // namespace RNReplicaNet

#endif
