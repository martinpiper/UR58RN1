/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef __PLATFORMINFO_H__
#define __PLATFORMINFO_H__
#include "RNPlatform/Inc/Thread.h"

namespace RNReplicaNet
{

/**
 * A class designed to read various platform specific attributes
 */
class PlatformInfo
{
public:
	/**
	 * Supplies information on if the library is compiled with threading on or off.
	 * \return returns true if threading is turned on
	 */
	static bool IsThreaded(void);

	/**
	 * Tests for endianness of the machine
	 * \return returns true for little endian machines
	 */
	static bool IsLittleEndian(void);

	/**
	 * Sets if threads are allowed on this system. Allowing threads with single threaded libraries may result in unexpected results.
	 * \param allow true means allow threads, false does not allow threads
	 */
	static void SetThreaded(const bool allow);

	/**
	 * Sets the preferred processor for the ReplicaNet thread. See Thread::SetPreferredProcessor() for more information.
	 * This must be called before any ReplicaNet class is allocated. This configuration information is remembered for the lifetime of the application.
	 */
	static void SetProcessorReplicaNet(const int processor = kReplicaNetPreferredProcessorOSChoice);
	static int GetProcessorReplicaNet(void);

	/**
	 * Sets the preferred processor for the XPSession thread. See Thread::SetPreferredProcessor() for more information.
	 * This must be called before any ReplicaNet class is allocated. This configuration information is remembered for the lifetime of the application.
	 */
	static void SetProcessorXPSession(const int processor = kReplicaNetPreferredProcessorOSChoice);
	static int GetProcessorXPSession(void);

	/**
	 * Sets the preferred processor for the XPURL thread. See Thread::SetPreferredProcessor() for more information.
	 * This must be called before any ReplicaNet class is allocated. This configuration information is remembered for the lifetime of the application.
	 */
	static void SetProcessorXPURL(const int processor = kReplicaNetPreferredProcessorOSChoice);
	static int GetProcessorXPURL(void);

	/**
	 * Gets the number of CPU cores.
	 */
	static int GetNumberOfCores(void);
};

} // namespace RNReplicaNet

#endif
