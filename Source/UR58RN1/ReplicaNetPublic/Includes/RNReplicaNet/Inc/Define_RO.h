/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef __DEFINE_RO_H__
#define __DEFINE_RO_H__
#include "RNReplicaNet/Inc/DataBlock.h"
#include "RNReplicaNet/Inc/ROConstants.h"
#ifdef REPLICANET_VISUALDEBUGGER
#include "RNXPURL/Inc/XPURL.h"
#include "RNXPURL/Inc/DebugHandler.h"
#include "RNReplicaNet/Inc/RO_RegistryBlock.h"
#include "RNReplicaNet/Inc/ReplicaNet.h"
#endif

#define _MAKE_BASE(x) x
#define _MAKE_RO(x) _RO_##x
#define _MAKE_RO_ALLOCFUNC(x) _RO_AllocateFunc_##x
#define _MAKE_RO_DELETEFUNC(x) _RO_DeleteFunc_##x
#define _MAKE_RO_SETCLASSIDFUNC(x) _RO_SetClassIDFunc_##x
#define _MAKE_RO_REGISTERFUNC(x) _RO_RegisterFunc_##x

#ifdef REPLICANET_VISUALDEBUGGER
#define _RO_DODEBUGNAME()	\
	if (RNReplicaNet::XPURL::GetDebugHandler() && GetRegistryBlock() && GetRegistryBlock()->GetName())	\
	{	\
		_Internal_SetFullyQualifiedName(GetRegistryBlock()->GetName());	\
	}
#else
#define _RO_DODEBUGNAME()
#endif


/* _RO_DO_STD_FRAMEWORK macro starts here */
#define _RO_DO_STD_FRAMEWORK(x)	\
_RO_BEGINCTOR(x)	\
_Internal_GetUpdateType();	\
_Internal_SetupLoadBalance();	\
_Internal_SetupLoadBalanceScore();	\
_Internal_SetupLoadBalanceDefault();	\
_Internal_SetupLoadBalanceScoreDefault();	\
_Internal_SetupPropagateToSessionDistanceRecalculationDelay();	\
_Internal_SetupPropagateDistance();	\
_Internal_SetupPropagateDistanceDefault();	\
_Internal_SetupPropagateDistanceInclude();	\
_Internal_SetupPropagateDistanceIncludeDefault();	\
_Internal_SetupPropagateDistanceExclude();	\
_Internal_SetupPropagateDistanceExcludeDefault();	\
_Internal_SetupBand();	\
_RO_DODEBUGNAME()	\
_RO_ENDCTOR(x)	\
_RO_DTOR(x)	\
_RO_DO_ALLOCATE(x)	\
_RO_DO_ALLOCATEFUNC(x)
/* _RO_DO_STD_FRAMEWORK macro ends here */


/* _RO_STD_FRAMEWORK macro starts here */
#define _RO_STD_FRAMEWORK(x)	\
public:	\
_MAKE_RO(x)();	\
virtual ~_MAKE_RO(x)();	\
_RO_DEF_ALLOCATE(x)	\
_RO_DEF_SETUP_TYPE_SPECIFICS(x)	\
_RO_DO_REGISTER(x)	\
_RO_DO_IDS(x)	\
_RO_DO_ALLOW_FRIEND(x)	\
private:
/* _RO_STD_FRAMEWORK macro ends here */

/* _RO_BEGINCTOR macro starts here */
#define _RO_BEGINCTOR(x)	\
int _MAKE_RO(x)::mClassID = kReplicaObjectUnknownClassID;	\
RNReplicaNet::s_RO_RegistryBlock *_MAKE_RO(x)::mBoundRegistryBlock = 0;	\
bool _MAKE_RO(x)::mRegistered = false;	\
int _MAKE_RO(x)::mStaticExpectedNumDataBlocks = -1;	\
_MAKE_RO(x)::_MAKE_RO(x)()	\
{
/* _RO_BEGINCTOR macro ends here */

/* _RO_ENDCTOR macro starts here */
#define _RO_ENDCTOR(x)	\
}
/* _RO_ENDCTOR macro ends here */

