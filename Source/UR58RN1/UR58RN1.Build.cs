// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UR58RN1 : ModuleRules
{
	public UR58RN1(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"UR58RN1",
			"UR58RN1/ReplicaNetPublic/Includes",
			"UR58RN1/ReplicaNetGlue",
			"UR58RN1/Variant_Platforming",
			"UR58RN1/Variant_Platforming/Animation",
			"UR58RN1/Variant_Combat",
			"UR58RN1/Variant_Combat/AI",
			"UR58RN1/Variant_Combat/Animation",
			"UR58RN1/Variant_Combat/Gameplay",
			"UR58RN1/Variant_Combat/Interfaces",
			"UR58RN1/Variant_Combat/UI",
			"UR58RN1/Variant_SideScrolling",
			"UR58RN1/Variant_SideScrolling/AI",
			"UR58RN1/Variant_SideScrolling/Gameplay",
			"UR58RN1/Variant_SideScrolling/Interfaces",
			"UR58RN1/Variant_SideScrolling/UI"
		});

        PublicDefinitions.Add("_CRT_SECURE_NO_WARNINGS");

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
