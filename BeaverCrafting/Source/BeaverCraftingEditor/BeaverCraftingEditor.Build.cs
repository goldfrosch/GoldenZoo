using UnrealBuildTool;

public class BeaverCraftingEditor : ModuleRules
{
	public BeaverCraftingEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new[]
			{
				"Core",
				"DataTableEditor"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new[]
			{
				"CoreUObject",
				"Engine",
				"UnrealEd",
				"DeveloperSettings",
				"InputCore",
				"Slate",
				"SlateCore",
				"BeaverCrafting"
			}
		);
	}
}
