#include "BeaverCraftingEditor.h"

#include "UI/BCraftingRecipeDataTableEditorExtension.h"
#include "UI/BCraftingRecipeIndexVisualizerTab.h"
#include "Modules/ModuleManager.h"

void FBeaverCraftingEditorModule::StartupModule()
{
	RecipeIndexVisualizerTab = MakeUnique<FBCraftingRecipeIndexVisualizerTab>();
	RecipeIndexVisualizerTab->Register();

	RecipeDataTableEditorExtension = MakeUnique<FBCraftingRecipeDataTableEditorExtension>(*RecipeIndexVisualizerTab);
	RecipeDataTableEditorExtension->Register();
}

void FBeaverCraftingEditorModule::ShutdownModule()
{
	if (RecipeDataTableEditorExtension)
	{
		RecipeDataTableEditorExtension->Unregister();
		RecipeDataTableEditorExtension.Reset();
	}

	if (RecipeIndexVisualizerTab)
	{
		RecipeIndexVisualizerTab->Unregister();
		RecipeIndexVisualizerTab.Reset();
	}
}

IMPLEMENT_MODULE(FBeaverCraftingEditorModule, BeaverCraftingEditor)
