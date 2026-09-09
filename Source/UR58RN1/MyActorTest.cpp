// Fill out your copyright notice in the Description page of Project Settings.


#include "MyActorTest.h"
#include "ReplicaNetGlue/GameObject.h"
#include "ReplicaNetGlue/Plane.h"

// Sets default values
AMyActorTest::AMyActorTest()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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
		mNetworkObject = FindUnownedGameObject();
	}

	Super::Tick(DeltaTime);

	if (mNetworkObject)
	{
		if (mNetworkObject->mReplica)
		{
			if (mNetworkObject->mReplica->GetClassID() == Plane::StaticGetClassID())
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
	}
}
