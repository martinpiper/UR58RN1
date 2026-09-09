/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef __DATABLOCK_FUNCTION_H__
#define __DATABLOCK_FUNCTION_H__

#include "RNReplicaNet/Inc/DataBlock.h"
#include "RNPlatform/Inc/MessageHelper.h"
#include "RNPlatform/Inc/RegistryManagerMap.h"
#include <list>

namespace RNReplicaNet
{

struct CombinedDataPacket;
struct SessionBlock_Function;

/**
 * This DataBlock derived class enables class member functions to be called on replicas of an object.
 * Deleting a ReplicaObject derived class during a networked function call is not allowed. ReplicaObject::DelayedDelete() should be used instead. This will flag the object for a delete as soon as it is safe to do so.<br>
 * This DataBlock does not do any latency compensation or prediction.<br>
 * This DataBlock does not use the rules set by SetUpdateDelayMinimum() and SetForceSendMaximumUpdates() because the frequency of calling member functions and hence the frequency of DataBlock updates is defined by the application.<br>
 * For security reasons only the master can send function calls to other replicas. A replica does not send any function updates to other replicas directly and will only send to the master. Therefore many of the macros, like ALL_REPLICAS_FUNCTION_CALL(), will operate differently depending on if the object is a master or a replica.<br>
 * Before the object is published no network class member function calls will be propagated. Using these macros with an unpublished object will only cause the non-network portion to be executed, for example GLOBAL_FUNCTION_CALL will only call the function on the object.<br>
 * You can use these macros to call network class member functions:<br>
 * GLOBAL_FUNCTION_CALL()<br>
 * Example: GLOBAL_FUNCTION_CALL(ASampleMemberFunction(foo,bar,wibble));<br>
 * From a master this calls the function on the master and all replicas.<br>
 * From a replica this only calls the function on the current replica and the master.<br><br>
 * ALL_REPLICAS_FUNCTION_CALL()<br>
 * Example: ALL_REPLICAS_FUNCTION_CALL(ASampleMemberFunction(foo));<br>
 * From a master this calls the function on all replicas.<br>
 * From a replica this only calls the function on the master.<br><br>
 * MASTER_FUNCTION_CALL()<br>
 * Example: MASTER_FUNCTION_CALL(ASampleMemberFunction(bar,wibble));<br>
 * From a master this calls the function on all replicas.<br>
 * From a replica this only calls the function on the master.<br>
 * ONLY_MASTER_FUNCTION_CALL()<br>
 * Example: ONLY_MASTER_FUNCTION_CALL(ASampleMemberFunction(bar,wibble));<br>
 * From a master this calls the function only on the master.<br>
 * From a replica this only calls the function on the master.<br>
 * ALL_REPLICAS_FUNCTION_CALL() and MASTER_FUNCTION_CALL() have identical implementation, the naming is different to improve code readability.<br><br>
 * NOMINATED_REPLICAS_FUNCTION_CALL()<br>
 * Example: NOMINATED_REPLICAS_FUNCTION_CALL(numSessionIDs,sessionIDs,APublicFunction(random_int,random_float));<br>
 * From a master this uses DataBlock_Function::SetSessionIDFilter() to propagate function calls to selected session IDs.
 * From a replica this calls the function on the master regardless of the session ID list.<br>
 */
class DataBlock_Function : public DataBlock
{
public:
	RN_CLASS_MINIMAL_RTTI();
	typedef	void (tDataBlock_Function_funcp)(void *classp,void *data,void *thisDataBlock);

	/**
	 * The ctor performs some basic initialisation
	 */
	DataBlock_Function();

	/**
	 * The dtor makes sure everything is tidy
	 */
	virtual ~DataBlock_Function();

	virtual void Poll(DataBlockReason *reason,const bool discardOlderIncluded = false,const unsigned int discardOrder = 0);

	virtual void ParseMessage(MessageHelper *message);

	/**
	 * This registers the memory area with this data block type and returns a new pointer
	 * \param classp is the pointer to this class that is passed through the message pipeline
	 * \param funcp the actual function pointer to call that cracks the message
	 * \return Returns a pointer that can be used to attach to a ReplicaObject
	 */
	static DataBlock_Function *Register(void *classp,tDataBlock_Function_funcp *funcp);

