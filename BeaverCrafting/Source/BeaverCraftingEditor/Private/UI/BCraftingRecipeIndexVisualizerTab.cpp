#include "UI/BCraftingRecipeIndexVisualizerTab.h"

#include "Engine/DataTable.h"
#include "Framework/Docking/TabManager.h"
#include "Internationalization/Text.h"
#include "Styling/AppStyle.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Views/STableRow.h"
#include "Widgets/Views/STreeView.h"

namespace
{
	const FName RecipeIndexVisualizerTabId(TEXT("BeaverCrafting.RecipeIndexVisualizer"));
	constexpr int32 FontSizeDelta = 4;

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
			return FText::FromString(FString::Printf(TEXT("%s [ %d ]"), *Token.TokenType.ToString(), Token.IntValue));
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
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
		RecipeIndexVisualizerTabId,
		FOnSpawnTab::CreateRaw(this, &FBCraftingRecipeIndexVisualizerTab::SpawnTab))
		.SetDisplayName(NSLOCTEXT("BeaverCraftingEditor", "RecipeIndexVisualizerTab", "Recipe Index Visualizer"))
		.SetTooltipText(NSLOCTEXT("BeaverCraftingEditor", "RecipeIndexVisualizerTabTooltip", "Visualize the BeaverCrafting recipe index tree."));
}

void FBCraftingRecipeIndexVisualizerTab::Unregister()
{
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(RecipeIndexVisualizerTabId);
}

void FBCraftingRecipeIndexVisualizerTab::ShowRecipeIndex(UDataTable* SourceTable, const FBCraftingRecipeIndex& RecipeIndex)
{
	CurrentSourceTable = SourceTable;
	CurrentRecipeIndex = RecipeIndex;
	bHasData = true;
	RebuildTreeItems();
	FGlobalTabmanager::Get()->TryInvokeTab(RecipeIndexVisualizerTabId);

	if (TreeView.IsValid())
	{
		TreeView->RequestTreeRefresh();
		for (const FTreeNodePtr& RootItem : RootItems)
		{
			TreeView->SetItemExpansion(RootItem, true);
		}
	}
}

TSharedRef<SDockTab> FBCraftingRecipeIndexVisualizerTab::SpawnTab(const FSpawnTabArgs& Args)
{
	return SNew(SDockTab)
		.TabRole(NomadTab)
		[
			BuildTabContent()
		];
}

TSharedRef<SWidget> FBCraftingRecipeIndexVisualizerTab::BuildTabContent()
{
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
				.Text_Raw(this, &FBCraftingRecipeIndexVisualizerTab::GetHeaderText)
				.Font(GetHeaderFont())
				.TextStyle(FAppStyle::Get(), "NormalText.Important")
			]
			+ SVerticalBox::Slot()
			.FillHeight(1.0f)
			[
				SAssignNew(TreeView, STreeView<FTreeNodePtr>)
				.TreeItemsSource(&RootItems)
				.OnGenerateRow_Raw(this, &FBCraftingRecipeIndexVisualizerTab::GenerateTreeRow)
				.OnGetChildren_Raw(this, &FBCraftingRecipeIndexVisualizerTab::GetChildrenForTree)
				.SelectionMode(ESelectionMode::Single)
			]
		];
}

FBCraftingRecipeIndexVisualizerTab::FTreeNodePtr FBCraftingRecipeIndexVisualizerTab::BuildTreeNode(const TSharedPtr<const FBCraftingRecipeNode>& SourceNode, const FText& Label) const
{
	FTreeNodePtr NewNode = MakeShared<FTreeNode>();
	NewNode->Label = Label;

	for (const TPair<FBCraftingSearchToken, TSharedPtr<FBCraftingRecipeNode>>& ChildEntry : GetSortedChildren(SourceNode))
	{
		NewNode->Children.Add(BuildTreeNode(ChildEntry.Value, MakeTokenText(ChildEntry.Key)));
	}

	for (const FBCraftingRecipeMatch& Match : SourceNode->Matches)
	{
		FTreeNodePtr MatchNode = MakeShared<FTreeNode>();
		MatchNode->Label = FText::FromString(FString::Printf(TEXT("Match | Row=%s | ResultItemId=%d | Priority=%d"),
			*Match.RecipeRowName.ToString(),
			Match.ResultItemId,
			Match.Priority));
		NewNode->Children.Add(MatchNode);
	}

	return NewNode;
}

void FBCraftingRecipeIndexVisualizerTab::RebuildTreeItems()
{
	RootItems.Reset();

	const TSharedPtr<const FBCraftingRecipeNode> RootNode = CurrentRecipeIndex.GetRootNode();
	if (!RootNode.IsValid())
	{
		return;
	}

	FTreeNodePtr RootItem = MakeShared<FTreeNode>();
	RootItem->Label = FText::FromString(TEXT("Root"));

	for (const TPair<FBCraftingSearchToken, TSharedPtr<FBCraftingRecipeNode>>& ChildEntry : GetSortedChildren(RootNode))
	{
		RootItem->Children.Add(BuildTreeNode(ChildEntry.Value, MakeTokenText(ChildEntry.Key)));
	}

	for (const FBCraftingRecipeMatch& Match : RootNode->Matches)
	{
		FTreeNodePtr MatchNode = MakeShared<FTreeNode>();
		MatchNode->Label = FText::FromString(FString::Printf(TEXT("Match | Row=%s | ResultItemId=%d | Priority=%d"),
			*Match.RecipeRowName.ToString(),
			Match.ResultItemId,
			Match.Priority));
		RootItem->Children.Add(MatchNode);
	}

	RootItems.Add(RootItem);
}

void FBCraftingRecipeIndexVisualizerTab::GetChildrenForTree(FTreeNodePtr InItem, TArray<FTreeNodePtr>& OutChildren) const
{
	if (!InItem.IsValid())
	{
		return;
	}

	OutChildren.Append(InItem->Children);
}

TSharedRef<ITableRow> FBCraftingRecipeIndexVisualizerTab::GenerateTreeRow(FTreeNodePtr InItem, const TSharedRef<STableViewBase>& OwnerTable) const
{
	return SNew(STableRow<FTreeNodePtr>, OwnerTable)
	[
		SNew(STextBlock)
		.Font(GetTreeRowFont())
		.Text(InItem.IsValid() ? InItem->Label : FText::GetEmpty())
	];
}

FText FBCraftingRecipeIndexVisualizerTab::GetHeaderText() const
{
	if (!bHasData)
	{
		return NSLOCTEXT("BeaverCraftingEditor", "RecipeIndexVisualizerEmpty", "No recipe index has been generated yet.");
	}

	return FText::Format(
		NSLOCTEXT("BeaverCraftingEditor", "RecipeIndexVisualizerHeader", "Source Table: {0} | Indexed Recipes: {1}"),
		FText::FromName(CurrentSourceTable.IsValid() ? CurrentSourceTable->GetFName() : NAME_None),
		FText::AsNumber(CurrentRecipeIndex.GetRecipeCount()));
}
