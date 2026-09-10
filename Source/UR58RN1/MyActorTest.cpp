// Fill out your copyright notice in the Description page of Project Settings.


#include "MyActorTest.h"
#include "ReplicaNetGlue/GameObject.h"
#include "ReplicaNetGlue/Plane.h"

int AMyActorTest::sID = 0;

// Sets default values
AMyActorTest::AMyActorTest()
{
	SetReplicates(false);
	SetReplicateMovement(false);
	PrimaryActorTick.bCanEverTick = true;
	mID = sID++;

	std::string name = std::string(TCHAR_TO_UTF8(*GetName()));
	char buffer[256];
	sprintf(buffer, "Name '%s' and ID %d\n", name.c_str() , mID);
	OutputDebugStringA(buffer);
}

// Called when the game starts or when spawned
void AMyActorTest::BeginPlay()
{
	NetworkClientInit();

	Super::BeginPlay();
}

void AMyActorTest::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	mNetworkObject = 0;
	NetworkClientDisconnect();

	Super::EndPlay(EndPlayReason);
}

// Called every frame
void AMyActorTest::Tick(float DeltaTime)
{
	// Try to attach any unowned network objects...
	if (!mNetworkObject)
	{
		mNetworkObject = FindUnownedGameObject(Plane::StaticGetClassID() , mID);
	}

	Super::Tick(DeltaTime);

	if (mNetworkObject)
	{
		Plane* plane = (Plane*)mNetworkObject;

		FVector NewLocation;
		NewLocation.X = plane->GetPosition().x;
		NewLocation.Y = plane->GetPosition().y;
		NewLocation.Z = plane->GetPosition().z;

		FRotator NewRotation(0, 0, 0);
		NewRotation.Yaw = plane->GetRotation().y;

		SetActorLocationAndRotation(NewLocation, NewRotation);
	}
}
