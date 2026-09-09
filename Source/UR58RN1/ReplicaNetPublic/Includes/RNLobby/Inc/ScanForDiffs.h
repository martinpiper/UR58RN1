/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef _SCANFORDIFFS_H_
#define _SCANFORDIFFS_H_
#include <list>
#include <vector>
#include "RNLobby/Inc/ScanPath.h"

namespace RNReplicaNet
{

namespace RNLobby
{

class ScanForDiffs
{
public:
	ScanForDiffs();
	virtual ~ScanForDiffs();

	/// This sets the maximum number of patch versions for each new file scanned.
	/// \param numVersions The number of versions for each new file scanned to keep in the history. 0 is a special case and will keep an infinite history of changes.
	void SetNumVersions(const size_t numVersions = 3);

	/// Gets the value from SetNumVersions(). 
	size_t GetNumVersions(void);

	/// This uses a patch index file and scans the files in a product to generate patch files from the previous versions submitted to this product index.
	/// \param level The compression level to use. 10 gives the best compression but is slow and uses more memory. 1 is fast, uses less memory but does not compress as well. The default is 8 giving reasonable performance.
	bool Start(const char *patchIndexPath,const char *patchDataPath,const std::list<ScanPath::Entry> &productFiles,std::vector<size_t> &idsRemoved,const size_t stripProductPath = 0,const size_t level = 8);

	/// A virtual callback that is triggered when the scanning status is updated. This can be called frequently so it should complete quickly.
	/// \return True allows the scan to continue. False terminates the scan as quickly as possible.
	virtual bool CallbackScanning(size_t pos,size_t max,const ScanPath::Entry &entry,const size_t filePos,const size_t fileMax);

	// A small utility function to compress and encrypt a file.
	int CompressFile(size_t pos,size_t max,const ScanPath::Entry &entry,const char *sourceFile,const char *destinationFile,const size_t level,const size_t chunkSize = 256*1024 , const bool useCores = true);

	// A small utility function to decrypt and decompress a file.
	int DecompressFile(size_t pos,size_t max,const ScanPath::Entry &entry,const char *sourceFile,const char *destinationFile);

	size_t mNumVersions;
};

} // namespace RNLobby

} // namespace RNReplicaNet

#endif
