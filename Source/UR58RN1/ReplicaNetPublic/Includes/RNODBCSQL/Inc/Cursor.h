/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef _RNODBCSQL_CURSOR_H_
#define _RNODBCSQL_CURSOR_H_
#include <map>
#include <list>
#include <string>
#include "RNPlatform/Inc/ThreadClass.h"

namespace RNReplicaNet
{

namespace RNODBCSQL
{

/// A cursor used to access information returned by a SQL query.
class Cursor
{
public:
	Cursor();
	virtual ~Cursor();

	/// Simple test to see if there is a row waiting in the cursor.
	bool GetNext(void);

	/// Gets the value from the first column and not the column name. Then advances onto the next row.
	/// \param result is only updated if there is a value, if there is no value then result is unchanged.
	/// \return true when thee is a value, false if there is no value.
	bool GetNext(std::string &result);
	bool GetNext(int &result);
	bool GetNext(float &result);
	bool GetNext(double &result);

	/// Gets the value from the first column and not the column name. The advances onto the next column. At the end of the row the next row is fetched and the first column value is returned.
	/// Basically this gets the values in row then column order.
	bool GetNextByRowColumn(std::string &result);
	bool GetNextByRowColumn(int &result);

	/// Gets the whole row
	bool GetNext(std::list<std::pair<std::string,std::string> > &result);

	/// Gets the number of rows.
	size_t GetNumResults(void);

	/// Gets the value of the current row from the column.
	/// \param column A case insensitive column name.
	/// \param result The result reference.
	bool GetByColumn(const char *column,std::string &result);

	/// Gets the value of the current row from the column.
	/// \param column A case insensitive column name.
	std::string GetByColumn(const char *column);

	/// Returns true if the cursor has no results
	bool IsEmpty(void);

	size_t GetCurrentRowIndex(void);

	void Clear(void);

private:
friend class ODBCSQL;
	std::list<std::list<std::pair<std::string,std::string> > > mResults;
	std::list<std::list<std::pair<std::string,std::string> > >::iterator mCurrentRow;
	std::list<std::pair<std::string,std::string> >::iterator mCurrentColumn;
	std::map<std::string,std::string> mCurrentRowMap;
	void MapCurrentRow(void);
	size_t mCurrentRowNum;

	void AdvanceRow(void);
};

} // namespace RNODBCSQL

} // namespace RNReplicaNet

#endif
