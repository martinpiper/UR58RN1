/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef __DATABLOCK_H__
#define __DATABLOCK_H__
#include <string>

#include "RNPlatform/Inc/SysTime.h"
#include "RNPlatform/Inc/MinimalRTTI.h"

namespace RNReplicaNet
{

class MessageHelper;
class DynamicMessageHelper;
class ReplicaNet;
class ReplicaObject;

const int kDataBlockReason_SomeoneJoined			= 0;	/**< Called for a data block when someone joins */
const int kDataBlockReason_SomeoneLeft				= 1;	/**< Called for a data block when someone leaves */
const int kDataBlockReason_NormalPoll				= 2;	/**< Poll this datablock for the current session ID and if the data block creates a packet it gets sent */
const int kDataBlockReason_CalculateReplicaPoll		= 4;	/**< Polls the data block for a replica of an object which allows extrapolation to be done each frame. DataBlockReason::mFloatValue is the time delta since the last update.*/
const int kDataBlockReason_CalculateMasterPoll		= 5;	/**< A poll done once per master object to allow the master to calculate extrapolation data if needed */
const int kDataBlockReason_MasterPrepAfterTakeover	= 6;	/**< This makes the master do preparation work on a datablock after an object takeover has been done */
const int kDataBlockReason_ReplicaCallBackPoll		= 7;	/**< Polls the data block for a replica of an object which then allows replica datablocks to create datablocks to be sent to the master datablock */
const int kDataBlockReason_ReplicaPrepAfterTakeover	= 8;	/**< This makes the replica do preparation work on a datablock after an object takeover has been done */
const int kDataBlockReason_RecordingNormalPoll		= 9;	/**< This event is used for replicas that have been asked to emit a packet for recording. This is usually the last known state the object received via a packet update. */
const int kDataBlockReason_AfterDataBlocksReceived	= 10;	/**< This event is triggered once for each object after all initial state update datablocks are received and before ReplicaObject::PostObjectCreate() is triggered. */

struct DataBlockReason;

/**
 * A ReplicaObject derived class can have any number of variables that need to be shared to the corresponding replicas.<br>
 * A data block is a class that monitors for a change of a specific variable in a master object and if needed creates a data packet that is sent around the session to other replicas.<br>
 * It is possible for a DataBlock to receive kDataBlockReason_CalculateMasterPoll and kDataBlockReason_ReplicaPrepAfterTakeover, thus allowing a DataBlock to be a master and a replica at the same time and this can allow a DataBlock to reflect propagated values.
 * Each new data type that can get shared across a network creates a new class that inherits from this one.
 * For IsClassType() and GetThisClassType() to work the DataBlock type checking must be enabled by including the RN_CLASS_MINIMAL_RTTI macro in the public part of a derived class definition.
 */
class DataBlock
{
friend class ReplicaObject;
public:
	RN_CLASS_MINIMAL_RTTI_BASE_CLASS()
	virtual int GetThisClassType(void);	// Override this to assert if someone tries to directly query this base class. It should never happen.
	/**
	 * The ctor performs some basic initialisation
	 */
	DataBlock();

	/**
	 * The dtor makes sure everything is tidy
	 */
	virtual ~DataBlock();

	/**
	 * A poll function that is called by ReplicaNet to check for updates to this particular data block. If this virtual method is implemented by a derived class the parent method should be called before a data packet is created.
	 * Processing for this function may occur during the thread context of ReplicaNet so processing time used by this function should be kept to a minimum to avoid performance issues.
	 * \param reason the reason code that is passed in to this class to enable a response to be calculated
	 * \param discardOlderIncluded When true and using SetDiscardOlder() this gives a hint to the DataBlock that the derived class has filled in a valid value for discardOrder. The default value is false and the DataBlock will use a global discard order seqeunce.
	 * \param discardOrder When using SetDiscardOlder() and discardOlderIncluded is true this must contain a valid order sequence number for the update. The sequence should be unique for each target sessionID for the update. Since the sequence number is local for each target sessionID the window size for SetDiscardOlder() can be smaller.
	 */
	virtual void Poll(DataBlockReason *reason = 0,const bool discardOlderIncluded = false,const unsigned int discardOrder = 0);

