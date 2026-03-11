#pragma once

#include "Delegates/Delegate.h"

class FExtender;
class FBCraftingRecipeIndexVisualizerTab;
class FUICommandList;
class FToolBarBuilder;
class UDataTable;

class FBCraftingRecipeDataTableEditorExtension
{
public:
	explicit FBCraftingRecipeDataTableEditorExtension(FBCraftingRecipeIndexVisualizerTab& InVisualizerTab);

	void Register();
	void Unregister();

private:
	TSharedRef<FExtender> ExtendRecipeDataTableToolbar(const TSharedRef<FUICommandList> CommandList, const TArray<UObject*> ContextObjects);
	void AddRecipeDataTableToolbar(FToolBarBuilder& ToolbarBuilder, TWeakObjectPtr<UDataTable> RecipeTable);
	void HandleRecipeToolbarButtonClicked(TWeakObjectPtr<UDataTable> RecipeTable) const;
	static UDataTable* FindRecipeDataTable(const TArray<UObject*>& ContextObjects);

	FBCraftingRecipeIndexVisualizerTab& VisualizerTab;
	FDelegateHandle DataTableToolbarExtenderHandle;
};