/* _RO_DTOR macro starts here */
#define _RO_DTOR(x)	\
_MAKE_RO(x)::~_MAKE_RO(x)()	\
{	\
}
/* _RO_DTOR macro ends here */

/* ReplicaObject reliable macros go here */

/* _RO_DO_REPLICAOBJECT_MAKERELIABLE macro starts here */
#define _RO_DO_REPLICAOBJECT_MAKERELIABLE()	\
protected:	\
virtual void _Internal_GetUpdateType(void)	\
{	\
	UpdateSetReliable();	\
}	\
public:
/* _RO_DO_REPLICAOBJECT_MAKERELIABLE macro ends here */

/* _RO_DO_REPLICAOBJECT_MAKECERTAIN macro starts here */
#define _RO_DO_REPLICAOBJECT_MAKECERTAIN()	\
protected:	\
virtual void _Internal_GetUpdateType(void)	\
{	\
	UpdateSetCertain();	\
}	\
public:
/* _RO_DO_REPLICAOBJECT_MAKECERTAIN macro ends here */

/* _RO_DO_REPLICAOBJECT_MAKEUNRELIABLE macro starts here */
#define _RO_DO_REPLICAOBJECT_MAKEUNRELIABLE()	\
protected:	\
virtual void _Internal_GetUpdateType(void)	\
{	\
	UpdateSetUnreliable();	\
}	\
public:
/* _RO_DO_REPLICAOBJECT_MAKEUNRELIABLE macro ends here */

/* _RO_DO_REPLICAOBJECT_MAKEORDERED macro starts here */
#define _RO_DO_REPLICAOBJECT_MAKEORDERED()	\
protected:	\
	virtual void _Internal_GetUpdateType(void)	\
{	\
	UpdateSetOrdered();	\
}	\
public:
/* _RO_DO_REPLICAOBJECT_MAKEORDERED macro ends here */




/* _RO_DEF_ALLOCATEFUNC macro starts here */
#define _RO_DEF_ALLOCATEFUNC(x)	\
extern RNReplicaNet::ReplicaObject * _MAKE_RO_ALLOCFUNC(x)(void);	\
extern void _MAKE_RO_DELETEFUNC(x)(RNReplicaNet::ReplicaObject *object);	\
extern void _MAKE_RO_SETCLASSIDFUNC(x)(int classid,RNReplicaNet::s_RO_RegistryBlock *theBlock);	\
extern void _MAKE_RO_REGISTERFUNC(x)(void);
/* _RO_DEF_ALLOCATEFUNC macro ends here */

/* _RO_DO_ALLOCATEFUNC macro starts here */
#define _RO_DO_ALLOCATEFUNC(x)	\
RNReplicaNet::ReplicaObject * _MAKE_RO_ALLOCFUNC(x)(void)	\
{	\
	return _MAKE_BASE(x)::AllocateForReplicaNet();	\
}	\
void _MAKE_RO_DELETEFUNC(x)(RNReplicaNet::ReplicaObject *object)	\
{	\
	_MAKE_BASE(x)::DeleteForReplicaNet(object);	\
}	\
void _MAKE_RO_SETCLASSIDFUNC(x)(int classid,RNReplicaNet::s_RO_RegistryBlock *theBlock)	\
{	\
	_MAKE_RO(x)::StaticSetClassID(classid,theBlock);	\
}	\
void _MAKE_RO_REGISTERFUNC(x)(void)	\
{	\
	_MAKE_RO(x)::StaticSetRegistered(true);	\
}
/* _RO_DO_ALLOCATEFUNC macro ends here */

/* _RO_DEF_ALLOCATE macro starts here */
#define _RO_DEF_ALLOCATE(x)	\
static _MAKE_BASE(x)* AllocateForReplicaNet();	\
static void DeleteForReplicaNet(RNReplicaNet::ReplicaObject *object);
/* _RO_DEF_ALLOCATE macro ends here */

/* _RO_DO_ALLOCATE macro starts here */
#define _RO_DO_ALLOCATE(x)	\
_MAKE_BASE(x)* _MAKE_RO(x)::AllocateForReplicaNet()	\
{	\
	return new _MAKE_BASE(x);	\
}	\
void _MAKE_RO(x)::DeleteForReplicaNet(RNReplicaNet::ReplicaObject *object)	\
{	\
	delete object;	\
}
/* _RO_DO_ALLOCATE macro ends here */

