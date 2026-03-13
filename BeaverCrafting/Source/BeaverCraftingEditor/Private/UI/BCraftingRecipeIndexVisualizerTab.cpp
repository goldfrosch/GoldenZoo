#include "UI/BCraftingRecipeIndexVisualizerTab.h"

#include "Editor.h"
#include "Engine/DataTable.h"
#include "Framework/Docking/TabManager.h"
#include "Internationalization/Text.h"
#include "Styling/AppStyle.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "Toolkits/AssetEditorToolkit.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Views/STableRow.h"
#include "Widgets/Views/STreeView.h"

#define LOCTEXT_NAMESPACE "BeaverCraftingRecipeIndexVisualizer"

namespace
{
	const FName RecipeIndexVisualizerTabId(TEXT("BeaverCrafting.RecipeIndexVisualizer"));
	constexpr int32 FontSizeDelta = 2;

	FSlateFontInfo GetHeaderFont()
	{
		FSlateFontInfo Font = FAppStyle::Get().GetFontStyle("NormalFont");
		Font.Size += FontSizeDelta;
		return Font;
	}

	FSlateFontInfo GetTreeRowFont()
	{
		FSlateFontInfo Font = FAppStyle::Get().GetFontStyle("NormalFont");
		Font.Size += FontSizeDelta;
		return Font;
	}

	FText MakeTokenText(const FBCraftingSearchToken& Token)
	{
		if (Token.TokenType == BCraftingRecipeTokens::ProductType)
		{
			return FText::FromString(FString::Printf(TEXT("%s: %s"), *Token.TokenType.ToString(), *Token.NameValue.ToString()));
		}

		if (Token.TokenType == BCraftingRecipeTokens::ItemId)
		{
			return FText::FromString(FString::Printf(TEXT("%s [ %s ]"), *Token.TokenType.ToString(), *Token.NameValue.ToString()));
		}

		if (Token.TokenType == BCraftingRecipeTokens::ItemCount)
		{
			return FText::FromString(FString::Printf(TEXT("%s ( %d )"), *Token.TokenType.ToString(), Token.IntValue));
		}

		return FText::FromString(TEXT("Undefined Token Type"));
	}

	TArray<TPair<FBCraftingSearchToken, TSharedPtr<FBCraftingRecipeNode>>> GetSortedChildren(const TSharedPtr<const FBCraftingRecipeNode>& SourceNode)
	{
		TArray<TPair<FBCraftingSearchToken, TSharedPtr<FBCraftingRecipeNode>>> SortedChildren;
		if (!SourceNode.IsValid())
		{
			return SortedChildren;
		}

		SortedChildren.Reserve(SourceNode->Children.Num());
		for (const TPair<FBCraftingSearchToken, TSharedPtr<FBCraftingRecipeNode>>& ChildEntry : SourceNode->Children)
		{
			SortedChildren.Add(ChildEntry);
		}

		SortedChildren.Sort([](const TPair<FBCraftingSearchToken, TSharedPtr<FBCraftingRecipeNode>>& Left, const TPair<FBCraftingSearchToken, TSharedPtr<FBCraftingRecipeNode>>& Right)
		{
			return Left.Key.LexicalLess(Right.Key);
		});

		return SortedChildren;
	}
}

struct FBCraftingRecipeIndexVisualizerTab::FTreeNode
{
	FText Label;
	TArray<FTreeNodePtr> Children;
};

void FBCraftingRecipeIndexVisualizerTab::Register()
{
	if (!GEditor)
	{
		return;
	}

	if (UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>())
	{
		AssetOpenedHandle = AssetEditorSubsystem->OnAssetOpenedInEditor().AddRaw(this, &FBCraftingRecipeIndexVisualizerTab::HandleAssetOpened);
		AssetClosedHandle = AssetEditorSubsystem->OnAssetClosedInEditor().AddRaw(this, &FBCraftingRecipeIndexVisualizerTab::HandleAssetClosed);

		for (UObject* Asset : AssetEditorSubsystem->GetAllEditedAssets())
		{
			if (!IsSupportedRecipeTable(Asset))
			{
				continue;
			}

			for (IAssetEditorInstance* AssetEditorInstance : AssetEditorSubsystem->FindEditorsForAsset(Asset))
			{
				RegisterLocalTab(Cast<UDataTable>(Asset), AssetEditorInstance);
			}
		}
	}
}

