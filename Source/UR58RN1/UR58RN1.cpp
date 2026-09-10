// Copyright Epic Games, Inc. All Rights Reserved.

#include "UR58RN1.h"
#include "Modules/ModuleManager.h"

class MyGameModule : public FDefaultGameModuleImpl
{
public:
	MyGameModule()
	{

	}
	virtual ~MyGameModule() = default;

	virtual void StartupModule() override
	{
	}

};
IMPLEMENT_PRIMARY_GAME_MODULE(MyGameModule, UR58RN1, "UR58RN1" );

DEFINE_LOG_CATEGORY(LogUR58RN1)
