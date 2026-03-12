#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"

#include "SquirreltemData.generated.h"

UENUM()
enum class EItemType : uint32
{
	Undefined, Interactive, Equipment, Consume, Ingredient, Build, Max,
};

UENUM(BlueprintType)
enum class EItemGrade : uint8
{
	None, Common, Uncommon, Rare, Unique,
};

UENUM()
enum class EMetaDataKey : uint32
{
	None, Durability
};

UENUM()
enum class EConstDataKey : uint32
{
	None, MaxDurability, ItemUseType, SocketName, GeneratedItemId,
	ChanceBasedSpawnItemId,
	ItemGrade,
};

USTRUCT(BlueprintType)
struct SQUIRRELTEM_API FItemAmount
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data", meta = (ClampMin = "0", UIMin = "0"))
	int32 ItemId = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data", meta = (ClampMin = "1", UIMin = "1"))
	int32 Count = 1;

	bool IsValid() const
	{
		return ItemId > 0 && Count > 0;
	}
};

USTRUCT(BlueprintType)
struct SQUIRRELTEM_API FItemInfoData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	uint16 GetItemId() const
	{
		return ItemId;
	}

	void SetItemId(const uint16 NewItemId)
	{
		ItemId = NewItemId;
	}

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

	EItemGrade GetItemGrade() const
	{
		return ItemGrade;
	}

	void SetItemGrade(const EItemGrade NewItemGrade)
	{
		ItemGrade = NewItemGrade;
	}

	const TSubclassOf<AActor>& GetOnHandItemClass() const
	{
		return OnHandItemClass;
	}

	const TMap<EMetaDataKey, FString>& GetMetaData() const
	{
		return MetaData;
	}

	const TMap<EConstDataKey, FString>& GetConstData() const
	{
		return ConstData;
	}

private:
	UPROPERTY(EditDefaultsOnly, Category = "Data", meta = (AllowPrivateAccess = true, ClampMin = "0", UIMin = "0"))
	uint16 ItemId = 0;

	UPROPERTY(EditDefaultsOnly, Category = "Data", meta = (AllowPrivateAccess = true))
	FString DisplayName;

	UPROPERTY(EditDefaultsOnly, Category = "Data", meta = (AllowPrivateAccess = true, MultiLine = true))
	FText Description;

	UPROPERTY(EditDefaultsOnly, Category = "Data", meta = (AllowPrivateAccess = true))
	EItemType ItemType = EItemType::Undefined;

	UPROPERTY(EditDefaultsOnly, Category = "Data", meta = (AllowPrivateAccess = true))
	TSoftObjectPtr<UTexture2D> Thumbnail;

	UPROPERTY()
	EItemGrade ItemGrade = EItemGrade::None;

	UPROPERTY(EditDefaultsOnly, Category = "Data", meta = (AllowPrivateAccess = true))
	TSubclassOf<AActor> OnHandItemClass;

	UPROPERTY(EditDefaultsOnly, Category = "Data", meta = (AllowPrivateAccess = true, ClampMin = "0", ClampMax = "1000", UIMin = "0", UIMax = "1000"))
	uint32 MaxItemCount = 0;

	UPROPERTY()
	TMap<EMetaDataKey, FString> MetaData;

	UPROPERTY(EditDefaultsOnly, Category = "Data", meta = (AllowPrivateAccess = true))
	TMap<EConstDataKey, FString> ConstData;
};

USTRUCT(BlueprintType)
struct SQUIRRELTEM_API FItemMetaInfo
{
	GENERATED_BODY()

	uint16 GetId() const
	{
		return Id;
	}

	void SetId(const uint16 NewId)
	{
		Id = NewId;
	}

	uint32 GetCurrentCount() const
	{
		return CurrentCount;
	}

	const TMap<EMetaDataKey, FString>& GetMetaData() const
	{
		return MetaData;
	}

	void SetMetaDataValue(const EMetaDataKey Key, const FString& Value)
	{
		MetaData.Add(Key, Value);
	}

	void SetMetaData(const TMap<EMetaDataKey, FString>& NewMetaData)
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

		for (const TTuple<EMetaDataKey, FString>& Data : MetaData)
		{
			if (!CompareItem.MetaData.Contains(Data.Key))
			{
				return false;
			}

			if (MetaData[Data.Key] != CompareItem.MetaData[Data.Key])
			{
				return false;
			}
		}

		return true;
	}

	bool IsValid() const
	{
		return Id > 0;
	}

private:
	UPROPERTY(EditDefaultsOnly, Category = "Data", meta = (ClampMin = "0", ClampMax = "1000", UIMin = "0", UIMax = "1000"))
	uint16 Id = 0;

	UPROPERTY(EditDefaultsOnly, Category = "Data", meta = (ClampMin = "0", ClampMax = "1000", UIMin = "0", UIMax = "1000"))
	uint32 CurrentCount = 0;

	UPROPERTY(EditDefaultsOnly, Category = "Data")
	TMap<EMetaDataKey, FString> MetaData;
};

FORCEINLINE uint32 GetTypeHash(const FItemMetaInfo& ItemMetaInfo)
{
	uint32 DefaultHash = 0;
	DefaultHash = HashCombine(DefaultHash, GetTypeHash(ItemMetaInfo.GetId()));

	for (const TTuple<EMetaDataKey, FString>& MetaData : ItemMetaInfo.GetMetaData())
	{
		DefaultHash = HashCombine(DefaultHash, GetTypeHash(MetaData));
	}

	return DefaultHash;
}

USTRUCT(BlueprintType)
struct FItemMetaInfo_Net
{
	GENERATED_BODY()

	UPROPERTY()
	int32 Id = 0;

	UPROPERTY()
	int32 Count = 0;

	UPROPERTY()
	TArray<EMetaDataKey> Keys;

	UPROPERTY()
	TArray<FString> Values;

	FItemMetaInfo_Net()
	{
	}

	FItemMetaInfo_Net(const FItemMetaInfo& Original)
	{
		Id = Original.GetId();
		Count = static_cast<int32>(Original.GetCurrentCount());

		for (const auto& Pair : Original.GetMetaData())
		{
			Keys.Add(Pair.Key);
			Values.Add(Pair.Value);
		}
	}

	void To(FItemMetaInfo& OutData) const
	{
		OutData.SetId(static_cast<uint16>(Id));
		OutData.SetCurrentCount(Count);
		TMap<EMetaDataKey, FString> MetaData;

		for (int32 i = 0; i < Keys.Num(); ++i)
		{
			MetaData.Add(Keys[i], Values[i]);
		}

		OutData.SetMetaData(MetaData);
	}
};
