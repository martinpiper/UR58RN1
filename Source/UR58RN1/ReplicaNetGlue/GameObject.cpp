/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
/*
This file is the base class for any type of game object.
When an object is allocated or deleted the gGameObjects array tracks the list of allocated objects
*/

#include "GameObject.h"
#include "RNReplicaNet/Inc/ReplicaNet.h"
#include "RNReplicaNet/Inc/ReplicaObject.h"

std::set<GameObject *> gGameObjects;

RNReplicaNet::ReplicaNet* gNetwork = 0;

void NetworkClientInit(void)
{
	if (!gNetwork)
	{
		gNetwork = new RNReplicaNet::ReplicaNet();

		gNetwork->SetAutomaticPoll();
		// Disconnect ReplicaNet polling from the session and transport threads.
		gNetwork->SetPollLayerBelow(false);
		gNetwork->SetPollLayerBelowForXPSession(false);

		gNetwork->SessionFind();
		// Wait a while
		Sleep(500);
		std::string url = gNetwork->SessionEnumerateFound();

		if (!url.empty())
		{
			char buffer[256];
			sprintf(buffer, "Trying to join %s\n", url.c_str());
			OutputDebugStringA(buffer);
			gNetwork->SessionJoin(url);
		}
	}
}

GameObject::GameObject() : mDeleteMe(false) , mReplica(0) , mOwned(false)
{
	// Lock our own object list on delete
	gNetwork->LockObjects();

	gGameObjects.insert(this);

	// UnLock our own object list on delete
	gNetwork->UnLockObjects();
};

GameObject::~GameObject()
{
	// Lock our own object list on delete
	gNetwork->LockObjects();

	gGameObjects.erase(this);

	// UnLock our own object list on delete
	gNetwork->UnLockObjects();
};

void GameObject::PollIt(void)
{
	printf("Default PollIt()\n");
};

void GameObject::Delete(void)
{
	mDeleteMe = true;
}


GameObject* FindUnownedGameObject(void)
{
	GameObject* ret = 0;
	// Lock our own object list on delete
	gNetwork->LockObjects();

	auto st = gGameObjects.begin();
	while (st != gGameObjects.end())
	{
		GameObject* test = *st;
		if (!test->mOwned)
		{
			ret = test;
			ret->mOwned = true;
			break;
		}
		st++;
	}

	// UnLock our own object list on delete
	gNetwork->UnLockObjects();

	return ret;
}

void PollGameOjects(void)
{
	// Lock our own object list on delete
	gNetwork->LockObjects();

	auto st = gGameObjects.begin();
	while (st != gGameObjects.end())
	{
		GameObject* test = *st;
		test->PollIt();
		st++;
	}

	// UnLock our own object list on delete
	gNetwork->UnLockObjects();
}
