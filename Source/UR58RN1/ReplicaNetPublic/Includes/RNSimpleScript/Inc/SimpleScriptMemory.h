/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef __SIMPLESCRIPTMEMORY_H__
#define __SIMPLESCRIPTMEMORY_H__

class SimpleScriptMemory
{
public:
	SimpleScriptMemory();
	virtual ~SimpleScriptMemory();

	bool LoadBinary(const char *filename);

	unsigned char GetU8(const int address);
	bool SetU8(const int address,const unsigned char value);
	signed char GetS8(const int address);
	bool SetS8(const int address,const signed char value);

	unsigned short GetU16(const int address);
	bool SetU16(const int address,const unsigned short value);
	signed short GetS16(const int address);
	bool SetS16(const int address,const signed short value);

	unsigned int GetU32(const int address);
	bool SetU32(const int address,const unsigned int value);
	signed int GetS32(const int address);
	bool SetS32(const int address,const signed int value);

	int GetException(void) const;

	int GetExceptionAddress(void) const;

	void *GetPointer(const int address = 0);

	int GetSize(void)
	{
		return mSize;
	}

private:
	void *mMemory;
	int mSize;

	int mException;
	int mExceptionAddress;
};

#endif
