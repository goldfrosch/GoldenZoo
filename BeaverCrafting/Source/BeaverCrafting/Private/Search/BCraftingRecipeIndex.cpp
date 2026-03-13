#include "Search/BCraftingRecipeIndex.h"

#include "Engine/DataTable.h"
#include "Settings/SquirreltemSettings.h"

namespace
{
	void ValidateItemReference(const USquirreltemSettings* ItemSettings, const FName ItemId, const FName RecipeRowName, const TCHAR* FieldName)
	{
		if (ItemId.IsNone())
		{
			UE_LOG(LogTemp, Warning,
				TEXT("[BeaverCrafting] Recipe '%s' has an empty %s."),
				*RecipeRowName.ToString(),
				FieldName);
			return;
		}

		if (ItemSettings && ItemSettings->HasItemInfo(ItemId))
		{
			return;
		}

		UE_LOG(LogTemp, Warning,
			TEXT("[BeaverCrafting] Recipe '%s' references unknown ItemId '%s' in %s."),
			*RecipeRowName.ToString(),
			*ItemId.ToString(),
			FieldName);
	}
}

FBCraftingRecipeIndex::FBCraftingRecipeIndex()
{
	Reset();
}

void FBCraftingRecipeIndex::Reset()
{
	RootNode = MakeShared<FBCraftingRecipeNode>();
	RecipeCount = 0;
}

void FBCraftingRecipeIndex::AddRecipe(const FName RecipeRowName, const TArray<FBCraftingSearchToken>& SearchTokens, const FBCraftingRecipeMatch& Match)
{
	if (!RootNode.IsValid())
	{
		Reset();
	}

	TSharedPtr<FBCraftingRecipeNode> CurrentNode = RootNode;
	for (const FBCraftingSearchToken& Token : SearchTokens)
	{
		TSharedPtr<FBCraftingRecipeNode>& ChildNode = CurrentNode->Children.FindOrAdd(Token);
		if (!ChildNode.IsValid())
		{
			ChildNode = MakeShared<FBCraftingRecipeNode>();
		}

		CurrentNode = ChildNode;
	}

	FBCraftingRecipeMatch StoredMatch = Match;
	StoredMatch.bFound = true;
	StoredMatch.RecipeRowName = RecipeRowName;
	CurrentNode->Matches.Add(StoredMatch);
	++RecipeCount;
}

bool FBCraftingRecipeIndex::FindExact(const FBCraftingRecipeQuery& Query, FBCraftingRecipeMatch& OutMatch) const
{
	return FindExactTokens(Query.BuildSearchTokens(), OutMatch);
}

bool FBCraftingRecipeIndex::FindExactTokens(const TArray<FBCraftingSearchToken>& SearchTokens, FBCraftingRecipeMatch& OutMatch) const
{
	OutMatch = FBCraftingRecipeMatch();

	if (!RootNode.IsValid())
	{
		return false;
	}

	TSharedPtr<FBCraftingRecipeNode> CurrentNode = RootNode;
	for (const FBCraftingSearchToken& Token : SearchTokens)
	{
		const TSharedPtr<FBCraftingRecipeNode>* ChildNode = CurrentNode->Children.Find(Token);
		if (!ChildNode || !ChildNode->IsValid())
		{
			return false;
		}

		CurrentNode = *ChildNode;
	}

	if (CurrentNode->Matches.IsEmpty())
	{
		return false;
	}

	const FBCraftingRecipeMatch* BestMatch = &CurrentNode->Matches[0];
	for (const FBCraftingRecipeMatch& Candidate : CurrentNode->Matches)
	{
		if (IsHigherPriority(Candidate, *BestMatch))
		{
			BestMatch = &Candidate;
		}
	}

	OutMatch = *BestMatch;
	return OutMatch.bFound;
}

bool FBCraftingRecipeIndex::IsHigherPriority(const FBCraftingRecipeMatch& Left, const FBCraftingRecipeMatch& Right)
{
	if (Left.Priority != Right.Priority)
	{
		return Left.Priority > Right.Priority;
	}

	return Left.RecipeRowName.LexicalLess(Right.RecipeRowName);
}

bool FBCraftingRecipeIndexBuilder::BuildFromDataTable(UDataTable* RecipeTable, FBCraftingRecipeIndex& OutIndex)
{
	OutIndex.Reset();

	if (!RecipeTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("[BeaverCrafting] RecipeTable is not configured."));
		return false;
	}

	const USquirreltemSettings* ItemSettings = GetDefault<USquirreltemSettings>();

	for (const TPair<FName, uint8*>& RowEntry : RecipeTable->GetRowMap())
	{
		const FName RecipeRowName = RowEntry.Key;
		const FBCraftingRecipeRow* RecipeRow = reinterpret_cast<const FBCraftingRecipeRow*>(RowEntry.Value);
		if (!RecipeRow)
		{
			continue;
		}

		ValidateItemReference(ItemSettings, RecipeRow->ResultItemId, RecipeRowName, TEXT("ResultItemId"));
		for (const FBCraftingIngredientClause& IngredientClause : RecipeRow->IngredientClauses)
		{
			ValidateItemReference(ItemSettings, IngredientClause.ItemId, RecipeRowName, TEXT("IngredientClauses.ItemId"));
		}

		FBCraftingRecipeMatch Match;
		Match.bFound = true;
		Match.RecipeRowName = RecipeRowName;
		Match.ResultItemId = RecipeRow->ResultItemId;
		Match.Priority = RecipeRow->Priority;

		OutIndex.AddRecipe(RecipeRowName, RecipeRow->BuildSearchTokens(), Match);
	}

	return OutIndex.GetRecipeCount() > 0;
}
