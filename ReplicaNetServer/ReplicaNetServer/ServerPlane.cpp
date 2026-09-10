/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
/*
The Plane game object. Every tick the plane will move a little furthur around in a circle.
This object have corresponding ROL files in the ROLFiles folder. The ROL files are compiled by the ROLCompiler.
The CompiledGameObjects folder contains the compiled ROL definition file for this object. The member variables listed
in the ROL file for this class are automatically updated when this object is allocated on another machine
*/
#include "Plane.h"
#include "RNReplicaNet/Inc/ReplicaNet.h"

#ifndef PI
#define PI 3.14159265358979323846
#endif

Plane::Plane()
{
	static int sID = 0;
	mID = sID++;

	mReplica = this;		// Setup our forward reference pointer
	mPosition = D3DXVECTOR4(0,0,0,0);
	mRotation = D3DXVECTOR4(0,0,0,0);

	mPlayerNumber = rand() & 7;
	mPlayerColour = rand() & 7;
	mPlayerEnergy = 1.0f;
	mFunctionXRot = 0.0f;
}

Plane::~Plane()
{
}

void Plane::PollIt(void)
{
	// Only run the "physics" if we are a master object
	if (!IsMaster())
	{
		return;
	}

	double time = GetBoundReplicaNet()->GetTime() + mTimeOffset;

	mPosition.x = mOriginalPosition.x + float(sin(time * mTimeScaleX) * mPosScaleX);
	mPosition.y = mOriginalPosition.y + float(sin(time * mTimeScaleY) * mPosScaleY);
	mPosition.z = mOriginalPosition.z + float(cos(time * mTimeScaleZ) * mPosScaleZ);

	mRotation.y = float(time * 10.0f);
}
