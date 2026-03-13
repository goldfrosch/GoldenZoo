#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"

#include "SquirreltemData.generated.h"

UENUM()
enum class EItemType : uint32
{
	Undefined, Interactive, Equipment, Consume, Ingredient, Build, Max,
};

USTRUCT(BlueprintType)
struct SQUIRRELTEM_API FItemInfoData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	const FString& GetDisplayName() const
	{
		return DisplayName;
	}

	const FText& GetDescription() const
	{
		return Description;
	}

	EItemType GetItemType() const
	{
		return ItemType;
	}

	uint32 GetMaxItemCount() const
	{
		return MaxItemCount;
	}

	const TSoftObjectPtr<UTexture2D>& GetThumbnail() const
	{
		return Thumbnail;
	}

	const FGameplayTag& GetItemGrade() const
	{
		return ItemGrade;
	}

	void SetItemGrade(const FGameplayTag& NewItemGrade)
	{
		ItemGrade = NewItemGrade;
	}

	const TSubclassOf<AActor>& GetOnHandItemClass() const
	{
		return OnHandItemClass;
	}

	const TMap<FGameplayTag, FString>& GetMetaData() const
	{
		return MetaData;
	}

	const TMap<FGameplayTag, FString>& GetConstData() const
	{
		return ConstData;
	}

private:
	UPROPERTY(EditDefaultsOnly, Category = "Data", meta = (AllowPrivateAccess = true))
	FString DisplayName;

	UPROPERTY(EditDefaultsOnly, Category = "Data", meta = (AllowPrivateAccess = true, MultiLine = true))
	FText Description;

	UPROPERTY(EditDefaultsOnly, Category = "Data", meta = (AllowPrivateAccess = true))
	EItemType ItemType = EItemType::Undefined;

	UPROPERTY(EditDefaultsOnly, Category = "Data", meta = (AllowPrivateAccess = true))
	TSoftObjectPtr<UTexture2D> Thumbnail;

	UPROPERTY()
	FGameplayTag ItemGrade;

	UPROPERTY(EditDefaultsOnly, Category = "Data", meta = (AllowPrivateAccess = true))
	TSubclassOf<AActor> OnHandItemClass;

	UPROPERTY(EditDefaultsOnly, Category = "Data", meta = (AllowPrivateAccess = true, ClampMin = "0", ClampMax = "1000", UIMin = "0", UIMax = "1000"))
	uint32 MaxItemCount = 0;

	UPROPERTY()
	TMap<FGameplayTag, FString> MetaData;

	UPROPERTY(EditDefaultsOnly, Category = "Data", meta = (AllowPrivateAccess = true))
	TMap<FGameplayTag, FString> ConstData;
};

USTRUCT(BlueprintType)
struct SQUIRRELTEM_API FItemMetaInfo
{
	GENERATED_BODY()

	FName GetId() const
	{
		return Id;
	}

	void SetId(const FName& NewId)
	{
		Id = NewId;
	}

	uint32 GetCurrentCount() const
	{
		return CurrentCount;
	}

	const TMap<FGameplayTag, FString>& GetMetaData() const
	{
		return MetaData;
	}

	void SetMetaDataValue(const FGameplayTag& Key, const FString& Value)
	{
		MetaData.Add(Key, Value);
	}

	void SetMetaData(const TMap<FGameplayTag, FString>& NewMetaData)
	{
		MetaData = NewMetaData;
	}

	void SetCurrentCount(const int32 NewValue)
	{
		CurrentCount = static_cast<uint32>(FMath::Max(NewValue, 0));
	}

	bool operator==(const FItemMetaInfo& CompareItem) const
	{
		if (CompareItem.GetId() != GetId())
		{
			return false;
		}

		if (MetaData.Num() != CompareItem.MetaData.Num())
		{
			return false;
		}

		for (const TTuple<FGameplayTag, FString>& Data : MetaData)
		{
			const FString* CompareValue = CompareItem.MetaData.Find(Data.Key);
			if (!CompareValue)
			{
				return false;
			}

			if (Data.Value != *CompareValue)
			{
				return false;
			}
		}

		return true;
	}

	bool IsValid() const
	{
		return !Id.IsNone();
	}

private:
	FName Id;

	UPROPERTY(EditDefaultsOnly, Category = "Data", meta = (ClampMin = "0", ClampMax = "1000", UIMin = "0", UIMax = "1000"))
	uint32 CurrentCount = 0;

	UPROPERTY(EditDefaultsOnly, Category = "Data")
	TMap<FGameplayTag, FString> MetaData;
};

FORCEINLINE uint32 GetTypeHash(const FItemMetaInfo& ItemMetaInfo)
{
	uint32 DefaultHash = 0;
	DefaultHash = HashCombine(DefaultHash, GetTypeHash(ItemMetaInfo.GetId()));

	uint32 MetaDataHash = 0;
	for (const TTuple<FGameplayTag, FString>& MetaData : ItemMetaInfo.GetMetaData())
	{
		const uint32 PairHash = HashCombine(GetTypeHash(MetaData.Key), GetTypeHash(MetaData.Value));
		MetaDataHash ^= PairHash;
	}

	return HashCombine(DefaultHash, MetaDataHash);
}
