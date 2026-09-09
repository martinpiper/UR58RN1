/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef __DATABLOCK_PREDICT_FLOAT_H__
#define __DATABLOCK_PREDICT_FLOAT_H__

#include "RNReplicaNet/Inc/DataBlock.h"
#include "RNPlatform/Inc/RegistryManagerMap.h"

namespace RNReplicaNet
{

/**
 * This DataBlock derived class tries to perform data prediction on float variable types.<br>
 * This class operates on three levels.<br>
 * 1) It constantly checks the data value and notes down changes based on what it thinks are continuity changes using the extrapolation model for the master side.<br>
 * 2) Each {value,delta and time} update sent to a replica object is stored and using this information the extrapolation/interpolation is checked against what the current value is doing. If the extrapolation is too far off based on the input tolerance values then the master copy of the extrapolation data is used to calculate a new more accurate prediction model.<br>
 * 3) The replica version of this class receives {value,delta and time} updates and extrapolates these. When performing extrapolation/interpolation the contents of the class member variable is read and the interpolation filter is used to smooth the update from the contents to the network value. When a continuity break is received the interpolation filter is ignored.<br>
 * The DataBlock type can also vary the accuracy of the tolerance used dependent on the calculated distance to the observer object for a session. If there is no observer or the distance couldn't be calculated (kReplicaObject_InfiniteDistance is returned by CalculateDistanceToObject) then the minimum tolerance is used.<br>
 * This DataBlock does allow the Ordered() flag to be set, however it has little use since this DataBlock will automatically reject old updates.
 */
class DataBlock_Predict_Float : public DataBlock
{
public:
	RN_CLASS_MINIMAL_RTTI();
	/**
	 * The ctor performs some basic initialisation
	 */
	DataBlock_Predict_Float();

	/**
	 * The dtor makes sure everything is tidy
	 */
	virtual ~DataBlock_Predict_Float();

	virtual void Poll(DataBlockReason *reason,const bool discardOlderIncluded = false,const unsigned int discardOrder = 0);

	virtual void ParseMessage(MessageHelper *message);

	virtual void GiveDeltaHint(void *pdata,int dataLen);

	virtual void GiveDeltaHint(const float delta);

	virtual void ContinuityBreak(unsigned char breakTypes);

	/**
	 * Gets the last delta for this DataBlock. Depending on the ReplicaObject replica or master state this is either the last received delta update or the most accurate calculated delta update.
	 * \return Returns true if the delta information has been updated. If false the input values have not been changed.
	 */
	virtual bool GetLastDelta(bool &continuityBreak, float &sample, SysTimeType &time, float &delta);

	bool IsAttached(void *data);

	/**
	 * This registers the memory area with this data block type and returns a new pointer
	 * \param data the start of the memory to check
	 * \return Returns a pointer that can be used to attach to a ReplicaObject
	 */
	static DataBlock_Predict_Float *Register(float *data);

	/**
	 * This registers the memory area with this data block type. This is used when the memory is in datablocks that are allocated as part of a bigger class.
	 * \param data the start of the memory to check
	 */
	void RegisterInClassPointer(float *data)
	{
		mData = data;
	}

	/**
	 * This sets the minimum error that is tolerated for this variable type
	 * \param minerror the minimum error to tolerate
	 */
	void SetMinError(float minerror = 0.1f);

	/**
	 * This sets the maximum error that is tolerated for this variable type
	 * \param maxerror the maximum error to tolerate
	 */
	void SetMaxError(float maxerror = 1.0f);

	/**
	 * Sets the minimum distance start point for the minimum error to take effect
	 * \param the distance
	 */
	void SetMinDistance(float distance = 0.1f);

	/**
	 * Sets the maximum distance start point for the maximum error to take effect
	 * \param the distance
	 */
	void SetMaxDistance(float distance = 1.0f);

	/**
	 * This sets the interpolation filter factor to use when extrapolating this variable type<br>
	 * 0 < interpolation <= 1.0f<br>
	 * 1.0f Is very hard interpolation and 0.1f is very soft. 0.1f Is the default.
	 * \param interpolation the interpolation to use
	 */
	void SetInterpolationFilter(float interpolation = 0.1f);

	/**
	 * For distance based interpolation this makes the calculation based on a quadratic formula. The default is linear.
	 */
	void SetQuadratic(void)
	{
		mIsQuadratic = true;
	}

