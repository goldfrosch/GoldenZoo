#pragma once

#include "Modules/ModuleManager.h"

class FBCraftingRecipeDataTableEditorExtension;
class FBCraftingRecipeIndexVisualizerTab;

class FBeaverCraftingEditorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	TUniquePtr<FBCraftingRecipeDataTableEditorExtension> RecipeDataTableEditorExtension;
	TUniquePtr<FBCraftingRecipeIndexVisualizerTab> RecipeIndexVisualizerTab;
};
