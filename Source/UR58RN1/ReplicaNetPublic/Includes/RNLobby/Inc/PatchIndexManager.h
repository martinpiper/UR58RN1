/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef _PATCHINDEXMANAGER_H_
#define _PATCHINDEXMANAGER_H_
#include <string>
#include <map>
#include "RNLobby/Inc/ScanPath.h"

namespace RNReplicaNet
{

class DynamicMessageHelper;

namespace RNLobby
{

/// Handles loading and saving of patch index files with various access methods.
class PatchIndexManager
{
public:
	// Used by the stl::map
	struct ltLowerStr
	{
		bool operator() (const std::string& a,const std::string& b) const;
	};

	PatchIndexManager();
	virtual ~PatchIndexManager();

	struct VersionInfo
	{
		VersionInfo() : mUniqueID(0) , mProductVersion(0) , mIsPatch(false) , mNewThisIteration(false) , mDataFileSize(0)
		{
		}

		ScanPath::FileChecksum mChecksum;
		// This unique ID is used to form the filename of the file that is used to contain the patch/compressed data file this index entry.
		// This means a minimum unique ID can be used to delete old patch/compressed files from the patch data directory when they are no longer needed.
		unsigned int mUniqueID;
		unsigned int mProductVersion;
		bool mIsPatch;
		bool mNewThisIteration;
		unsigned int mDataFileSize;	// The size of the patch or the compressed file that may be downloaded

		void Read(DynamicMessageHelper &in);

		void Write(DynamicMessageHelper &out) const;

		void Dump(void) const;
	};

	// Paired with file path by the std::map
	struct EntryInfo
	{
		EntryInfo()
		{
			mNumMaxVersions = 0;
			mHasBeenHandled = false;
		}
		unsigned int mNumMaxVersions;		// Custom limit per file if required
		VersionInfo mScratch;
		std::list<VersionInfo> mVersions;
		bool mHasBeenHandled;
	};

	void Clear(void);
	bool ReadIndex(const char *filename);
	bool WriteIndex(const char *filename,const bool minimalInfo = false);
	void Dump(void) const;

	void AddEntryInfo(std::string filename,const EntryInfo &entryInfo);
	EntryInfo *GetEntryInfo(std::string filename);

	unsigned int GetUniqueID(void);

	void RemoveUnhandled(void);
	void ClearNewThisIteration(void);

	void SetProductVersion(unsigned int productVersion)
	{
		mProductVersion = productVersion;
	}

	unsigned int GetProductVersion(void)
	{
		return mProductVersion;
	}

	unsigned int GetNumEntries(void)
	{
		return (int)mIndex.size();
	}

	void BeginIterate(void);

	EntryInfo *Iterate(std::string &index);

	static unsigned int GetVersion(void);


	struct ModifiedInfo
	{
		ModifiedInfo() : mVisited(false)
		{
		}

		bool mVisited;
		std::string mTimeStamp;
	};

	void ClearModified(void);
	bool ReadModified(const char *filename);
	bool WriteModified(const char *filename);
	bool GetIsModified(const char *filename);

private:
	unsigned int mProductVersion;
	unsigned int mLastUniqueID;

	std::map<std::string,EntryInfo,ltLowerStr> mIndex;
	std::map<std::string,EntryInfo,ltLowerStr>::iterator mSt;

	std::map<std::string,ModifiedInfo,ltLowerStr> mModified;

};

} // namespace RNLobby

} // namespace RNReplicaNet

#endif