void FBCraftingRecipeIndexVisualizerTab::Unregister()
{
	if (GEditor)
	{
		if (UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>())
		{
			if (AssetOpenedHandle.IsValid())
			{
				AssetEditorSubsystem->OnAssetOpenedInEditor().Remove(AssetOpenedHandle);
				AssetOpenedHandle.Reset();
			}

			if (AssetClosedHandle.IsValid())
			{
				AssetEditorSubsystem->OnAssetClosedInEditor().Remove(AssetClosedHandle);
				AssetClosedHandle.Reset();
			}
		}
	}

	TArray<IAssetEditorInstance*> EditorInstances;
	EditorTabStates.GetKeys(EditorInstances);
	for (IAssetEditorInstance* AssetEditorInstance : EditorInstances)
	{
		UnregisterLocalTab(AssetEditorInstance);
	}

	EditorTabStates.Empty();
}

void FBCraftingRecipeIndexVisualizerTab::ShowRecipeIndex(UDataTable* SourceTable, const FBCraftingRecipeIndex& RecipeIndex)
{
	if (!SourceTable || !GEditor)
	{
		return;
	}

	UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
	if (!AssetEditorSubsystem)
	{
		return;
	}

	IAssetEditorInstance* AssetEditorInstance = AssetEditorSubsystem->FindEditorForAsset(SourceTable, true);
	if (!AssetEditorInstance)
	{
		return;
	}

	RegisterLocalTab(SourceTable, AssetEditorInstance);

	FEditorTabState* State = FindState(AssetEditorInstance);
	if (!State)
	{
		return;
	}

	State->SourceTable = SourceTable;
	State->RecipeIndex = RecipeIndex;
	State->bHasData = true;
	RebuildTreeItems(*State);

	AssetEditorInstance->InvokeTab(FTabId(RecipeIndexVisualizerTabId));

	if (State->TreeView.IsValid())
	{
		State->TreeView->RequestTreeRefresh();
		for (const FTreeNodePtr& RootItem : State->RootItems)
		{
			State->TreeView->SetItemExpansion(RootItem, true);
		}
	}
}

void FBCraftingRecipeIndexVisualizerTab::HandleAssetOpened(UObject* Asset, IAssetEditorInstance* AssetEditorInstance)
{
	if (!IsSupportedRecipeTable(Asset) || !AssetEditorInstance)
	{
		return;
	}

	RegisterLocalTab(Cast<UDataTable>(Asset), AssetEditorInstance);
}

void FBCraftingRecipeIndexVisualizerTab::HandleAssetClosed(UObject* Asset, IAssetEditorInstance* AssetEditorInstance)
{
	if (!IsSupportedRecipeTable(Asset) || !AssetEditorInstance)
	{
		return;
	}

	UnregisterLocalTab(AssetEditorInstance);
}

void FBCraftingRecipeIndexVisualizerTab::RegisterLocalTab(UDataTable* SourceTable, IAssetEditorInstance* AssetEditorInstance)
{
	if (!SourceTable || !AssetEditorInstance || EditorTabStates.Contains(AssetEditorInstance))
	{
		return;
	}

	TSharedPtr<FTabManager> TabManager = AssetEditorInstance->GetAssociatedTabManager();
	if (!TabManager.IsValid())
	{
		return;
	}

	FEditorTabState& State = EditorTabStates.Add(AssetEditorInstance);
	State.SourceTable = SourceTable;
	State.WorkspaceCategory = TabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("RecipeIndexVisualizerCategory", "Beaver Crafting"));

	TabManager->RegisterTabSpawner(RecipeIndexVisualizerTabId, FOnSpawnTab::CreateRaw(this, &FBCraftingRecipeIndexVisualizerTab::SpawnTab, AssetEditorInstance))
		.SetDisplayName(LOCTEXT("RecipeIndexVisualizerTab", "Recipe Index Visualizer"))
		.SetTooltipText(LOCTEXT("RecipeIndexVisualizerTabTooltip", "Visualize the BeaverCrafting recipe index tree."))
		.SetGroup(State.WorkspaceCategory.ToSharedRef());
}