/* _RO_REGISTER macro starts here */
#define _RO_DO_REGISTER(x)	\
public:	\
virtual void SetRegistered(bool flag = true)	\
{	\
	 mRegistered = flag;	\
}	\
static void StaticSetRegistered(bool flag = true)	\
{	\
	 mRegistered = flag;	\
}	\
virtual bool GetRegistered(void)	\
{	\
	return mRegistered;	\
}	\
private:	\
	static bool mRegistered;	\
public:
/* _RO_REGISTER macro ends here */

/* _RO_DO_IDS macro starts here */
#define _RO_DO_IDS(x)	\
static void StaticSetClassID(const int class_id,RNReplicaNet::s_RO_RegistryBlock *theBlock)	\
{	\
	mClassID = class_id;	\
	mBoundRegistryBlock = theBlock;	\
}	\
virtual const int GetClassID(void)	\
{	\
	return mClassID;	\
}	\
RNReplicaNet::s_RO_RegistryBlock *GetRegistryBlock(void) const	\
{	\
	return mBoundRegistryBlock;	\
}	\
static int StaticGetClassID(void)	\
{	\
	return mClassID;	\
}	\
static RNReplicaNet::s_RO_RegistryBlock *StaticGetRegistryBlock(void)	\
{	\
	return mBoundRegistryBlock;	\
}	\
private:	\
	static int mClassID;	\
	static RNReplicaNet::s_RO_RegistryBlock *mBoundRegistryBlock;	\
public:
/* _RO_DO_IDS macro ends here */

/* _RO_DO_ALLOW_PUBLIC macro starts here */
#define _RO_DO_ALLOW_PUBLIC()	\
public:
/* _RO_DO_ALLOW_PUBLIC macro ends here */

/* _RO_DO_MAKE_PRIVATE macro starts here */
#define _RO_DO_MAKE_PRIVATE()	\
private:
/* _RO_DO_MAKE_PRIVATE macro ends here */

/* _RO_DO_ALLOW_FRIEND macro starts here */
#define _RO_DO_ALLOW_FRIEND(x)	\
friend class _MAKE_BASE(x);
/* _RO_DO_ALLOW_FRIEND macro ends here */

/* _RO_DO_ALLOW_FRIEND_RO macro starts here */
#define _RO_DO_ALLOW_FRIEND_RO(x)	\
friend class _MAKE_RO(x);
/* _RO_DO_ALLOW_FRIEND_RO macro ends here */

/* _RO_DO_PUBLIC_RO macro starts here */
#define _RO_DO_PUBLIC_RO(x)	\
public _MAKE_RO(x)
/* _RO_DO_PUBLIC_RO macro ends here */

/* _RO_DO_APPROVEFAULT macro starts here */
#define _RO_DO_APPROVEFAULT()	\
public:	\
virtual bool ApproveFault(void)	\
{	\
	return true;	\
}	\
private:
/* _RO_DO_APPROVEFAULT macro ends here */

/* _RO_DO_APPROVEMIGRATION macro starts here */
#define _RO_DO_APPROVEMIGRATION()	\
public:	\
virtual bool ApproveMigration(void)	\
{	\
	return true;	\
}	\
private:
/* _RO_DO_APPROVEMIGRATION macro ends here */


/* _RO_DO_SETUPLOADBALANCE macro starts here */
#define _RO_DO_SETUPLOADBALANCE(x)	\
public:	\
virtual void _Internal_SetupLoadBalance(void)	\
{	\
	SetLoadBalancing(x);	\
}	\
private:
/* _RO_DO_SETUPLOADBALANCE macro ends here */

/* _RO_DO_SETUPLOADBALANCEDEFAULT macro starts here */
#define _RO_DO_SETUPLOADBALANCEDEFAULT(x)	\
public:	\
virtual void _Internal_SetupLoadBalanceDefault(void)	\
{	\
	SetLoadBalancingDefault(x);	\
}	\
private:
/* _RO_DO_SETUPLOADBALANCEDEFAULT macro ends here */

