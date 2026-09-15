/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
/*
The camera object. Typically this object is very simple so it does not require a cpp file
*/
#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "GameObject.h"
#include "_RO_Camera.h"

class Camera : _RO_DO_PUBLIC_RO(Camera) , public GameObject
{
public:
	Camera()
	{
		mReplica = this;		// Setup our forward reference pointer
		mPosition = D3DXVECTOR4(0,0,0,0);
	}

	virtual ~Camera()
	{
	}

	CALCULATE_DISTANCE();

	void PollIt(void)
	{
	}

	bool GetPosition(D3DXVECTOR4 &input)
	{
		input = mPosition;
		return true;
	}

	D3DXVECTOR4 mPosition;
};

#endif
