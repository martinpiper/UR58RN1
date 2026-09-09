/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef _PATCHER_H_
#define _PATCHER_H_

namespace RNReplicaNet
{

namespace RNLobby
{

class PatcherPrivate;

/// This class generates minimal patches for binary files and also patches files to produce new versions of those files.
/// No checking of file integrity is carried out before a patch is applied because this is a file version problem which is
/// the domain of another set of support classes.
class Patcher
{
public:
	Patcher();
	virtual ~Patcher();

	/// Generates a patch from sourcePath compared with newPath and stores the result in patchPath.
	/// \param sourcePath The original file path.
	/// \param newPath The new file path.
	/// \param patchPath The destination file path for the patch file.
	/// \param level The compression level to use. 10 gives the best compression but is slow and uses more memory. 1 is fast, uses less memory but does not compress as well. The default is 5 giving reasonable performance.
	/// \return The length of the generated patch file. This can be zero and indicates the two files are identical. A negative value indicates an error occurred.
	int GeneratePatch(const char *sourcePath,const char *newPath,const char *patchPath,const size_t level = 5);

	/// Combines a file from sourcePath with a patch file from patchPath and stores the resultant file in destinationPath.
	/// \param sourcePath The original file path.
	/// \param patchPath The patch file path.
	/// \param destinationPath The destination file path for the combined file.
	/// \return Success returns true. False indicates an error occurred.
	bool PatchFile(const char *sourcePath,const char *patchPath,const char *destinationPath);

	/// A virtual callback that is triggered when the scanning status is updated.
	/// \return True allows the scan to continue. False terminates the scan as quickly as possible.
	virtual bool CallbackScanning(const size_t filePos,const size_t fileMax);

private:
	PatcherPrivate &mData;
};

} // namespace RNLobby

} // namespace RNReplicaNet

#endif