void FBCraftingRecipeIndexVisualizerTab::UnregisterLocalTab(IAssetEditorInstance* AssetEditorInstance)
{
	if (!AssetEditorInstance)
	{
		return;
	}

	if (TSharedPtr<FTabManager> TabManager = AssetEditorInstance->GetAssociatedTabManager())
	{
		TabManager->UnregisterTabSpawner(RecipeIndexVisualizerTabId);
	}

	EditorTabStates.Remove(AssetEditorInstance);
}

FBCraftingRecipeIndexVisualizerTab::FEditorTabState* FBCraftingRecipeIndexVisualizerTab::FindState(IAssetEditorInstance* AssetEditorInstance)
{
	return EditorTabStates.Find(AssetEditorInstance);
}

const FBCraftingRecipeIndexVisualizerTab::FEditorTabState* FBCraftingRecipeIndexVisualizerTab::FindState(IAssetEditorInstance* AssetEditorInstance) const
{
	return EditorTabStates.Find(AssetEditorInstance);
}

TSharedRef<SDockTab> FBCraftingRecipeIndexVisualizerTab::SpawnTab(const FSpawnTabArgs& Args, IAssetEditorInstance* AssetEditorInstance)
{
	check(Args.GetTabId().TabType == RecipeIndexVisualizerTabId);

	FEditorTabState* State = FindState(AssetEditorInstance);
	check(State);

	TSharedRef<SDockTab> DockTab = SNew(SDockTab)
		.Label(LOCTEXT("RecipeIndexVisualizerDockTitle", "Recipe Index Visualizer"))
		[
			BuildTabContent(AssetEditorInstance)
		];

	State->DockTab = DockTab;
	return DockTab;
}

TSharedRef<SWidget> FBCraftingRecipeIndexVisualizerTab::BuildTabContent(IAssetEditorInstance* AssetEditorInstance)
{
	FEditorTabState* State = FindState(AssetEditorInstance);
	check(State);

	return SNew(SBorder)
		.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
		.Padding(8.0f)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 0.0f, 0.0f, 8.0f)
			[
				SNew(STextBlock)
				.Text_Lambda([this, AssetEditorInstance]()
				{
					return GetHeaderText(AssetEditorInstance);
				})
				.Font(GetHeaderFont())
				.TextStyle(FAppStyle::Get(), "NormalText.Important")
			]
			+ SVerticalBox::Slot()
			.FillHeight(1.0f)
			[
				SAssignNew(State->TreeView, STreeView<FTreeNodePtr>)
				.TreeItemsSource(&State->RootItems)
				.OnGenerateRow_Lambda([this, AssetEditorInstance](FTreeNodePtr InItem, const TSharedRef<STableViewBase>& OwnerTable)
				{
					return GenerateTreeRow(AssetEditorInstance, InItem, OwnerTable);
				})
				.OnGetChildren_Lambda([this, AssetEditorInstance](FTreeNodePtr InItem, TArray<FTreeNodePtr>& OutChildren)
				{
					GetChildrenForTree(AssetEditorInstance, InItem, OutChildren);
				})
				.SelectionMode(ESelectionMode::Single)
			]
		];
}