	/**
	 * This parses a message for the derived class of this data block. If this virtual method is implemented by a derived class the parent method should be called.
	 * Implementation note: The same amount of data added to the message during the Poll() must equal the length of data retrieved during this function. Care must be taken when implementing functionality such as GetDiscardOlder() to preserve the length of data.
	 * Processing for this function may occur during the thread context of ReplicaNet so processing time used by this function should be kept to a minimum to avoid performance issues.
	 * \param message The current message buffer that contains the packet data
	 */
	virtual void ParseMessage(MessageHelper *message);

	/**
	 * Sets the name of the variable attached to this DataBlock which is then used to output extended debugging information using the registered DebugHandler if available. The default value for the variable name is null.
	 * This function is used by ReplicaNet and the standard DataBlocks added to objects using the ROL Language if the preprocessor define REPLICANET_VISUALDEBUGGER is used. For user allocated DataBlocks this function can be used to set extra debugging information, such as the variable name, useful to the author. Each DataBlock implementation can choose to not output debug information if there is no name set. The standard DataBlocks included with ReplicaNet output the DataBlock class name with a unique hex number identifier prefixed with "<Unknown>" if the variable name is null.
	 * \param name The name of the variable.
	 */
	void SetVariableName(const char *name = 0);

	/**
	 * Bind this data block with the ReplicaNet pointer. This is called from ReplicaObject::RegisterDataBlock()
	 * \param current the ReplicaNet pointer
	 */
	void BindReplicaNet(ReplicaNet *current);

	/**
	 * Bind this data block with the ReplicaObject pointer. This is called from ReplicaObject::RegisterDataBlock()
	 * \param current the ReplicaObject pointer
	 */
	void BindReplicaObject(ReplicaObject *object);

	/**
	 * Sets the reliable flag for a particular data set
	 * \param reliable set to true if this data block is meant to be reliable and the certain update is set to be false. The default is false
	 */
	void SetReliableFlag(bool reliable = false);

	/**
	 * Gets the reliable flag for a particular data set
	 * \return if this data block is meant to be reliable true is returned
	 */
	bool GetReliableFlag(void) const;

	/**
	 * Sets the certain flag for a particular data set
	 * \param certain set to true if this data block is meant to be certain and the reliable update is set to be false. The default is false
	 */
	void SetCertainFlag(bool certain = false);

	/**
	 * Gets the reliable flag for a particular data set
	 * \return if this data block is meant to be certain true is returned
	 */
	bool GetCertainFlag(void) const;

	/**
	 * Sets the unreliable ordered flag for a particular data set
	 * \param certain set to true if this data block is meant to be using the unreliable ordered packet sending method, the certain and reliable flags are set to be false. The default is false
	 */
	void SetOrderedFlag(bool ordered = false);

	/**
	 * Gets the ordered flag for a particular data set
	 * \return if this data block is meant to be ordered true is returned
	 */
	bool GetOrderedFlag(void) const;

