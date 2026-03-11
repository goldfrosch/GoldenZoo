#pragma once

#include "CoreMinimal.h"
#include "Data/BCraftingRecipeTypes.h"

class UDataTable;

struct FBCraftingRecipeNode
{
	TMap<FBCraftingSearchToken, TSharedPtr<FBCraftingRecipeNode>> Children;
	TArray<FBCraftingRecipeMatch> Matches;
};

class BEAVERCRAFTING_API FBCraftingRecipeIndex
{
public:
	FBCraftingRecipeIndex();

	void Reset();
	void AddRecipe(FName RecipeRowName, const TArray<FBCraftingSearchToken>& SearchTokens, const FBCraftingRecipeMatch& Match);
	bool FindExact(const FBCraftingRecipeQuery& Query, FBCraftingRecipeMatch& OutMatch) const;
	bool FindExactTokens(const TArray<FBCraftingSearchToken>& SearchTokens, FBCraftingRecipeMatch& OutMatch) const;
	int32 GetRecipeCount() const { return RecipeCount; }
	TSharedPtr<const FBCraftingRecipeNode> GetRootNode() const { return RootNode; }

private:
	static bool IsHigherPriority(const FBCraftingRecipeMatch& Left, const FBCraftingRecipeMatch& Right);

	TSharedPtr<FBCraftingRecipeNode> RootNode;
	int32 RecipeCount = 0;
};

class BEAVERCRAFTING_API FBCraftingRecipeIndexBuilder
{
public:
	static bool BuildFromDataTable(UDataTable* RecipeTable, FBCraftingRecipeIndex& OutIndex);
};
