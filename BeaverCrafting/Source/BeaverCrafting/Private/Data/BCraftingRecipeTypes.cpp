#include "Data/BCraftingRecipeTypes.h"

namespace BCraftingRecipeTokens
{
	const FName ProductType(TEXT("ProductType"));
	const FName ItemId(TEXT("ItemId"));
	const FName ItemCount(TEXT("ItemCount"));
}

namespace
{
	FBCraftingSearchToken MakeToken(const FName TokenType, const FName NameValue, const int32 IntValue)
	{
		FBCraftingSearchToken Token;
		Token.TokenType = TokenType;
		Token.NameValue = NameValue;
		Token.IntValue = IntValue;
		return Token;
	}

	int32 GetNormalizedCount(const int32 Count)
	{
		return FMath::Max(Count, 1);
	}

	struct FOrderedItemEntry
	{
		int32 ItemId = 0;
		int32 Count = 1;
	};

	void SortOrderedItemEntries(TArray<FOrderedItemEntry>& Entries)
	{
		Entries.Sort([](const FOrderedItemEntry& Left, const FOrderedItemEntry& Right)
		{
			return Left.ItemId < Right.ItemId;
		});
	}

	TArray<FOrderedItemEntry> BuildOrderedItemEntries(const TArray<FBCraftingIngredientClause>& Clauses)
	{
		TMap<int32, int32> CountByItemId;
		for (const FBCraftingIngredientClause& Clause : Clauses)
		{
			if (Clause.ItemId == 0)
			{
				continue;
			}

			CountByItemId.FindOrAdd(Clause.ItemId) += GetNormalizedCount(Clause.Count);
		}

		TArray<FOrderedItemEntry> Entries;
		Entries.Reserve(CountByItemId.Num());
		for (const TPair<int32, int32>& Entry : CountByItemId)
		{
			FOrderedItemEntry& OrderedEntry = Entries.AddDefaulted_GetRef();
			OrderedEntry.ItemId = Entry.Key;
			OrderedEntry.Count = Entry.Value;
		}

		SortOrderedItemEntries(Entries);
		return Entries;
	}

	TArray<FOrderedItemEntry> BuildOrderedItemEntries(const TArray<FBCraftingSelectionIngredient>& Ingredients)
	{
		TMap<int32, int32> CountByItemId;
		for (const FBCraftingSelectionIngredient& Ingredient : Ingredients)
		{
			if (Ingredient.ItemId == 0)
			{
				continue;
			}

			CountByItemId.FindOrAdd(Ingredient.ItemId) += GetNormalizedCount(Ingredient.Count);
		}

		TArray<FOrderedItemEntry> Entries;
		Entries.Reserve(CountByItemId.Num());
		for (const TPair<int32, int32>& Entry : CountByItemId)
		{
			FOrderedItemEntry& OrderedEntry = Entries.AddDefaulted_GetRef();
			OrderedEntry.ItemId = Entry.Key;
			OrderedEntry.Count = Entry.Value;
		}

		SortOrderedItemEntries(Entries);
		return Entries;
	}
}

bool FBCraftingSearchToken::LexicalLess(const FBCraftingSearchToken& Other) const
{
	const int32 TokenTypeCompare = TokenType.Compare(Other.TokenType);
	if (TokenTypeCompare != 0)
	{
		return TokenTypeCompare < 0;
	}

	const int32 NameCompare = NameValue.Compare(Other.NameValue);
	if (NameCompare != 0)
	{
		return NameCompare < 0;
	}

	return IntValue < Other.IntValue;
}

void FBCraftingIngredientClause::AppendSearchTokens(TArray<FBCraftingSearchToken>& OutTokens) const
{
	OutTokens.Add(MakeToken(BCraftingRecipeTokens::ItemId, NAME_None, ItemId));
	OutTokens.Add(MakeToken(BCraftingRecipeTokens::ItemCount, NAME_None, GetNormalizedCount(Count)));
}

TArray<FBCraftingSearchToken> FBCraftingRecipeQuery::BuildSearchTokens() const
{
	const TArray<FOrderedItemEntry> OrderedIngredients = BuildOrderedItemEntries(Ingredients);

	TArray<FBCraftingSearchToken> Tokens;
	Tokens.Reserve(1 + OrderedIngredients.Num() * 2);
	Tokens.Add(MakeToken(BCraftingRecipeTokens::ProductType, ProductType, 0));

	for (const FOrderedItemEntry& Ingredient : OrderedIngredients)
	{
		Tokens.Add(MakeToken(BCraftingRecipeTokens::ItemId, NAME_None, Ingredient.ItemId));
		Tokens.Add(MakeToken(BCraftingRecipeTokens::ItemCount, NAME_None, Ingredient.Count));
	}

	return Tokens;
}

TArray<FBCraftingSearchToken> FBCraftingRecipeRow::BuildSearchTokens() const
{
	const TArray<FOrderedItemEntry> OrderedClauses = BuildOrderedItemEntries(IngredientClauses);

	TArray<FBCraftingSearchToken> Tokens;
	Tokens.Reserve(1 + OrderedClauses.Num() * 2);
	Tokens.Add(MakeToken(BCraftingRecipeTokens::ProductType, ProductType, 0));

	for (const FOrderedItemEntry& Clause : OrderedClauses)
	{
		Tokens.Add(MakeToken(BCraftingRecipeTokens::ItemId, NAME_None, Clause.ItemId));
		Tokens.Add(MakeToken(BCraftingRecipeTokens::ItemCount, NAME_None, Clause.Count));
	}

	return Tokens;
}