	/**
	 * Instead of using the unreliable ordered packet sending type this enables automatic DataBlock checking of older updates.
	 * This update method will add extra data for each sequence order numbers for update sent, rather than for each update packet type.
	 * If Poll() discardOlderIncluded is false, which is the default option, a global sequence order number will be used for all sessionIDs. So the windowBits must be larger to accomodate larger numbers of players and potential wrap around of the seqeunce.
	 * If Poll() discardOlderIncluded is true then the sequence order supplied by the derived dataBlock will be used, this can use a sequence order local to each sessionID. Since the sequence order can be local to each sessionID the windowBits can be smaller.
	 * This update method will discard any updates that are older than the currently received update.
	 * Implementation note: The derived DataBlock needs to call GetDiscardThisUpdate() to implement this functionality. When ignoring an update during ParseMessage() the size of data from the input packet must be accounted for. The standard ReplicaNet DataBlocks use this where appropriate.
	 * While this update method can be enabled for reliable and unreliable ordered packet update types it is not optimal to do so because all packet updates are already ordered by the network layer.
	 * \param windowBits The number of bits to use for the order checking window. The larger the number of bits the more accurate the order checking is. A good value is to use 16 bits for small scale games of 32 players and >=24 bits for MMO games. The default value is 0 and disables this mechanism. The minimum emabled value for this mechanism is 8 bits.
	 */
	void SetDiscardOlder(const int windowBits = 0);

	/**
	 * Gets the update ordered flag for a particular data set
	 * \return if this data block is meant to be ordered true is returned
	 */
	int GetDiscardOlder(void) const;

	/**
	 * Sets the automatic update flag for a particular data set. By default a DataBlock will calculate the best time to create an update for a particular set of data. If the automatic updates are switched off then the datablock relies on hints from the user to calculate updates.
	 * \param automatic set to true if this data block is meant to be automatically updated. The default is true
	 */
	void SetAutomaticUpdatesFlag(bool automatic = true);

	/**
	 * Gets the automatic update flag for a particular data set
	 * \return if this data block is meant to be automatically updated true is returned
	 */
	bool GetAutomaticUpdatesFlag(void) const;

	/**
	 * This is a feedback function used by a master ReplicaObject that enables an application to give a delta hint to a DataBlock. The delta should be expressed in units of change per second.
	 * \param pdata the pointer to the data type to give a hint for
	 * \param dataLem the length of the data type
	 */
	virtual void GiveDeltaHint(void *pdata,int dataLen);

	/**
	 * This is a feedback function used by a master ReplicaObject that enables an application to give a delta hint to a DataBlock. This float type call maps to the opaque data type call.
	 * \param This delta hint is a float type
	 */
	void GiveDeltaHint(float delta)
	{
		GiveDeltaHint(&delta,sizeof(delta));
	}

	/**
	 * Enums for the continuity break function
	 */
	enum ContinuityBreakTypes
	{
		kStop = 1,				/**< Indicates the data set value has stopped changing for the time being */
		kSuddenChange = 2,		/**< Indicates a sudden change in direction or movement */
		kTeleport = 4			/**< Indicates the value has a large dislocation i.e. An object has teleported. This can result in an update being calculated and sent for a DataBlock so use with care. */
	};

	/**
	 * This is used by a master ReplicaObject to set a continuity break for the data block and provides useful information to any extrapolation algorithms.
	 * \param breakTypes The break types to use for this break. For example : DataBlock::kSuddenChange | DataBlock::kTeleport
	 */
	virtual void ContinuityBreak(unsigned char breakTypes);

	/**
	 * If the DataBlock is attached to a reflection ReplicaObject this allows any received replica update to be forced into the outgoing DataBlock updates.
	 * /param enable The default value is false meaning the DataBlock will calculate its own delta updates.
	 */
	void SetForceReflectionUpdate(const bool enable = false);

	bool GetForceReflectionUpdate(void);

	/**
	 * If the DataBlock is attached to a reflection ReplicaObject this allows any received replica update to be applied to the reflection master, improving the accuracy of outgoing reflection updates. The replica poll stage will be ignored since the reflection master poll has already updated the value.
	 * /param enable The default value is false meaning the DataBlock will calculate its own updates during the normal replica poll.
	 */
	void SetReflectionMasterUpdate(const bool enable = false);

	bool GetReflectionMasterUpdate(void);

	/**
	 * Given a data pointer this function identifies if this data block is attached to the pointer.
	 * \return Returns true if the data pointer is attached to this data block.
	 */
	virtual bool IsAttached(void *data);

