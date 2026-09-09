/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef __SIMPLESCRIPTREGISTER_H__
#define __SIMPLESCRIPTREGISTER_H__

class SimpleScriptRegister
{
public:
	enum LastType
	{
		kUndefined = 0,
		kInt,
		kFloat,
		kProgramCounter
	};

	SimpleScriptRegister();
	virtual ~SimpleScriptRegister();

	LastType GetRegisterType(void) const;

	void SetProgramCounter(const unsigned int address);
	unsigned int GetProgramCounter(void) const;

	void SetInteger(const int value);
	int GetInteger(void) const;

	/// Does not change the underlying type of the register, only applies the 32 bit value
	void SetRaw32(const int value);
	void SetRaw32Float(const float value);

	/// Does not type cast from the register type, only returns the 32 bit register value.
	int GetRaw32(void) const;
	/// Does not type cast from the register type, only returns the 32 bit float value.
	float GetRaw32Float(void) const;


	void SetFloat(const float value);
	float GetFloat(void) const;

	void SetIsConstant(const bool isConstant = false);
	bool GetIsConstant(void) const
	{
		return mIsConstant;
	}

	void SetLastComparisonResult(const int lastComparisonResult);

	int GetLastComparisonResult(void) const;

	void WriteTo64Bits(void *const memory);

	void LoadFrom64Bits(void *const memory);

	void CopyFrom(SimpleScriptRegister *source)
	{
		mLastTypeWritten = source->mLastTypeWritten;
		mValue = source->mValue;
	}

	void SetNumberFromString(const char *value);
	void SetFloatNumberFromString(const char *value);

	int mRegisterIndex;

	void WriteTo32Bits(void *const memory);

	void LoadFrom32Bits(void *const memory , LastType type);

	void SetFloatingPointType(void);

private:
	LastType mLastTypeWritten;
	union
	{
		int mIntValue;
		float mFloatValue;
		unsigned int mProgramCounter;
	} mValue;

	bool mIsConstant;
	int mLastComparisonResult;
};

#endif
