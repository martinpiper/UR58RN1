// ReplicaNetServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

#include "GameObject.h"
#include "RNReplicaNet/Inc/ReplicaNet.h"
#include "RNReplicaNet/Inc/ReplicaObject.h"
#include "Plane.h"
#include <random>

int main()
{
    std::cout << "Hello World!\n";


	gNetwork = new RNReplicaNet::ReplicaNet();

	gNetwork->SetDataBlockUpdatePolicy(false);
	gNetwork->SetAutomaticPoll();
	// Disconnect ReplicaNet polling from the session and transport threads.
	gNetwork->SetPollLayerBelow(false);
	gNetwork->SetPollLayerBelowForXPSession(false);
	gNetwork->SessionCreate("ReplicaNetServerUnreal1");

	std::random_device rd;
	std::mt19937 gen(rd());

	std::uniform_real_distribution<float> distribTimeoffset(0, 100);
	std::uniform_real_distribution<float> distribScaleX(100, 300);
	std::uniform_real_distribution<float> distribScaleY(100, 300);
	std::uniform_real_distribution<float> distribScaleZ(0, 150);	// Just up bounces

	std::uniform_real_distribution<float> distribPosX(-500, 500);
	std::uniform_real_distribution<float> distribPosY(-500, 500);
	std::uniform_real_distribution<float> distribPosZ(50, 600);

	for (auto i = 0; i < 20; i++)
	{
		Plane* object = new Plane();
		object->mTimeOffset = distribTimeoffset(gen);
		object->mPosScaleX = distribScaleX(gen);
		object->mPosScaleY = distribScaleY(gen);
		object->mPosScaleZ = distribScaleZ(gen);
		object->mTimeScaleX = 1.0f / (distribScaleX(gen) + 10.0f);
		object->mTimeScaleY = 1.0f / (distribScaleY(gen) + 10.0f);
		object->mTimeScaleZ = 1.0f / (distribScaleZ(gen) + 10.0f);
		object->SetPosition(D3DXVECTOR4(distribPosX(gen), distribPosY(gen), distribPosZ(gen), 0));
		object->Publish();
	}

	while (true)
	{
		Sleep(1000);
		printf("Time %f send %f recv %f\n" , gNetwork->GetTime(), gNetwork->GetNetworkSendRate(), gNetwork->GetNetworkReceiveRate());
		PollGameOjects();
		gNetwork->ProcessDataBlockUpdate();
	}
}