	/**
	 * For distance based interpolation this makes the calculation based on a linear formula. The default is linear.
	 */
	void SetLinear(void)
	{
		mIsQuadratic = false;
	}

	/**
	 * Allows this replica DataBlock to use interpolation for a period specified by time. This has the effect of permanently lagging the changes made to the variable attached to this DataBlock. This option uses some extra memory to store updates until they can be actioned. This function can be used at any time. Each replica DataBlock can use a different time value.
	 * With ReplicaObjects that change owner there may be a pause in updates visible until the new master continues the movement and sends updates. NOTE: If a reflection object ID is set and matches the current session ID the lagged value will be ignored to avoid the update to replicas being lagged twice.
	 * \param time The time is seconds, so using SetLagged(0.2f) will cause all changes in this variable to appear to be 0.2 seconds behind the master object. Using a value less than or equal to 0.0f will disable this feature.
	 */
	void SetLagged(const float time = 0.0f);

	/**
	 * Allows this replica DataBlock to snap to the extrapolated value if the difference is greater than the configured difference.
	 * For a replica DataBlock if the difference between this DataBlock and the extrapolated value is greater than the configured amount then the extrapolated value is applied without any interpolation filter.
	 * This option can be changed as often as required and can be configured differently for each peer.
	 * \param difference The default value is -1.0f meaning this feature is disabled. Any value greater than or equal to 0.0f will enable this feature.
	 */
	void SetSnapToDifference(const float difference = -1.0f);

	/**
	 * Enable the boundary value checking code. This allows the DataBlock to have wrap around values (for example 0 <= angle < 360) or values that are capped.
	 * The upper and lower boundary and wrap around or capped are set by the other Boundary prefixed functions. The default is for this feature to be disabled.
	 * If enabled on a master then the user value will not be modified however the internal value used will be capped/wrapped before applying any update filtering.
	 * If enabled on a client then the user value will be capped/wrapped.
	 */
	void BoundaryEnable(void);

	/**
	 * Disables any boundary checking code.
	 */
	void BoundaryDisable(void);

	/**
	 * Use this keyword to set the boundary checks to use a wrap around value.
	 */
	void BoundaryWrapAround(void);

	/**
	 * Use this keyword to set the boundary checks to use a capped value.
	 */
	void BoundaryCapped(void);

	/**
	 * Where lower <= value < upper for wrapped checks and lower <= value <= upper for capped checks.
	 */
	void BoundaryLowerUpper(const float lower,const float upper);

	/**
	 * If the boundary value checking is enabled this will also write-back the boundary check to the master's client value. This write-back will happen the next time this DataBlock is polled.
	 * The default is for this value write-back to be disabled.
	 */
	void BoundaryApplyToMaster(void);

	/**
	 * Disables BoundaryApplyToMaster().
	 */
	void BoundaryDoNotApplyToMaster(void);

	/**
	 * Utility function to apply the boundary checking rules configured for a Predict_Float DataBlock to a variable. The variable must be registered with ReplicaNet as DataBlock_Predict_Float.
	 * \return True is returned if the variable was registered and false if not registered.
	 */
	static bool BoundaryApplyToValue(ReplicaObject *object,float &value);

	/**
	 * Set this DataBlock to use full 32 bit floating point values when propagating the value.
	 * This is the default option.
	 */
	void SetValueFloat(void);

	/**
	 * Set this DataBlock to use short 16 bit floating point values when propagating the value.
	 */
	void SetValueShortFloat(void);

	/**
	 * Set this DataBlock to use full 32 bit floating point values when propagating the value.
	 * This is the default option.
	 */
	void SetDeltaFloat(void);

	/**
	 * Set this DataBlock to use short 16 bit floating point values when propagating the value.
	 */
	void SetDeltaShortFloat(void);

	/**
	 * Sets the local replica extrapolation/interpolation, thus causing a local replica continuity break until an update arrives from the network.
	 * \param time The network time for the update.
	 * \param sample The data value.
	 * \param delta The delta value applied to the data per second.
	 * \param continuityBreak If true this indicates the value should be applied to the prediction calculation directly without an interpolation filter, for example a teleport continuity break uses this.
	 * \param ignoreTimeCheck Ignores the time check allowing an older network update to be applied to the prediction calculation.
	 * \param clearPreviousUpdates When true and using lagged updates (using SetLagged) this clears any previously received but pending updates.
	 */
	void ReplicaContinuityBreak(const SysTimeType time, const float sample, const float delta, const bool continuityBreak, const bool ignoreTimeCheck = false, const bool clearPreviousUpdates = false);

