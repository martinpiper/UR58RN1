// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MyGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class UR58RN1_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	UMyGameInstance();
	void Init();
	void Shutdown();

	FDelegateHandle PreTickHandle;

	void MyPreTickFunction(UWorld* World, ELevelTick TickType, float DeltaSeconds);
	void MyPostTickFunction(UWorld* World, ELevelTick TickType, float DeltaSeconds);
};
