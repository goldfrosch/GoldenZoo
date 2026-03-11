#pragma once

#include "CoreMinimal.h"
#include "Search/BCraftingRecipeIndex.h"
#include "Widgets/Views/STreeView.h"

class SDockTab;
class UDataTable;

class FBCraftingRecipeIndexVisualizerTab
{
public:
	void Register();
	void Unregister();
	void ShowRecipeIndex(UDataTable* SourceTable, const FBCraftingRecipeIndex& RecipeIndex);

private:
	struct FTreeNode;
	using FTreeNodePtr = TSharedPtr<FTreeNode>;

	TSharedRef<SDockTab> SpawnTab(const FSpawnTabArgs& Args);
	TSharedRef<SWidget> BuildTabContent();
	FTreeNodePtr BuildTreeNode(const TSharedPtr<const FBCraftingRecipeNode>& SourceNode, const FText& Label) const;
	void RebuildTreeItems();
	void GetChildrenForTree(FTreeNodePtr InItem, TArray<FTreeNodePtr>& OutChildren) const;
	TSharedRef<ITableRow> GenerateTreeRow(FTreeNodePtr InItem, const TSharedRef<STableViewBase>& OwnerTable) const;
	FText GetHeaderText() const;

	TWeakObjectPtr<UDataTable> CurrentSourceTable;
	FBCraftingRecipeIndex CurrentRecipeIndex;
	TArray<FTreeNodePtr> RootItems;
	TSharedPtr<STreeView<FTreeNodePtr>> TreeView;
	bool bHasData = false;
};
