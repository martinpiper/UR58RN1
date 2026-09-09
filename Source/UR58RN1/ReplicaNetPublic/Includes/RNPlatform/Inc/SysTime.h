/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef __SYSTIME_H__
#define __SYSTIME_H__

#if defined(_XBOX)
#include <xtl.h>			// For XBox builds
#else
#if defined(_WIN32)
#include <windows.h>		// For Windows builds
#endif
#endif

#if defined(_PS2)
#include <eekernel.h>
#endif

namespace RNReplicaNet
{

#if !defined(REPLICANET_TIME_FLOAT) && !defined(REPLICANET_TIME_DOUBLE)
#define REPLICANET_TIME_DOUBLE
#endif
#ifdef REPLICANET_TIME_FLOAT
typedef float SysTimeType;
#endif
#ifdef REPLICANET_TIME_DOUBLE
typedef double SysTimeType;
#endif

struct SystemTime
{
	unsigned short wYear;
	unsigned short wMonth;
	unsigned short wDayOfWeek;
	unsigned short wDay;
	unsigned short wHour;
	unsigned short wMinute;
	unsigned short wSecond;
	unsigned short wMilliseconds;
};

/**
 * Enables a high performance timer to be used.
 */
class SysTime
{
public:

	/**
	 * The ctor performs some basic initialisation
	 */
	SysTime();

	/**
	 * The dtor makes sure everything is tidy
	 */
	virtual ~SysTime();

	/**
	 * Read the current time and return the time as a SysTimeType which uses floating point.
	 * Use the preprocessor define REPLICANET_TIME_FLOAT or REPLICANET_TIME_DOUBLE to force SysTimeType to be a float or a double. By default the type is double.
	 */
	SysTimeType		FloatTime(void);

	/**
	 * \return The number of milliseconds that have elapsed since the system was started.
	 */
	static unsigned int GetTickCount(void);

	/**
	 * Resets the time back to zero. Effectively initialising this class again.
	 */
	void Reset(void);

	/// A replacement for the Win32 API GetSystemTime() and struct SYSTEMTIME
	static void GetSystemTime(SystemTime *sysTime);

private:
#if defined(_WIN32)
	double		pfreq;
	double		curtime;
	double		lastcurtime;
	int			lowshift;
	int			sametimecount;
	unsigned int	oldtime;


	LARGE_INTEGER startticktime;
	LARGE_INTEGER	PerformanceFreq;
#endif
#if defined(_PS2)
	u_long mPS2Initial;
#endif
	int			first;
	unsigned int mBaseTickCount;
	int		mSecBase;
};

} // namespace RNReplicaNet

#endif //#ifndef __SYSTIME_H__