FBCraftingRecipeIndexVisualizerTab::FTreeNodePtr FBCraftingRecipeIndexVisualizerTab::BuildTreeNode(const FEditorTabState& State, const TSharedPtr<const FBCraftingRecipeNode>& SourceNode, const FText& Label) const
{
	FTreeNodePtr NewNode = MakeShared<FTreeNode>();
	NewNode->Label = Label;

	for (const TPair<FBCraftingSearchToken, TSharedPtr<FBCraftingRecipeNode>>& ChildEntry : GetSortedChildren(SourceNode))
	{
		NewNode->Children.Add(BuildTreeNode(State, ChildEntry.Value, MakeTokenText(ChildEntry.Key)));
	}

	for (const FBCraftingRecipeMatch& Match : SourceNode->Matches)
	{
		FTreeNodePtr MatchNode = MakeShared<FTreeNode>();
		MatchNode->Label = FText::FromString(FString::Printf(TEXT("Match | Row=%s | ResultItemId=%s | Priority=%d"),
			*Match.RecipeRowName.ToString(),
			*Match.ResultItemId.ToString(),
			Match.Priority));
		NewNode->Children.Add(MatchNode);
	}

	return NewNode;
}

void FBCraftingRecipeIndexVisualizerTab::RebuildTreeItems(FEditorTabState& State) const
{
	State.RootItems.Reset();

	const TSharedPtr<const FBCraftingRecipeNode> RootNode = State.RecipeIndex.GetRootNode();
	if (!RootNode.IsValid())
	{
		return;
	}

	FTreeNodePtr RootItem = MakeShared<FTreeNode>();
	RootItem->Label = FText::FromString(TEXT("Root"));

	for (const TPair<FBCraftingSearchToken, TSharedPtr<FBCraftingRecipeNode>>& ChildEntry : GetSortedChildren(RootNode))
	{
		RootItem->Children.Add(BuildTreeNode(State, ChildEntry.Value, MakeTokenText(ChildEntry.Key)));
	}

	for (const FBCraftingRecipeMatch& Match : RootNode->Matches)
	{
		FTreeNodePtr MatchNode = MakeShared<FTreeNode>();
		MatchNode->Label = FText::FromString(FString::Printf(TEXT("Match | Row=%s | ResultItemId=%s | Priority=%d"),
			*Match.RecipeRowName.ToString(),
			*Match.ResultItemId.ToString(),
			Match.Priority));
		RootItem->Children.Add(MatchNode);
	}

	State.RootItems.Add(RootItem);
}

void FBCraftingRecipeIndexVisualizerTab::GetChildrenForTree(IAssetEditorInstance* AssetEditorInstance, FTreeNodePtr InItem, TArray<FTreeNodePtr>& OutChildren) const
{
	if (!FindState(AssetEditorInstance) || !InItem.IsValid())
	{
		return;
	}

	OutChildren.Append(InItem->Children);
}

TSharedRef<ITableRow> FBCraftingRecipeIndexVisualizerTab::GenerateTreeRow(IAssetEditorInstance* AssetEditorInstance, FTreeNodePtr InItem, const TSharedRef<STableViewBase>& OwnerTable) const
{
	check(FindState(AssetEditorInstance));

	return SNew(STableRow<FTreeNodePtr>, OwnerTable)
	[
		SNew(STextBlock)
		.Font(GetTreeRowFont())
		.Text(InItem.IsValid() ? InItem->Label : FText::GetEmpty())
	];
}

FText FBCraftingRecipeIndexVisualizerTab::GetHeaderText(IAssetEditorInstance* AssetEditorInstance) const
{
	const FEditorTabState* State = FindState(AssetEditorInstance);
	if (!State || !State->bHasData)
	{
		return LOCTEXT("RecipeIndexVisualizerEmpty", "No recipe index has been generated yet.");
	}

	return FText::Format(
		LOCTEXT("RecipeIndexVisualizerHeader", "Source Table: {0} | Indexed Recipes: {1}"),
		FText::FromName(State->SourceTable.IsValid() ? State->SourceTable->GetFName() : NAME_None),
		FText::AsNumber(State->RecipeIndex.GetRecipeCount()));
}

bool FBCraftingRecipeIndexVisualizerTab::IsSupportedRecipeTable(const UObject* Asset) const
{
	const UDataTable* DataTable = Cast<UDataTable>(Asset);
	return DataTable && DataTable->GetRowStruct() == FBCraftingRecipeRow::StaticStruct();
}

#undef LOCTEXT_NAMESPACE