	/**
	 * This registers the memory area with this data block type. This is used when the memory is in datablocks that are allocated as part of a bigger class.
	 * \param classp is the pointer to this class that is passed through the message pipeline
	 * \param funcp the actual function pointer to call that cracks the message
	 */
	void RegisterInClassPointer(void *classp,tDataBlock_Function_funcp *funcp)
	{
		mClassp = classp;
		mData = funcp;
	}

	/**
	 * This configures the next networked function call from a master object to send the function call to the nominated sessionIDs. After the function call is completed all subsequent network function calls are sent without this filter unless this function is used. The NOMINATED_REPLICAS_FUNCTION_CALL() macro also uses this function.<br>
	 * Care must be taken when using this static function that multi-threaded ReplicaObject derived classes do not interrupt calls to this function and the subsequent network function call. The array pointed to by this function must persist until the subsequent network function call.<br>
	 * \param numSessionIDs The number of session IDs in the array.
	 * \param sessionIDs An array of session IDs that will receive the next network function call.
	 */
	static void SetSessionIDFilter(const int numSessionIDs = 0,const int *sessionIDs = 0);

	void AddFunctionStateChange(void *data,int len,const char *debug = 0);

	bool IsAttached(void *data);

	virtual void OwnerChanged(const int sessionID);

protected:	// User defined DataBlocks have access to these variables.
	void FreeSessionBlocks(void);
	void *mClassp;
	tDataBlock_Function_funcp *mData;

	RegistryManagerMap<int,SessionBlock_Function> mPerSessionFunction;

	std::list<CombinedDataPacket *> mFunctionStateChanges;

	static int mConfiguredNumSessions;
	static int *mConfiguredSessionIDs;
};

// Specialise this template to allow different variable types to be used with DataBlock_Function
template<typename T>
void FunctionAddVariable(DynamicMessageHelper &message, T &variable)
{
	message.AddVariable(&variable,sizeof(variable));
}

// Specialise this template to allow different variable types to be used with DataBlock_Function
template<typename T>
void FunctionGetVariable(MessageHelper &message, T &variable)
{
	message.GetVariable(&variable,sizeof(variable));
}

} // namespace RNReplicaNet