	/**
	 * Sets the amount of time the extrapolation will calculate into the future.
	 * \param timeDelta The time in seconds.
	 */
	void SetTimeDelta(const float timeDelta = 0.0f);

protected:	// User defined DataBlocks have access to these variables.
	float *mData;
	bool mContinuityBreak;
	bool mIsDistanceBased;
	float mMinDistance;
	float mMaxDistance;
	float mMinError;
	bool mIsQuadratic;
	bool mGotData;
	bool mImplementTeleport;
	bool mDoPrep;


	float mTestData;

	float mMaxError;
	float mInterpolationFilter;


	struct SessionBlock_Predict_Float
	{
		SessionBlock_Predict_Float();

		virtual ~SessionBlock_Predict_Float();

		unsigned int mDiscardOrder;
		bool mContinuityBreak;
		float mTestData;
		float mDelta;

		float mSample;
		SysTimeType mSampleTime;

		bool mImplementTeleport;

		// Forced updates
		// This works off local time, not network time
		SysTimeType mLastUpdateSentLocalTime;
		int mNumForcedUpdates;
	};

	RegistryManagerMap<int,SessionBlock_Predict_Float> mPerSessionData;

	struct ReplicaData
	{
		bool mFirstTime;
		bool mFirstTimeNotReset;

		float mDelta;
		float mSample;
		SysTimeType mSampleTime;
	};

	ReplicaData mReplicaData;

	struct MasterData
	{
		float mDelta;
		float mSample;
		SysTimeType mSampleTime;
		float mSampleHistory[2];
		SysTimeType mSampleHistoryTime[2];			// This is the network time and is used as values to send to any replicas
		SysTimeType mSampleHistoryTimeLocal[2];		// This is the local time and is only used to calculate deltas
		bool mGotDeltaHint;
		float mTheDeltaHint;
	};
	MasterData mMasterData;

	bool mDoneMasterPoll;

	// When lagged parsing is enabled any incoming replica packets are stored with this structure into mParsedByTime
	struct ParsedData
	{
		float mSample;
		float mDelta;
		bool mFirstTime;
	};
	std::map<SysTimeType,ParsedData> mParsedByTime;

	float mLagged;
	SysTimeType mFirstLaggedTime;

	float mSnapToDifference;

	bool mBoundaryEnable;
	bool mBoundaryIsWrap;
	float mBoundaryLower,mBoundaryUpper;
	bool mBoundaryWriteBack;

	float ApplyBoundaryToValue(const float value) const;

	SysTimeType mLastGoodDataTime;

	bool mShortValue;
	bool mShortDelta;

	int AddDataBlockData(DynamicMessageHelper *message,const float sample,const SysTimeType time,const float delta);

	float mTimeDelta;

	void FreeSessionBlocks(void);

	float CalculateReplicaValue(DataBlockReason *reason);
};

} // namespace RNReplicaNet

