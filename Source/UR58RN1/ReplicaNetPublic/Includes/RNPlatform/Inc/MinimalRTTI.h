/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef _RN_MINIMAL_RTTI_H_
#define _RN_MINIMAL_RTTI_H_

// Should be in the public section.
/* RN_CLASS_MINIMAL_RTTI macro starts here */
#define RN_CLASS_MINIMAL_RTTI() \
	static int StaticThisClassType(void)	\
	{	\
		static int thisClassType = GetNewClassType();	\
		return thisClassType;	\
	}	\
	virtual int GetThisClassType(void)	\
	{	\
		return StaticThisClassType();	\
	}
/* RN_CLASS_MINIMAL_RTTI macro ends here */


// Should be in the public section of the base class.
/* RN_CLASS_MINIMAL_RTTI_BASE_CLASS macro starts here */
#define RN_CLASS_MINIMAL_RTTI_BASE_CLASS() \
bool IsClassType(const int targetClassType)	\
{	\
	int thisType = GetThisClassType();	\
	if (thisType == targetClassType)	\
	{	\
		return true;	\
	}	\
	return false;	\
}	\
protected:	\
static int GetNewClassType(void)	\
{	\
	static int classTypeCount = 0;	\
	return classTypeCount++;	\
}	\
public:
/* RN_CLASS_MINIMAL_RTTI_BASE_CLASS macro ends here */


namespace RNReplicaNet
{


} // namespace RNReplicaNet

#endif
