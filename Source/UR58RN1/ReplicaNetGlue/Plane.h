/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
/*
See Plane.cpp
*/
#ifndef __PLANE_H__
#define __PLANE_H__

#include "GameObject.h"

#include "_RO_Plane.h"

class Plane : _RO_DO_PUBLIC_RO(Plane) , public GameObject
{
public:
	Plane();
	virtual ~Plane();

	void Render(void);

	void PollIt(void);

	D3DXVECTOR4 GetPosition(void)
	{
		return mPosition;
	}

	void SetPosition(D3DXVECTOR4 position)
	{
		mPosition = position;
		mOriginalPosition = position;
	}

	D3DXVECTOR4 GetRotation(void)
	{
		return mRotation;
	}

	float CalculateDistanceToObject(RNReplicaNet::ReplicaObject *object);

	bool ApproveFault(void);

	void OwnerChange(void);

	void NetworkCallAMemberFunction(float xrot)
	{
		// Double check we are a master.
		if (!IsMaster())
		{
			return;
		}
		// This function call is set to be reliable so detect a state change and call the networked function when the state changes.
		if (xrot != mFunctionXRot)
		{
			// This calls AMemberFunction(ypos) on the master and the replica.
			GLOBAL_FUNCTION_CALL(AMemberFunction(xrot));
		}
	}

	// Not directly propagated by the ROL file. Instead this variable is set by the networked function AMemberFunction().
	float mFunctionXRot;

	float mTimeOffset = 0.0f;
	float mTimeScaleX = 1.0f / 50.0f;
	float mTimeScaleY = 1.0f / 20.0f;
	float mTimeScaleZ = 1.0f / 50.0f;

	float mPosScaleX = 150.0f;
	float mPosScaleY = 25.0f;
	float mPosScaleZ = 150.0f;


/* These variables are defined in the ROL file for this object */
private:
_RO_DO_ALLOW_FRIEND_RO(Plane)
	// A networked function that is defined in the ROL file.
	void AMemberFunction(float xrot)
	{
		mFunctionXRot = xrot;
	}

	D3DXVECTOR4 mPosition;
	D3DXVECTOR4 mOriginalPosition;
	D3DXVECTOR4 mRotation;

	int mPlayerNumber;
	int mPlayerColour;
	float mPlayerEnergy;
};

#endif
