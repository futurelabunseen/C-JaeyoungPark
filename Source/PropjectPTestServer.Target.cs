// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

[SupportedPlatforms(UnrealPlatformClass.Server)]
public class PropjectPTestServerTarget : TargetRules
{
	public PropjectPTestServerTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Server;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_1;
        ExtraModuleNames.AddRange(new string[] { "PropjectPTest", "PropjectPTestGAS" });

		bUseChecksInShipping = true;
		bUseLoggingInShipping = true;
	}
}