/* _RO_DO_REGISTERBLOCK_PREDICT_FLOAT_VAR macro starts here */
#define _RO_DO_REGISTERBLOCK_PREDICT_FLOAT_VAR(x)	\
	{\
		RNReplicaNet::DataBlock_Predict_Float *datablock = 0;	\
		datablock = RNReplicaNet::DataBlock_Predict_Float::Register(&(basepoint->x));	\
		_RO_DO_SETDATABLOCKVARIABLENAME(x);
/* _RO_DO_REGISTERBLOCK_PREDICT_FLOAT_VAR macro ends here */

/* _RO_DO_REGISTERBLOCK_PREDICT_FLOAT_SETMAXERROR macro starts here */
#define _RO_DO_REGISTERBLOCK_PREDICT_FLOAT_SETMAXERROR(x)	\
		datablock->SetMaxError((float)x);
/* _RO_DO_REGISTERBLOCK_PREDICT_FLOAT_SETMAXERROR macro ends here */

/* _RO_DO_REGISTERBLOCK_PREDICT_FLOAT_SETMINERROR macro starts here */
#define _RO_DO_REGISTERBLOCK_PREDICT_FLOAT_SETMINERROR(x)	\
		datablock->SetMinError((float)x);
/* _RO_DO_REGISTERBLOCK_PREDICT_FLOAT_SETMINERROR macro ends here */

/* _RO_DO_REGISTERBLOCK_PREDICT_FLOAT_SETERROR macro starts here */
#define _RO_DO_REGISTERBLOCK_PREDICT_FLOAT_SETERROR(x)	\
		datablock->SetMinError((float)x);	\
		datablock->SetMaxError((float)x);
/* _RO_DO_REGISTERBLOCK_PREDICT_FLOAT_SETERROR macro ends here */

/* _RO_DO_REGISTERBLOCK_PREDICT_FLOAT_SETMINDISTANCE macro starts here */
#define _RO_DO_REGISTERBLOCK_PREDICT_FLOAT_SETMINDISTANCE(x)	\
		datablock->SetMinDistance((float)x);
/* _RO_DO_REGISTERBLOCK_PREDICT_FLOAT_SETMINDISTANCE macro ends here */

/* _RO_DO_REGISTERBLOCK_PREDICT_FLOAT_SETMAXDISTANCE macro starts here */
#define _RO_DO_REGISTERBLOCK_PREDICT_FLOAT_SETMAXDISTANCE(x)	\
		datablock->SetMaxDistance((float)x);
/* _RO_DO_REGISTERBLOCK_PREDICT_FLOAT_SETMAXDISTANCE macro ends here */

/* _RO_DO_REGISTERBLOCK_PREDICT_FLOAT_SETINTERPOLATIONFILTER macro starts here */
#define _RO_DO_REGISTERBLOCK_PREDICT_FLOAT_SETINTERPOLATIONFILTER(x)	\
		datablock->SetInterpolationFilter((float)x);
/* _RO_DO_REGISTERBLOCK_PREDICT_FLOAT_SETINTERPOLATIONFILTER macro ends here */

/* _RO_DO_REGISTERBLOCK_PREDICT_FLOAT_SETLINEAR macro starts here */
#define _RO_DO_REGISTERBLOCK_PREDICT_FLOAT_SETLINEAR()	\
		datablock->SetLinear();
/* _RO_DO_REGISTERBLOCK_PREDICT_FLOAT_SETLINEAR macro ends here */

/* _RO_DO_REGISTERBLOCK_PREDICT_FLOAT_SETQUADRATIC macro starts here */
#define _RO_DO_REGISTERBLOCK_PREDICT_FLOAT_SETQUADRATIC()	\
		datablock->SetQuadratic();
/* _RO_DO_REGISTERBLOCK_PREDICT_FLOAT_SETQUADRATIC macro ends here */

/* _RO_DO_REGISTERBLOCK_PREDICT_FLOAT_SETLAGGED macro starts here */
#define _RO_DO_REGISTERBLOCK_PREDICT_FLOAT_SETLAGGED(x)	\
		datablock->SetLagged((float)x);
/* _RO_DO_REGISTERBLOCK_PREDICT_FLOAT_SETLAGGED macro ends here */

/* _RO_DO_REGISTERBLOCK_PREDICT_FLOAT_SETSNAPTODIFFERENCE macro starts here */
#define _RO_DO_REGISTERBLOCK_PREDICT_FLOAT_SETSNAPTODIFFERENCE(x)	\
		datablock->SetSnapToDifference((float)x);
/* _RO_DO_REGISTERBLOCK_PREDICT_FLOAT_SETSNAPTODIFFERENCE macro ends here */

/* _RO_DO_REGISTERBLOCK_PREDICT_FLOAT_BOUNDARYENABLE macro starts here */
#define _RO_DO_REGISTERBLOCK_PREDICT_FLOAT_BOUNDARYENABLE()	\
		datablock->BoundaryEnable();
/* _RO_DO_REGISTERBLOCK_PREDICT_FLOAT_BOUNDARYENABLE macro ends here */

/* _RO_DO_REGISTERBLOCK_PREDICT_FLOAT_BOUNDARYDISABLE macro starts here */
#define _RO_DO_REGISTERBLOCK_PREDICT_FLOAT_BOUNDARYDISABLE()	\
		datablock->BoundaryDisable();
/* _RO_DO_REGISTERBLOCK_PREDICT_FLOAT_BOUNDARYDISABLE macro ends here */

/* _RO_DO_REGISTERBLOCK_PREDICT_FLOAT_BOUNDARYWRAPAROUND macro starts here */
#define _RO_DO_REGISTERBLOCK_PREDICT_FLOAT_BOUNDARYWRAPAROUND()	\
		datablock->BoundaryWrapAround();
/* _RO_DO_REGISTERBLOCK_PREDICT_FLOAT_BOUNDARYWRAPAROUND macro ends here */

/* _RO_DO_REGISTERBLOCK_PREDICT_FLOAT_BOUNDARYCAPPED macro starts here */
#define _RO_DO_REGISTERBLOCK_PREDICT_FLOAT_BOUNDARYCAPPED()	\
		datablock->BoundaryCapped();
/* _RO_DO_REGISTERBLOCK_PREDICT_FLOAT_BOUNDARYCAPPED macro ends here */

/* _RO_DO_REGISTERBLOCK_PREDICT_FLOAT_BOUNDARYLOWERUPPER macro starts here */
#define _RO_DO_REGISTERBLOCK_PREDICT_FLOAT_BOUNDARYLOWERUPPER(x,y)	\
		datablock->BoundaryLowerUpper((float)x,(float)y);
/* _RO_DO_REGISTERBLOCK_PREDICT_FLOAT_BOUNDARYLOWERUPPER macro ends here */

/* _RO_DO_REGISTERBLOCK_PREDICT_FLOAT_BOUNDARYAPPLYTOMASTER macro starts here */
#define _RO_DO_REGISTERBLOCK_PREDICT_FLOAT_BOUNDARYAPPLYTOMASTER()	\
		datablock->BoundaryApplyToMaster();
/* _RO_DO_REGISTERBLOCK_PREDICT_FLOAT_BOUNDARYAPPLYTOMASTER macro ends here */

/* _RO_DO_REGISTERBLOCK_PREDICT_FLOAT_BOUNDARYDONOTAPPLYTOMASTER macro starts here */
#define _RO_DO_REGISTERBLOCK_PREDICT_FLOAT_BOUNDARYDONOTAPPLYTOMASTER()	\
		datablock->BoundaryDoNotApplyToMaster();
/* _RO_DO_REGISTERBLOCK_PREDICT_FLOAT_BOUNDARYDONOTAPPLYTOMASTER macro ends here */



/* _RO_DO_REGISTERBLOCK_PREDICT_FLOAT_SETVALUEFLOAT macro starts here */
#define _RO_DO_REGISTERBLOCK_PREDICT_FLOAT_SETVALUEFLOAT()	\
		datablock->SetValueFloat();
/* _RO_DO_REGISTERBLOCK_PREDICT_FLOAT_SETVALUEFLOAT macro ends here */

/* _RO_DO_REGISTERBLOCK_PREDICT_FLOAT_SETVALUESHORTFLOAT macro starts here */
#define _RO_DO_REGISTERBLOCK_PREDICT_FLOAT_SETVALUESHORTFLOAT()	\
		datablock->SetValueShortFloat();
/* _RO_DO_REGISTERBLOCK_PREDICT_FLOAT_SETVALUESHORTFLOAT macro ends here */

/* _RO_DO_REGISTERBLOCK_PREDICT_FLOAT_SETDELTAFLOAT macro starts here */
#define _RO_DO_REGISTERBLOCK_PREDICT_FLOAT_SETDELTAFLOAT()	\
		datablock->SetDeltaFloat();
/* _RO_DO_REGISTERBLOCK_PREDICT_FLOAT_SETDELTAFLOAT macro ends here */

/* _RO_DO_REGISTERBLOCK_PREDICT_FLOAT_SETDELTASHORTFLOAT macro starts here */
#define _RO_DO_REGISTERBLOCK_PREDICT_FLOAT_SETDELTASHORTFLOAT()	\
		datablock->SetDeltaShortFloat();
/* _RO_DO_REGISTERBLOCK_PREDICT_FLOAT_SETDELTASHORTFLOAT macro ends here */

/* _RO_DO_REGISTERBLOCK_PREDICT_FLOAT_SETTIMEDELTA macro starts here */
#define _RO_DO_REGISTERBLOCK_PREDICT_FLOAT_SETTIMEDELTA(x)	\
	datablock->SetTimeDelta((float)x);
/* _RO_DO_REGISTERBLOCK_PREDICT_FLOAT_SETTIMEDELTA macro ends here */

#endif
