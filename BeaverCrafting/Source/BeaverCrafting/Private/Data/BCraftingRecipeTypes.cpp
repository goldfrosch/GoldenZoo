#include "Data/BCraftingRecipeTypes.h"

namespace BCraftingRecipeTokens
{
	const FName ProductType(TEXT("ProductType"));
	const FName MaterialId(TEXT("MaterialId"));
	const FName MaterialCount(TEXT("MaterialCount"));
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

	struct FOrderedMaterialEntry
	{
		FName MaterialId = NAME_None;
		int32 Count = 1;
	};

	void SortOrderedMaterialEntries(TArray<FOrderedMaterialEntry>& Entries)
	{
		Entries.Sort([](const FOrderedMaterialEntry& Left, const FOrderedMaterialEntry& Right)
		{
			return Left.MaterialId.LexicalLess(Right.MaterialId);
		});
	}

	TArray<FOrderedMaterialEntry> BuildOrderedMaterialEntries(const TArray<FBCraftingIngredientClause>& Clauses)
	{
		TMap<FName, int32> CountByMaterialId;
		for (const FBCraftingIngredientClause& Clause : Clauses)
		{
			CountByMaterialId.FindOrAdd(Clause.Key) += GetNormalizedCount(Clause.Count);
		}

		TArray<FOrderedMaterialEntry> Entries;
		Entries.Reserve(CountByMaterialId.Num());
		for (const TPair<FName, int32>& Entry : CountByMaterialId)
		{
			FOrderedMaterialEntry& OrderedEntry = Entries.AddDefaulted_GetRef();
			OrderedEntry.MaterialId = Entry.Key;
			OrderedEntry.Count = Entry.Value;
		}

		SortOrderedMaterialEntries(Entries);
		return Entries;
	}

	TArray<FOrderedMaterialEntry> BuildOrderedMaterialEntries(const TArray<FBCraftingSelectionIngredient>& Ingredients)
	{
		TMap<FName, int32> CountByMaterialId;
		for (const FBCraftingSelectionIngredient& Ingredient : Ingredients)
		{
			CountByMaterialId.FindOrAdd(Ingredient.MaterialId) += GetNormalizedCount(Ingredient.Count);
		}

		TArray<FOrderedMaterialEntry> Entries;
		Entries.Reserve(CountByMaterialId.Num());
		for (const TPair<FName, int32>& Entry : CountByMaterialId)
		{
			FOrderedMaterialEntry& OrderedEntry = Entries.AddDefaulted_GetRef();
			OrderedEntry.MaterialId = Entry.Key;
			OrderedEntry.Count = Entry.Value;
		}

		SortOrderedMaterialEntries(Entries);
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
	OutTokens.Add(MakeToken(BCraftingRecipeTokens::MaterialId, Key, 0));
	OutTokens.Add(MakeToken(BCraftingRecipeTokens::MaterialCount, NAME_None, GetNormalizedCount(Count)));
}

TArray<FBCraftingSearchToken> FBCraftingRecipeQuery::BuildSearchTokens() const
{
	const TArray<FOrderedMaterialEntry> OrderedIngredients = BuildOrderedMaterialEntries(Ingredients);

	TArray<FBCraftingSearchToken> Tokens;
	Tokens.Reserve(1 + OrderedIngredients.Num() * 2);
	Tokens.Add(MakeToken(BCraftingRecipeTokens::ProductType, ProductType, 0));

	for (const FOrderedMaterialEntry& Ingredient : OrderedIngredients)
	{
		Tokens.Add(MakeToken(BCraftingRecipeTokens::MaterialId, Ingredient.MaterialId, 0));
		Tokens.Add(MakeToken(BCraftingRecipeTokens::MaterialCount, NAME_None, Ingredient.Count));
	}

	return Tokens;
}

TArray<FBCraftingSearchToken> FBCraftingRecipeRow::BuildSearchTokens() const
{
	const TArray<FOrderedMaterialEntry> OrderedClauses = BuildOrderedMaterialEntries(IngredientClauses);

	TArray<FBCraftingSearchToken> Tokens;
	Tokens.Reserve(1 + OrderedClauses.Num() * 2);
	Tokens.Add(MakeToken(BCraftingRecipeTokens::ProductType, ProductType, 0));

	for (const FOrderedMaterialEntry& Clause : OrderedClauses)
	{
		Tokens.Add(MakeToken(BCraftingRecipeTokens::MaterialId, Clause.MaterialId, 0));
		Tokens.Add(MakeToken(BCraftingRecipeTokens::MaterialCount, NAME_None, Clause.Count));
	}

	return Tokens;
}
