/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef _PRODUCTPATCHER_H_
#define _PRODUCTPATCHER_H_
#include <string>
#include <list>
#include <stddef.h>
// MPi: TODO: Make cross platform
#include <windows.h>
#include <wininet.h>
#include "RNPlatform/Inc/SysTime.h"
#include "RNPlatform/Inc/ThreadClass.h"
#include "RNPlatform/Inc/Thread.h"

namespace RNReplicaNet
{

namespace RNLobby
{

/// This patches a product from patch data and if there are any missing product files these will be created.
/// A file date stamp and checksum cache is used to speed up checking of existing unchanged files.
class ProductPatcher : private ThreadClass , private Thread
{
public:
	ProductPatcher();
	virtual ~ProductPatcher();

	enum Status
	{
		kSuccess = 0,
		kErrorEncountered,
		kAborted
	};

	/// Starts patching a product given patch data paths.
	/// \param productPath The paths to the product to patch.
	/// \param multiThread By default the scan does not use an extra thread. Using true will start a thread to start the patch process and Start() will return immediately.
	/// \param chunkSize The number of bytes to use for a file buffer when accessing files. By default this uses a megabyte. The larger the buffer the quicker the download will be but the calls to CallbackUpdateDownloadPatch() will be less frequent.
	/// \return Success returns true, failure returns false.
	bool Start(const char *productPath,const char *patchIndexURL,const char *patchDataURL,const bool multiThread = false,const size_t chunkSize = 1024*1024);

	/// Stops the current patch if it is running.
	/// \return Success returns true, failure returns false.
	bool Stop(void);

	/// Tests the completion state of the last patch.
	/// \return A completed patch returns true, a patch that is in progress returns false.
	bool GetCompleted(void);

	/// A virtual callback that is triggered during the patching process.
	/// \return True allows the patch to continue. False terminates the patch as quickly as possible.
	virtual bool CallbackUpdateProgressProduct(const size_t fileIndex,const size_t maxFileIndex);
	/// A virtual callback that is triggered during the patching process.
	/// \return True allows the patch to continue. False terminates the patch as quickly as possible.
	virtual bool CallbackUpdateDownloadPatch(const char *patchingFile,const size_t filePosition,const size_t fileLength);
	/// A virtual callback that is triggered during the patching process.
	/// \return True allows the patch to continue. False terminates the patch as quickly as possible.
	virtual bool CallbackUpdateProgressFile(const char *patchingFile,const size_t filePosition,const size_t fileLength);

	/// A virtual callback that is triggered when the patching is complete.
	virtual void CallbackComplete(const Status status);

	/// A virtual callback that is triggered when there is an error.
	virtual void CallbackFailedToReadFile(const char *file);

	/// A virtual callback that is triggered to allow the client application to skip a file in the patch index during the patch process.
	/// The file only includes the entry as it is seen in the index and does not include any product path.
	/// \return true The default implementation always returns true allowing the file to be patched. Returning false will skip the file during the patching process.
	virtual bool CallbackProcessFile(const char *file);

	/// Gets the number of attempted downloads and the number of downloads that failed and were automatically tried again.
	/// This can be useful for giving an indication of the health of a network connection during patching.
	void GetDownloadAttemptsAndFailed(size_t &attempts, size_t &failed) const;

	/// Gets information that can be used to calculate the download speed.
	void GetDownloadSpeed(size_t &totalBytes, float &overTime) const;

protected:
	void Tidy(void);

private:

	void OpenInternet(void);

	int ThreadEntry(void);
	int RealThreadEntry(const char *productPath,const char *patchIndexURL,const char *patchDataURL);
	std::string GetRealFilePath(const char *url,const char *patchingFile,const char *tempName = 0);

	std::string mProductPath;
	std::string mPatchIndexURL;
	std::string mPatchDataURL;

	HINTERNET mInternet;
	void *mTempBuffer;
	size_t mChunkSize;
	HINTERNET mFile;
	FILE *mFP;
	volatile bool mCallCompleteRequired;

	bool mAborted;

	size_t mDownloadAttempts;
	size_t mDownloadAttemptsFailed;

	size_t mDownloadedBytes;
	SysTimeType mDownloadedOverTime;
};

} // namespace RNLobby

} // namespace RNReplicaNet

#endif
