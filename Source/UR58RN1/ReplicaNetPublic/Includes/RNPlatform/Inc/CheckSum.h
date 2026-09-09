/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef _CHECKSUM_H_
#define _CHECKSUM_H_

namespace RNReplicaNet
{

/**
 * This class handles simple checking of data blocks
 */
class Checksum
{
public:
	/**
	 * Performs a simple and quick checksum of a buffer
	 * \param buffer the data buffer to checksum
	 * \param the length of the data buffer in bytes
	 * \return the checksum for the data, the checksum returned is never 0
	 */
	static int ChecksumData( const void *buffer , const size_t length );

	/**
	 * Performs a simple and quick checksum of a buffer using a certain chunk size to generate more than 32 bits of data.
	 * \param checksumSlots The checksum slots array. By default this is cumulative so make sure the array is initialised to a known state.
	 * \param numSlots The number of checksumSlots supplied in the array.
	 * \param buffer The data buffer to checksum.
	 * \param length The length of the data buffer in bytes. When using the cumulative feature of this function the number of bytes must be a multiple of numSlots except for the last use of this function for a particular block of data.
	 * \initialiseSlots By default the checksumSlots are not initialised. Setting this to be true will initialise the checksumSlots.
	 * \initialiseSlotsWithLength By default the length of the data is not used to seed the checksumSlots when calculating the checksum. If this is true then the length will be used to seed the checksumSlots and this will alter the result of any cumulative checksum.
	 */
	static void ChecksumDataWithSlots( int *checksumSlots , const size_t numSlots, const void *buffer, const size_t length , const bool initialiseSlots = false , const bool initialiseSlotsWithLength = false);

private:
	Checksum(){}
	virtual ~Checksum(){}
};

} // namespace RNReplicaNet

#endif
