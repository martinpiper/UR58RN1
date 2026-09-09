/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef __XPCOMPRESSION_H__
#define __XPCOMPRESSION_H__

namespace RNReplicaNet
{

#define	_XPCOMPRESSION_VERSION 5100

/**
 * A general purpose lossless compression and decompression class which implements the LZMPi compression algorithm.
 */
class XPCompression
{
public:
	/**
	 * The ctor performs some basic initialisation
	 */
	XPCompression();

	/**
	 * The dtor makes sure everything is tidy
	 */
	virtual ~XPCompression();

	/**
	 * Given an input size calculate the maximum absolute worse case for the compressed output
	 * \param size the size of the data
	 * \return the worse case output length
	 */
	int CalculateMaximumMemory(int size);

	/**
	 * Compress a block of memory
	 * \param source_data the source data
	 * \param source_length the source data length in bytes
	 * \param destination_data the destination data
	 * \param destination_length the pointer to the destination data length in bytes to fill in
	 * \param level the compression level to use. 1 is fast and 10 is best but slower. The default for this parameter is 10. Compression level 6 is a good balance between compression ratio and speed.
	 * \return true is no error else false for an error
	 */
	bool Compress(void *source_data,int source_length,void *destination_data,int *destination_length,int level = 10);

	/**
	 * Decompress a block of memory
	 * \param source_data the source data
	 * \param source_length the source data length in bytes
	 * \param destination_data the destination data
	 * \param destination_length the pointer to the destination data length in bytes to fill in
	 * \param max_buffer_size the maximum size of the destination buffer
	 * \return true is no error else false for an error
	 */
	bool Decompress(void *source_data,int source_length,void *destination_data,int *destination_length,int max_buffer_size);

private:
};

} // namespace RNReplicaNet

#endif
