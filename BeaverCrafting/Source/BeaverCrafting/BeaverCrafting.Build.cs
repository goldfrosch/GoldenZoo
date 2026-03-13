using UnrealBuildTool;

public class BeaverCrafting : ModuleRules
{
	public BeaverCrafting(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"GameplayTags"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new[]
			{
				"AssetRegistry",
				"Squirreltem"
			}
		);
	}
}
