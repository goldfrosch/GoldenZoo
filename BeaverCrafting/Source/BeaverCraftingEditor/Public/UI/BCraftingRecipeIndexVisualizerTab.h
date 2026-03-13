#pragma once

#include "CoreMinimal.h"
#include "Search/BCraftingRecipeIndex.h"

class FSpawnTabArgs;
class FTabManager;
class FWorkspaceItem;
class IAssetEditorInstance;
class ITableRow;
class SDockTab;
class STableViewBase;
template<typename ItemType> class STreeView;
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
	struct FEditorTabState
	{
		TWeakObjectPtr<UDataTable> SourceTable;
		FBCraftingRecipeIndex RecipeIndex;
		TArray<FTreeNodePtr> RootItems;
		TSharedPtr<STreeView<FTreeNodePtr>> TreeView;
		TWeakPtr<SDockTab> DockTab;
		TSharedPtr<FWorkspaceItem> WorkspaceCategory;
		bool bHasData = false;
	};

	void HandleAssetOpened(UObject* Asset, IAssetEditorInstance* AssetEditorInstance);
	void HandleAssetClosed(UObject* Asset, IAssetEditorInstance* AssetEditorInstance);
	void RegisterLocalTab(UDataTable* SourceTable, IAssetEditorInstance* AssetEditorInstance);
	void UnregisterLocalTab(IAssetEditorInstance* AssetEditorInstance);
	FEditorTabState* FindState(IAssetEditorInstance* AssetEditorInstance);
	const FEditorTabState* FindState(IAssetEditorInstance* AssetEditorInstance) const;
	TSharedRef<SDockTab> SpawnTab(const FSpawnTabArgs& Args, IAssetEditorInstance* AssetEditorInstance);
	TSharedRef<SWidget> BuildTabContent(IAssetEditorInstance* AssetEditorInstance);
	FTreeNodePtr BuildTreeNode(const FEditorTabState& State, const TSharedPtr<const FBCraftingRecipeNode>& SourceNode, const FText& Label) const;
	void RebuildTreeItems(FEditorTabState& State) const;
	void GetChildrenForTree(IAssetEditorInstance* AssetEditorInstance, FTreeNodePtr InItem, TArray<FTreeNodePtr>& OutChildren) const;
	TSharedRef<ITableRow> GenerateTreeRow(IAssetEditorInstance* AssetEditorInstance, FTreeNodePtr InItem, const TSharedRef<STableViewBase>& OwnerTable) const;
	FText GetHeaderText(IAssetEditorInstance* AssetEditorInstance) const;
	bool IsSupportedRecipeTable(const UObject* Asset) const;

	TMap<IAssetEditorInstance*, FEditorTabState> EditorTabStates;
	FDelegateHandle AssetOpenedHandle;
	FDelegateHandle AssetClosedHandle;
};