	/**
	 * This virtual function allows the DataBlock to say how is was allocated.
	 * \return Returns true if the DataBlock was allocated using 'new' as a pointer returned.
	 */
	virtual bool IsAllocated(void) const;

	/**
	 * Defines how the DataBlock was allocated.
	 * \param isAllocated Set this to be true if the DataBlock was allocated by using 'new' and a pointer returned. True is the default value.
	 */
	void SetIsAllocated(const bool isAllocated = true);


	/**
	 * This sets the minimum possible delay between each update, due to a DataBlock filter condition being met, for this datablock to each ReplicaObject. This allows the bandwidth for this DataBlock to be fine tuned.
	 * A value of 1.0f will mean a maximum of 1 update per second to each ReplicaObject. A value of 0.2f will mean a maximum possible 5 updates due to filter conditions per second, one every 0.2 seconds, to each ReplicaObject.
	 * This setting is separate from SetForceSendTimeDelay()
	 * This can be changed by the user at any time.
	 * The default value is 0.0f which disables this mechanism.
	 * The actual mechanism is implemented by the derived DataBlock. For user extended DataBlocks this mechanism can be ignored if the DataBlock filter policy method does not require this functionality.
	 * \param delay The minimum delay in seconds.
	 */
	void SetUpdateDelayMinimum(const float delay = 0.0f);

	/**
	 * This function returns the value set by SetUpdateDelayMinimum()
	 * \return The value.
	 */
	float GetUpdateDelayMinimum(void) const;

	/**
	 * This sets the maximum number of consecutive force updates that can be sent from this DataBlock to each ReplicaObject.
	 * Forced updates are sent for this DataBlock to a ReplicaObject if the time since the last update, either due to the update filter policy or due to a forced update, is greater than the time set by SetForceSendTimeDelay().
	 * Usually the best possible approximation of the DataBlock change calculated at the time is used.
	 * If a normal DataBlock update is sent, in other words an update falls within the DataBlock update filter policy, then the trigger for forced updates is set allowing consecutive forced updates to be started.
	 * This feature is useful for unreliable updates where DataBlocks that do not change often usually causing fewer DataBlock updates can be refreshed or force updated.
	 * It is a good idea, but not required, to set SetForceSendTimeDelay() before setting this value to be anything greater than 0 because the default value of 0.0f for SetForceSendTimeDelay() could cause many quick consecutive updates to be sent.
	 * This can be changed by the user at any time.
	 * The default value is 0 which disables this mechanism.
	 * The actual mechanism is implemented by the derived DataBlock. For user extended DataBlocks this mechanism can be ignored if the DataBlock filter policy method does not require this functionality.
	 */
	void SetForceSendMaximumUpdates(const int updates = 0);

	/**
	 * This function returns the value set by SetUpdateDelayMaximum()
	 * \return The value.
	 */
	int GetForceSendMaximumUpdates(void) const;

	/**
	 * This sets a time delay from the previous DataBlock update to a ReplicaObject before another forced send update is required.
	 * Setting this to be 0.2f causes a forced send update to be sent if the time since the last DataBlock update is more than 0.2f seconds.
	 * Therefore if SetForceSendTimeDelay(0.3f) and SetForceSendMaximumUpdates(3) is used, then a DataBlock that stops changing will get 3 extra updates with a delay of 0.3f of a second between each update.
	 * This setting for forced updates overrides the SetUpdateDelayMaximum() setting.
	 * This can be changed by the user at any time.
	 * The default value is 0.0f which then causes delays to be sent every internal ReplicaNet poll as required by SetMaximumForceSendUpdates()
	 * The actual mechanism is implemented by the derived DataBlock. For user extended DataBlocks this mechanism can be ignored if the DataBlock filter policy method does not require this functionality.
	 */
	void SetForceSendTimeDelay(const float delay = 0.0f);

	/**
	 * This function returns the value set by SetUpdateDelayMaximum()
	 * \return The value.
	 */
	float GetForceSendTimeDelay(void) const;

