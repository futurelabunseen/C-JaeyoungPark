// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class PropjectPTest : ModuleRules
{
	public PropjectPTest(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(new string[] { "PropjectPTest" });

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "NavigationSystem",
            "AIModule",
            "Niagara",
            "EnhancedInput",
            "UMG",
            "GeometryCollectionEngine"
        });

        // GAS로 사용할 것들 정의
        PrivateDependencyModuleNames.AddRange(new string[] {
            "PropjectPTest",
            "GameFeatures",
            "GameplayAbilities",
            "GameplayTasks",
            "GameplayTags"
        });

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        PrivateDependencyModuleNames.Add("OnlineSubsystem");
        PrivateDependencyModuleNames.Add("OnlineSubsystemSteam");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
