// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstance.h"
#include "MyActorTest.h"

UMyGameInstance::UMyGameInstance()
{
	// Ensure consistent existing object ID mapping to network object mapping.
	// This handles cases where actors are allocayed for the editor view, then an in editor game view is created.
	AMyActorTest::sID = 0;
}