/* _RO_DO_SETUPLOADBALANCESCORE macro starts here */
#define _RO_DO_SETUPLOADBALANCESCORE(x)	\
public:	\
virtual void _Internal_SetupLoadBalanceScore(void)	\
{	\
	SetLoadScore(x);	\
}	\
private:
/* _RO_DO_SETUPLOADBALANCESCORE macro ends here */

/* _RO_DO_SETUPLOADBALANCESCOREDEFAULT macro starts here */
#define _RO_DO_SETUPLOADBALANCESCOREDEFAULT(x)	\
public:	\
virtual void _Internal_SetupLoadBalanceScoreDefault(void)	\
{	\
	SetLoadScoreDefault(x);	\
}	\
private:
/* _RO_DO_SETUPLOADBALANCESCOREDEFAULT macro ends here */


// Distance propagate functions
/* _RO_DO_SETUPPROPAGATETOSESSIONDISTANCERECALCULATIONDELAY macro starts here */
#define _RO_DO_SETUPPROPAGATETOSESSIONDISTANCERECALCULATIONDELAY(x)	\
public:	\
virtual void _Internal_SetupPropagateToSessionDistanceRecalculationDelay(void)	\
{	\
	SetPropagateToSessionDistanceRecalculationDelay(x);	\
}	\
private:
/* _RO_DO_SETUPPROPAGATETOSESSIONDISTANCERECALCULATIONDELAY macro ends here */

/* _RO_DO_SETUPPROPAGATEDISTANCE macro starts here */
#define _RO_DO_SETUPPROPAGATEDISTANCE(x)	\
public:	\
virtual void _Internal_SetupPropagateDistance(void)	\
{	\
	SetPropagateToSessionDistance(x);	\
}	\
private:
/* _RO_DO_SETUPPROPAGATEDISTANCE macro ends here */

/* _RO_DO_SETUPPROPAGATEDISTANCEDEFAULT macro starts here */
#define _RO_DO_SETUPPROPAGATEDISTANCEDEFAULT(x)	\
public:	\
virtual void _Internal_SetupPropagateDistanceDefault(void)	\
{	\
	SetPropagateToSessionDistanceDefault(x);	\
}	\
private:
/* _RO_DO_SETUPPROPAGATEDISTANCEDEFAULT macro ends here */

/* _RO_DO_SETUPPROPAGATEDISTANCEINCLUDE macro starts here */
#define _RO_DO_SETUPPROPAGATEDISTANCEINCLUDE(x)	\
public:	\
virtual void _Internal_SetupPropagateDistanceInclude(void)	\
{	\
	SetPropagateToSessionDistanceInclude(x);	\
}	\
private:
/* _RO_DO_SETUPPROPAGATEDISTANCEINCLUDE macro ends here */

/* _RO_DO_SETUPPROPAGATEDISTANCEINCLUDEDEFAULT macro starts here */
#define _RO_DO_SETUPPROPAGATEDISTANCEINCLUDEDEFAULT(x)	\
public:	\
virtual void _Internal_SetupPropagateDistanceIncludeDefault(void)	\
{	\
	SetPropagateToSessionDistanceIncludeDefault(x);	\
}	\
private:
/* _RO_DO_SETUPPROPAGATEDISTANCEINCLUDEDEFAULT macro ends here */

/* _RO_DO_SETUPPROPAGATEDISTANCEEXCLUDE macro starts here */
#define _RO_DO_SETUPPROPAGATEDISTANCEEXCLUDE(x)	\
public:	\
virtual void _Internal_SetupPropagateDistanceExclude(void)	\
{	\
	SetPropagateToSessionDistanceExclude(x);	\
}	\
private:
/* _RO_DO_SETUPPROPAGATEDISTANCEEXCLUDE macro ends here */

/* _RO_DO_SETUPPROPAGATEDISTANCEEXCLUDEDEFAULT macro starts here */
#define _RO_DO_SETUPPROPAGATEDISTANCEEXCLUDEDEFAULT(x)	\
public:	\
virtual void _Internal_SetupPropagateDistanceExcludeDefault(void)	\
{	\
	SetPropagateToSessionDistanceExcludeDefault(x);	\
}	\
private:
/* _RO_DO_SETUPPROPAGATEDISTANCEEXCLUDEDEFAULT macro ends here */