	/**
	 * Sets the band used by the underlying Transport when using the Reliable method to send this DataBlock update.
	 * The default value is 0 which is the same band as the internal session messages.
	 */
	void SetBand(const unsigned int band = 0);

	/**
	 * This function returns the value set by SetBand()
	 * \return The value.
	 */
	unsigned int GetBand(void) const;


	/**
	 * Called by ReplicaNet to check incoming packets for any discard older sequence when parsing incoming data packets.
	 */
	bool ShouldDiscard(MessageHelper *message);

	/**
	 * Called by ReplicaNet to indicate a change in owner. If this virtual method is implemented by a derived class the parent method must also be called.
	 */
	virtual void OwnerChanged(const int sessionID);

	/**
	 * Gets the last calculated result of a call to ShouldDiscard().
	 * If the dervied DataBlock needs to support SetDiscardOlder() call this during ParseMessage() to determine if an update should be discarded due to ordering problems.
	 */
	bool GetDiscardThisUpdate(void) const;

protected:	// User defined DataBlocks have access to these variables.
	/**
	 * This sets the ID of the data block when it is used in a ReplicaObject. This function is called from ReplicaObject::RegisterDataBlock()
	 */
	void SetID(int id);

	const char *GetROName(ReplicaObject *from = 0);

	void EmitBaseDebugInfo(const char *name,const char *className,DataBlockReason *reason);
	void EmitBaseDebugInfoUpdates(const char *name,const char *className,DataBlockReason *reason);

	std::string GetVariableName(void);

	short mID;
	ReplicaNet *mBoundReplicaNet;
	ReplicaObject *mBoundObject;
	bool mReliable;
	bool mCertain;
	bool mOrdered;
	bool mAutomaticUpdates;
	bool mIsAllocated;
	bool mDebugOn;
	char *mDebugText;
	float mUpdateDelayMinimum;
	int mForceSendMaximumUpdates;
	float mForceSendTimeDelay;
	char *mVariableName;
	unsigned int mBand;
	int mDiscardOlder;
	unsigned int mDiscardOlderMidPoint;
	unsigned int mDiscardOlderEndPoint;
	size_t mDiscardOlderRealSize;
	unsigned int mLocalisedOrderingSent;
	unsigned int mLocalisedOrderingReceived;
	bool mDiscardThisUpdate;

	bool mForceReflectionUpdate;
	bool mReflectionMasterUpdate;
};

/**
 * A parameter block that is used to pass state information to a DataBlock
 */
struct DataBlockReason
{
public:
	/**
	 * This structure ctor sets a reason code by default if none is passed in
	 * \param code The reason code to use,kDataBlockReason_NormalPoll by default
	 * \param session The session ID to set in the data block, -1 by default
	 */
	DataBlockReason(int code = kDataBlockReason_NormalPoll,int session = -1,ReplicaObject *object = 0)
	{
		mReasonCode = code;
		mSessionReference = session;
		mObserver = object;
		mFloatValue = 0.0f;
		mFloatTime = -1.0f;
	}

	int mReasonCode;						/**< The reason code */
	int mSessionReference;					/**< The reference session ID */
	DynamicMessageHelper *mMessageHelper;	/**< The message helper for this data block */
	ReplicaObject *mObserver;				/**< A reference to the observer object for this session reference */
	SysTimeType mFloatValue;				/**< A general purpose value that is dependant on the reason code */
	SysTimeType mFloatTime;
	SysTimeType mFloatLocalTime;
};

// This macro is used to finish any data block registration for a ReplicaObject
/* _RO_DO_REGISTERBLOCK_END macro starts here */
#define _RO_DO_REGISTERBLOCK_END()	\
		RegisterDataBlock(datablock);	\
	}
/* _RO_DO_REGISTERBLOCK_END macro ends here */

} // namespace RNReplicaNet

#endif
