#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"

#include "BCraftingRecipeTypes.generated.h"

namespace BCraftingRecipeTokens
{
	extern BEAVERCRAFTING_API const FName ProductType;
	extern BEAVERCRAFTING_API const FName MaterialId;
	extern BEAVERCRAFTING_API const FName MaterialCount;
}

USTRUCT(BlueprintType)
struct BEAVERCRAFTING_API FBCraftingSearchToken
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BeaverCrafting")
	FName TokenType = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BeaverCrafting")
	FName NameValue = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BeaverCrafting")
	int32 IntValue = 0;

	bool operator==(const FBCraftingSearchToken& Other) const
	{
		return TokenType == Other.TokenType && NameValue == Other.NameValue && IntValue == Other.IntValue;
	}

	bool LexicalLess(const FBCraftingSearchToken& Other) const;
};

FORCEINLINE uint32 GetTypeHash(const FBCraftingSearchToken& Token)
{
	return HashCombineFast(HashCombineFast(GetTypeHash(Token.TokenType), GetTypeHash(Token.NameValue)), GetTypeHash(Token.IntValue));
}

USTRUCT(BlueprintType)
struct BEAVERCRAFTING_API FBCraftingIngredientClause
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BeaverCrafting")
	FName Key = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BeaverCrafting", meta = (ClampMin = "1"))
	int32 Count = 1;

	void AppendSearchTokens(TArray<FBCraftingSearchToken>& OutTokens) const;
};

USTRUCT(BlueprintType)
struct BEAVERCRAFTING_API FBCraftingSelectionIngredient
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BeaverCrafting")
	FName MaterialId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BeaverCrafting", meta = (ClampMin = "1"))
	int32 Count = 1;
};

USTRUCT(BlueprintType)
struct BEAVERCRAFTING_API FBCraftingRecipeQuery
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BeaverCrafting")
	FName ProductType = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BeaverCrafting")
	TArray<FBCraftingSelectionIngredient> Ingredients;

	TArray<FBCraftingSearchToken> BuildSearchTokens() const;
};

USTRUCT(BlueprintType)
struct BEAVERCRAFTING_API FBCraftingRecipeRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BeaverCrafting")
	FName ResultId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BeaverCrafting")
	FName ProductType = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BeaverCrafting")
	TArray<FBCraftingIngredientClause> IngredientClauses;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BeaverCrafting")
	int32 Priority = 0;

	TArray<FBCraftingSearchToken> BuildSearchTokens() const;
};

USTRUCT(BlueprintType)
struct BEAVERCRAFTING_API FBCraftingRecipeMatch
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BeaverCrafting")
	bool bFound = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BeaverCrafting")
	FName RecipeRowName = NAME_None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BeaverCrafting")
	FName ResultId = NAME_None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BeaverCrafting")
	int32 Priority = 0;
};