/* _RO_DO_SETUPBAND macro starts here */
#define _RO_DO_SETUPBAND(x)	\
public:	\
virtual void _Internal_SetupBand(void)	\
{	\
	SetBand(x);	\
}	\
private:
/* _RO_DO_SETUPBAND macro ends here */


// Register DataBlocks functions

/* _RO_DEF_REGISTERDATABLOCKS macro starts here */
#define _RO_DEF_REGISTERDATABLOCKS(x)	\
void RegisterDataBlocks(void);
/* _RO_DEF_REGISTERDATABLOCKS macro ends here */

/* _RO_DO_BEGINREGISTERDATABLOCKS macro starts here */
#define _RO_DO_BEGINREGISTERDATABLOCKS(x)	\
void _MAKE_RO(x)::RegisterDataBlocks(void)	\
{	\
	_MAKE_BASE(x) *basepoint = (_MAKE_BASE(x) *)this;	\
	basepoint = basepoint;
/* _RO_DO_BEGINREGISTERDATABLOCKS macro ends here */


// Include visual debugger information if required
/* _RO_DO_SETDATABLOCKVARIABLENAME macro starts here */
#ifdef REPLICANET_VISUALDEBUGGER
#define _RO_DO_SETDATABLOCKVARIABLENAME(x)	\
	datablock->SetVariableName(#x);
#else
#define _RO_DO_SETDATABLOCKVARIABLENAME(x)
#endif
/* _RO_DO_SETDATABLOCKVARIABLENAME macro starts here */


/* _RO_DO_DATABLOCK_MAKERELIABLE macro starts here */
#define _RO_DO_DATABLOCK_MAKERELIABLE()	\
datablock->SetReliableFlag(true);
/* _RO_DO_DATABLOCK_MAKERELIABLE macro ends here */

/* _RO_DO_DATABLOCK_MAKECERTAIN macro starts here */
#define _RO_DO_DATABLOCK_MAKECERTAIN()	\
datablock->SetCertainFlag(true);
/* _RO_DO_DATABLOCK_MAKECERTAIN macro ends here */

/* _RO_DO_DATABLOCK_MAKEORDERED macro starts here */
#define _RO_DO_DATABLOCK_MAKEORDERED()	\
datablock->SetOrderedFlag(true);
/* _RO_DO_DATABLOCK_MAKEORDERED macro ends here */

/* _RO_DO_DATABLOCK_MAKEORDERED macro starts here */
#define _RO_DO_DATABLOCK_SETDISCARDOLDER(x)	\
	datablock->SetDiscardOlder(x);
/* _RO_DO_DATABLOCK_SETDISCARDOLDER macro ends here */


/* _RO_DO_DATABLOCK_MAKEUNRELIABLE macro starts here */
#define _RO_DO_DATABLOCK_MAKEUNRELIABLE()	\
datablock->SetReliableFlag(false);	\
datablock->SetCertainFlag(false);	\
datablock->SetOrderedFlag(false);
/* _RO_DO_DATABLOCK_MAKEUNRELIABLE macro ends here */

/* _RO_DO_DATABLOCK_MAKEAUTOMATIC macro starts here */
#define _RO_DO_DATABLOCK_MAKEAUTOMATIC()	\
datablock->SetAutomaticUpdatesFlag(true);
/* _RO_DO_DATABLOCK_MAKEAUTOMATIC macro ends here */

/* _RO_DO_DATABLOCK_MAKEMANUAL macro starts here */
#define _RO_DO_DATABLOCK_MAKEMANUAL()	\
datablock->SetAutomaticUpdatesFlag(false);
/* _RO_DO_DATABLOCK_MAKEMANUAL macro ends here */

/* _RO_DO_DATABLOCK_FORCEREFLECTIONUPDATE macro starts here */
#define _RO_DO_DATABLOCK_FORCEREFLECTIONUPDATE()	\
	datablock->SetForceReflectionUpdate(true);
/* _RO_DO_DATABLOCK_FORCEREFLECTIONUPDATE macro ends here */


/* _RO_DO_DATABLOCK_REFLECTIONMASTERUPDATE macro starts here */
#define _RO_DO_DATABLOCK_REFLECTIONMASTERUPDATE()	\
	datablock->SetReflectionMasterUpdate(true);
/* _RO_DO_DATABLOCK_REFLECTIONMASTERUPDATE macro ends here */

/* _RO_DO_DATABLOCK_SETUPDATEDELAYMINIMUM macro starts here */
#define _RO_DO_DATABLOCK_SETUPDATEDELAYMINIMUM(x)	\
datablock->SetUpdateDelayMinimum(x);
/* _RO_DO_DATABLOCK_SETUPDATEDELAYMINIMUM macro ends here */

/* _RO_DO_DATABLOCK_SETFORCESENDMAXIMUMUPDATES macro starts here */
#define _RO_DO_DATABLOCK_SETFORCESENDMAXIMUMUPDATES(x)	\
datablock->SetForceSendMaximumUpdates(x);
/* _RO_DO_DATABLOCK_SETFORCESENDMAXIMUMUPDATES macro ends here */

/* _RO_DO_DATABLOCK_SETFORCESENDTIMEDELAY macro starts here */
#define _RO_DO_DATABLOCK_SETFORCESENDTIMEDELAY(x)	\
datablock->SetForceSendTimeDelay(x);
/* _RO_DO_DATABLOCK_SETFORCESENDTIMEDELAY macro ends here */

/* _RO_DO_DATABLOCK_SETBAND macro starts here */
#define _RO_DO_DATABLOCK_SETBAND(x)	\
datablock->SetBand(x);
/* _RO_DO_DATABLOCK_SETBAND macro ends here */


/* _RO_DO_ENDREGISTERDATABLOCKS macro starts here */
#define _RO_DO_ENDREGISTERDATABLOCKS(x)	\
}
/* _RO_DO_ENDREGISTERDATABLOCKS macro ends here */

// The macros that handle individual data block registrations are in each data block include file

/* _RO_DO_DEFBLOCK_FUNCTION_VAR macro starts here */
#define _RO_DO_DEFBLOCK_FUNCTION_VAR(x)	\
	void *mDataBlockFunction_##x;	\
	static void x(void *classp,void *opaque_message,void *thisDataBlock);
/* _RO_DO_DEFBLOCK_FUNCTION_VAR macro ends here */

/* GLOBAL_FUNCTION_CALL macro starts here */
#define GLOBAL_FUNCTION_CALL(x)	\
{x	;}	\
{Call_##x	;};
/* GLOBAL_FUNCTION_CALL macro ends here */

/* ALL_REPLICAS_FUNCTION_CALL macro starts here */
#define ALL_REPLICAS_FUNCTION_CALL(x)	\
{Call_##x	;};
/* ALL_REPLICAS_FUNCTION_CALL macro ends here */

/* MASTER_FUNCTION_CALL macro starts here */
#define MASTER_FUNCTION_CALL(x)	\
{Call_##x	;};
/* MASTER_FUNCTION_CALL macro ends here */

/* ONLY_MASTER_FUNCTION_CALL macro starts here */
#define ONLY_MASTER_FUNCTION_CALL(x)	\
{if (IsMaster()) \
{x	;} else { Call_##x ;}\
};
/* ONLY_MASTER_FUNCTION_CALL macro ends here */

/* NOMINATED_REPLICAS_FUNCTION_CALL macro starts here */
#define NOMINATED_REPLICAS_FUNCTION_CALL(a,b,x)	\
RNReplicaNet::DataBlock_Function::SetSessionIDFilter(a,b);	\
{Call_##x	;};
/* NOMINATED_REPLICAS_FUNCTION_CALL macro ends here */

/* Macros used outside of object classes */
#ifdef REPLICANET_VISUALDEBUGGER
#define _RO_DEF_DO_PREDEFREGBLOCK(x)	\
theBlock = new RNReplicaNet::s_RO_RegistryBlock(ro_id,objectfuncp,objectfuncp_delete, #x );
#else
#define _RO_DEF_DO_PREDEFREGBLOCK(x)	\
theBlock = new RNReplicaNet::s_RO_RegistryBlock(ro_id,objectfuncp,objectfuncp_delete, 0 );
#endif		// ifdef REPLICANET_VISUALDEBUGGER
/* _RO_DEF_DO_PREDEF macro starts here */
#define _RO_DEF_DO_PREDEF(x)	\
_RO_DEF_ALLOCATEFUNC(x);	\
_RO_DEF_DO_PREDEF2(x)
/* _RO_DEF_DO_PREDEF macro ends here */

/* _RO_DEF_DO_PREDEF2 macro starts here */
#define _RO_DEF_DO_PREDEF2(x)	\
objectfuncp = _MAKE_RO_ALLOCFUNC(x);	\
objectfuncp_delete = _MAKE_RO_DELETEFUNC(x);	\
_RO_DEF_DO_PREDEFREGBLOCK(x);	\
_MAKE_RO_SETCLASSIDFUNC(x)(ro_id,theBlock);	\
repNet->AddItem(theBlock);	\
_MAKE_RO_REGISTERFUNC(x)();	\
ro_id++;
/* _RO_DEF_DO_PREDEF2 macro ends here */

#define _RO_DO_PREREGISTER_FUNC()	\
static bool doneregister = false;	\
void _ReplicaNetPreRegisterReplicasFunc(RNReplicaNet::ReplicaNet *repNet)	\
{	\
	RNReplicaNet::ReplicaNetDLLRegister thing;	\
	thing.Call(repNet);	\
}	\
void RNReplicaNet::ReplicaNetDLLRegister::Call(RNReplicaNet::ReplicaNet *repNet)	\
{	\
	doneregister = true;	\
	RNReplicaNet::s_RO_RegistryBlock *theBlock = 0;	\
	RNReplicaNet::p_RO_Func *objectfuncp = 0;	\
	RNReplicaNet::p_RO_FuncDelete *objectfuncp_delete = 0;	\
	int ro_id = 0;	\
	ro_id = ro_id;	\
	objectfuncp_delete = objectfuncp_delete;	\
	objectfuncp = objectfuncp;	\
	theBlock = theBlock;

// The extra bits added above are to avoid the compiler warnings for empty lists of classes to register

#define _RO_DO_PREREGISTER_FUNC_END()	\
}


/* _RO_DEF_REPLICAOBJECT_INTERNAL_SETUPOPAQUEPOINTER macro starts here */
#define _RO_DEF_REPLICAOBJECT_INTERNAL_SETUPOPAQUEPOINTER()	\
protected:	\
virtual void _Internal_SetupOpaquePointer(void);	\
public:
/* _RO_DEF_REPLICAOBJECT_INTERNAL_SETUPOPAQUEPOINTER macro ends here */

/* _RO_DO_REPLICAOBJECT_INTERNAL_SETUPOPAQUEPOINTER macro starts here */
#define _RO_DO_REPLICAOBJECT_INTERNAL_SETUPOPAQUEPOINTER(x,y)	\
void _MAKE_RO(x)::_Internal_SetupOpaquePointer(void)	\
{	\
	SetOpaquePointer((void *)((y *)((_MAKE_BASE(x) *)this)));	\
}
/* _RO_DO_REPLICAOBJECT_INTERNAL_SETUPOPAQUEPOINTER macro ends here */



/* _RO_DEF_SETUP_TYPE_SPECIFICS macro starts here */
#define _RO_DEF_SETUP_TYPE_SPECIFICS(x)	\
private:	\
	static int mStaticExpectedNumDataBlocks;	\
protected:	\
static void StaticSetExpectedNumDataBlocks(const int expected) {mStaticExpectedNumDataBlocks = expected;}	\
static int StaticGetExpectedNumDataBlocks(void) { return mStaticExpectedNumDataBlocks; }	\
virtual void SetNumExpectedDataBlocks(const int expected) { StaticSetExpectedNumDataBlocks(expected); }	\
virtual int GetNumExpectedDataBlocks(void) { return StaticGetExpectedNumDataBlocks(); }	\
private:
/* _RO_DEF_SETUP_TYPE_SPECIFICS macro ends here */

#endif
