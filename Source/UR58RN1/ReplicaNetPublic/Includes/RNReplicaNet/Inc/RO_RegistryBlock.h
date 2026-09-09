/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef __RO_REGISTRYBLOCK_H__
#define __RO_REGISTRYBLOCK_H__

namespace RNReplicaNet
{

class ReplicaObject;

/**
 * A typedef describing a pointer to the function that can allocate a derived class of ReplicaObject
 */
typedef ReplicaObject *p_RO_Func(void);
typedef void p_RO_FuncDelete(ReplicaObject *);

/**
 * This is a registry block used when class allocator functions are registered at startup
 */
struct s_RO_RegistryBlock
{
public:
	/**
	 * This ctor fills in the parameters in the structure
	 * \param id the class id to use for this block
	 * \param pfunc the function pointer that can allocate this class id
	 * \param pfunc_delete the function that can delete this class id
	 * \param name the name of the class. The default value is null
	 */
	s_RO_RegistryBlock(int id,p_RO_Func *pfunc,p_RO_FuncDelete *pfunc_delete,const char *name = 0);

	/**
	 * Gets the class id from the structure
	 * \return the class id
	 */
	int GetClassID(void);

	/**
	 * Calls the allocate function pointer registered for this block and returns the ReplicaObject pointer
	 * \return the replica object pointer
	 */
	ReplicaObject *CallBackAllocate(void);

	/**
	 * Calls the delete function pointer registered for this block
	 * \param object the replica object pointer to delete
	 */
	void CallBackDelete(ReplicaObject *object);

	/**
	 * Gets the class name if the preprocessor define REPLICANET_VISUALDEBUGGER is used. Otherwise the return value is null.
	 * If the class name is null then the ReplicaObject will not appear in the Visual Debugger output.
	 * \return The name
	 */
	const char *GetName(void) const;

private:
	int mID;
	p_RO_Func *mFunc;
	p_RO_FuncDelete *mFuncDelete;
	const char *mName;
};

} // namespace RNReplicaNet

#endif
