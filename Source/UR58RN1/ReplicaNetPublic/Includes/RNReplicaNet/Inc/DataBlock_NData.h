/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef __DATABLOCK_NDATA_H__
#define __DATABLOCK_NDATA_H__

#include "RNReplicaNet/Inc/DataBlock.h"
#include "RNPlatform/Inc/RegistryManagerMap.h"

namespace RNReplicaNet
{

/**
 * This DataBlock derived class scans the registered area for changes and if needed creates a packet detailing those changes<br>
 * This DataBlock does not do any lag compensation or prediction. It just informs each replica of a change when one is made. By default the DataBlock will preserve the endian format of the data. To disable the endian check, an example would be for string data, then SetEndianTest(false) or the ROL command DisableEndianTest();<br>
 */
class DataBlock_NData : public DataBlock
{
public:
	RN_CLASS_MINIMAL_RTTI();
	/**
	 * The ctor performs some basic initialisation
	 */
	DataBlock_NData();

	/**
	 * The dtor makes sure everything is tidy
	 */
	virtual ~DataBlock_NData();

	virtual void Poll(DataBlockReason *reason,const bool discardOlderIncluded = false,const unsigned int discardOrder = 0);

	virtual void ParseMessage(MessageHelper *message);

	/**
	 * This registers the memory area with this data block type and returns a new pointer
	 * \param data the start of the memory to check
	 * \param size the size of the data block
	 * \return Returns a pointer that can be used to attach to a ReplicaObject
	 */
	static DataBlock_NData *Register(void *const data,const int size);

	/**
	 * This registers the memory area with this data block type. This is used when the memory is in datablocks that are allocated as part of a bigger class.
	 * \param data the start of the memory to check
	 * \param size the size of the data block
	 */
	void RegisterInClassPointer(void *const data,const int size)
	{
		mData = data;
		mSize = size;
	}

	/**
	 * This allows a block of memory to be passed in that shows a default state of the data block. This default memory state is copied so it can be temporary.
	 * When a session joins or an object is created, the datablock states are serialised. By setting a default state the user can avoid the extra data being sent out for the datablock state.
	 * \param data the pointer to the memory.
	 */
	void SetDefaultData(const void *data);

	bool IsAttached(void *data);

	void SetEndianTest(const bool test = true);

	bool GetEndianTest(void) const;

	virtual void OwnerChanged(const int sessionID);

protected:	// User defined DataBlocks have access to these variables.
	void FreeSessionBlocks(void);
	void *mData;
	int mSize;

	struct SessionBlock_NData
	{
		SessionBlock_NData(int datasize);

		virtual ~SessionBlock_NData();

		unsigned int mDiscardOrder;
		void *mTestData;

		// Forced updates
		// This works off local time, not network time
		SysTimeType mLastUpdateSentLocalTime;
		int mNumForcedUpdates;
	};

	RegistryManagerMap<int,SessionBlock_NData> mPerSessionData;

	void *mDefaultData;
	bool mDisableEndian;
};

} // namespace RNReplicaNet

/* _RO_DO_REGISTERBLOCK_NDATA_VAR macro starts here */
#define _RO_DO_REGISTERBLOCK_NDATA_VAR(x)	\
	{\
		RNReplicaNet::DataBlock_NData *datablock = 0;	\
		datablock = RNReplicaNet::DataBlock_NData::Register(&(basepoint->x),sizeof(basepoint->x));	\
		_RO_DO_SETDATABLOCKVARIABLENAME(x);
/* _RO_DO_REGISTERBLOCK_NDATA_VAR macro ends here */

/* _RO_DO_REGISTERBLOCK_NDATA_VARAT macro starts here */
#define _RO_DO_REGISTERBLOCK_NDATA_VARAT(x)	\
	{\
		RNReplicaNet::DataBlock_NData *datablock = 0;	\
		datablock = RNReplicaNet::DataBlock_NData::Register((basepoint->x),sizeof(basepoint->x));	\
		_RO_DO_SETDATABLOCKVARIABLENAME(x);
/* _RO_DO_REGISTERBLOCK_NDATA_VARAT macro ends here */

/* _RO_DO_REGISTERBLOCK_NDATA_DISABLEENDIANTEST macro starts here */
#define _RO_DO_REGISTERBLOCK_NDATA_DISABLEENDIANTEST()	\
		datablock->SetEndianTest(false);
/* _RO_DO_REGISTERBLOCK_NDATA_DISABLEENDIANTEST macro ends here */


#endif
