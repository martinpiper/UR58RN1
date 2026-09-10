/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
/*
The base class definition for game objects. See GameObjkect.cpp
*/
#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#include <set>

class GameObject;
namespace RNReplicaNet
{
	class ReplicaNet;
	class ReplicaObject;
}

extern std::set<GameObject *> gGameObjects;
extern RNReplicaNet::ReplicaNet* gNetwork;

void NetworkClientInit(void);
void NetworkClientDisconnect(void);

typedef struct D3DXVECTOR4
{
	D3DXVECTOR4() : x(0.0f) , y(0.0f) , z(0.0f) , w(0.0f)
	{}

	D3DXVECTOR4(float x, float y, float z, float w)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}

	float x;
	float y;
	float z;
	float w;
} D3DXVECTOR4, * LPD3DXVECTOR4;

class GameObject
{
public:
	GameObject();

	virtual ~GameObject();

	virtual void PollIt(void);

	virtual D3DXVECTOR4 GetPosition(void)
	{
		return D3DXVECTOR4(0,0,0,0);
	}


	/**
	 * A delete method for a game object since deletion of a class while in a member function is not very good design
	 */
	void Delete(void);

	bool mDeleteMe;

	/*
	A pointer held by the class that points to the derived class of this object for forward references
	*/
	RNReplicaNet::ReplicaObject *mReplica;

	bool mOwned;

	int mID = -1;
};

GameObject* FindUnownedGameObject(const int ofType, const int withID);
void PollGameOjects(void);

#endif
