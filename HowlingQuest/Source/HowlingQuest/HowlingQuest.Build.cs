using UnrealBuildTool;

public class HowlingQuest : ModuleRules
{
	public HowlingQuest(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"AssetRegistry",
				"DeveloperSettings",
				"GameplayTags",
				"StructUtils"
			}
		);
	}
}
