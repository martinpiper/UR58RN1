// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstance.h"
#include "MyActorTest.h"
#include "ReplicaNetGlue/GameObject.h"
#include "RNReplicaNet/Inc/ReplicaNet.h"


UMyGameInstance::UMyGameInstance()
{
	// Ensure consistent existing object ID mapping to network object mapping.
	// This handles cases where actors are allocated for the editor view, then an in editor game view is created.
	AMyActorTest::sID = 0;
}

void UMyGameInstance::Init()
{
	AMyActorTest::sID = 0;

	NetworkClientInit();

	PreTickHandle = FWorldDelegates::OnWorldPreActorTick.AddUObject(this, &UMyGameInstance::MyPreTickFunction);
	PreTickHandle = FWorldDelegates::OnWorldPostActorTick.AddUObject(this, &UMyGameInstance::MyPostTickFunction);
}

void UMyGameInstance::Shutdown()
{
	NetworkClientDisconnect();
}

void UMyGameInstance::MyPreTickFunction(UWorld* World, ELevelTick TickType, float DeltaSeconds)
{
	if (!World) return;
	if (!gNetwork) return;

	// Update network object in synch with the game world tick
	gNetwork->Poll();
	// Ensure that network objects cannot disappear while actors are using them
	gNetwork->LockObjects();
}

void UMyGameInstance::MyPostTickFunction(UWorld* World, ELevelTick TickType, float DeltaSeconds)
{
	if (!World) return;
	if (!gNetwork) return;

	// Unlock objects again
	gNetwork->UnLockObjects();
}