/* _RO_DO_REGISTERBLOCK_FUNCTION_VAR macro starts here */
#define _RO_DO_REGISTERBLOCK_FUNCTION_VAR(x)	\
	{\
		RNReplicaNet::DataBlock_Function *datablock = 0;	\
		datablock = RNReplicaNet::DataBlock_Function::Register((void *)this,&x);	\
		mDataBlockFunction_##x = datablock;	\
		_RO_DO_SETDATABLOCKVARIABLENAME(x);
/* _RO_DO_REGISTERBLOCK_FUNCTION_VAR macro ends here */

// For member function wrapping. A lot of macro work
/* _RO_MAKE_MEMBERFUNC macro starts here */
#define _RO_MAKE_MEMBERFUNC(x)	Call_##x
/* _RO_MAKE_MEMBERFUNC macro ends here */

/* _RO_DO_MEMBERFUNC_PRESTAGE_DEF macro starts here */
#define _RO_DO_MEMBERFUNC_PRESTAGE_DEF(x,y)	\
void _MAKE_RO(x)::_RO_MAKE_MEMBERFUNC(y)(
/* _RO_DO_MEMBERFUNC_PRESTAGE_DEF macro ends here */


/* _RO_DO_MEMBERFUNC_PRESTAGE macro starts here */
#ifdef REPLICANET_VISUALDEBUGGER
#define _RO_DO_MEMBERFUNC_PRESTAGE(x)	\
)	\
{	\
	if (!mDataBlocksRegistered)	\
	{	\
		return;	\
	}	\
	RNReplicaNet::DynamicMessageHelper message;	\
	message.SetBufferSize(256);	\
	std::string debug;
#else //ifdef REPLICANET_VISUALDEBUGGER
#define _RO_DO_MEMBERFUNC_PRESTAGE(x)	\
)	\
{	\
	if (!mDataBlocksRegistered)	\
	{	\
		return;	\
	}	\
	RNReplicaNet::DynamicMessageHelper message;	\
	message.SetBufferSize(256);
#endif //ifdef REPLICANET_VISUALDEBUGGER
/* _RO_DO_MEMBERFUNC_PRESTAGE macro ends here */

/* _RO_DO_MEMBERFUNC_ADDVAR macro starts here */
#define _RO_DO_MEMBERFUNC_ADDVAR(x)	\
	RNReplicaNet::FunctionAddVariable(message,x);
// Updated version of this macro, does the same as _RO_DO_MEMBERFUNC_ADDVAR but also includes the string variable type for extra debug info.
#ifdef REPLICANET_VISUALDEBUGGER
#define _RO_DO_MEMBERFUNC_ADDVAR2(x,y)	\
	RNReplicaNet::FunctionAddVariable(message,x);	\
	if (RNReplicaNet::XPURL::GetDebugHandler())	\
	{	\
		debug += #y;	\
		debug += "\n";	\
		debug += RNReplicaNet::MessageHelper::DumpAsHex(&x,sizeof(x));	\
		debug += "\n";	\
	}
#else // ifdef REPLICANET_VISUALDEBUGGER
#define _RO_DO_MEMBERFUNC_ADDVAR2(x,y)	\
	RNReplicaNet::FunctionAddVariable(message,x);
#endif // ifdef REPLICANET_VISUALDEBUGGER
/* _RO_DO_MEMBERFUNC_ADDVAR macro ends here */

/* _RO_DO_MEMBERFUNC_POSTSTAGE macro starts here */
#ifdef REPLICANET_VISUALDEBUGGER
#define _RO_DO_MEMBERFUNC_POSTSTAGE(x)	\
	RNReplicaNet::DataBlock_Function *datablock = (RNReplicaNet::DataBlock_Function *) mDataBlockFunction_##x;	\
	datablock->AddFunctionStateChange(message.GetBuffer(),message.GetSize(),debug.c_str());	\
}
#else //ifdef REPLICANET_VISUALDEBUGGER
#define _RO_DO_MEMBERFUNC_POSTSTAGE(x)	\
	RNReplicaNet::DataBlock_Function *datablock = (RNReplicaNet::DataBlock_Function *) mDataBlockFunction_##x;	\
	datablock->AddFunctionStateChange(message.GetBuffer(),message.GetSize());	\
}
#endif //ifdef REPLICANET_VISUALDEBUGGER
/* _RO_DO_MEMBERFUNC_POSTSTAGE macro ends here */


/* _RO_DO_MEMBERFUNC_CRACK_START macro starts here */
#define _RO_DO_MEMBERFUNC_CRACK_START(x,y)	\
void _MAKE_RO(x)::y(void *classp,void *opaque_message,void *thisDataBlock)	\
{	\
	_MAKE_RO(x) *thisclass = (_MAKE_RO(x) *) classp;	\
	RNReplicaNet::MessageHelper *message = (RNReplicaNet::MessageHelper *) opaque_message;	\
	message = message;
/* _RO_DO_MEMBERFUNC_CRACK_START macro ends here */

/* _RO_DO_MEMBERFUNC_MESSAGE_CRACK macro starts here */
#define _RO_DO_MEMBERFUNC_MESSAGE_CRACK(x)	\
	RNReplicaNet::FunctionGetVariable(*message,x);
/* _RO_DO_MEMBERFUNC_MESSAGE_CRACK macro ends here */


/* _RO_DO_MEMBERFUNC_CRACK_END macro starts here */
#define _RO_DO_MEMBERFUNC_CRACK_END(x,y)	\
	if (!((RNReplicaNet::DataBlock*)thisDataBlock)->GetDiscardThisUpdate())	\
	{	\
		((x*)thisclass)->y(
/* _RO_DO_MEMBERFUNC_CRACK_END macro ends here */

/* _RO_DO_MEMBERFUNC_CRACK_END2 macro starts here */
#define _RO_DO_MEMBERFUNC_CRACK_END2()	\
	);	\
	}	\
}
/* _RO_DO_MEMBERFUNC_CRACK_END2 macro ends here */

#endif
