// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class FrogJumpUI : ModuleRules
{
	public FrogJumpUI(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
			}
			);
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"UMG",
				"EnhancedInput",
				"DeveloperSettings"
			}
			);
	}
}
