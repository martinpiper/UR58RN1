// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class UR58RN1EditorTarget : TargetRules
{
	public UR58RN1EditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V7;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_8;
		ExtraModuleNames.Add("UR58RN1");
        bOverrideBuildEnvironment = true;
        AdditionalCompilerArguments = "/wd4456";
	}
}
