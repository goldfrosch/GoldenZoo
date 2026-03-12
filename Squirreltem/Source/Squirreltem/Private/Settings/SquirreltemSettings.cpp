#include "Settings/SquirreltemSettings.h"

#include "Engine/DataTable.h"

USquirreltemSettings::USquirreltemSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

#if WITH_EDITOR
void USquirreltemSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName PropertyName = PropertyChangedEvent.MemberProperty
		? PropertyChangedEvent.MemberProperty->GetFName()
		: NAME_None;

	if (PropertyName == GET_MEMBER_NAME_CHECKED(USquirreltemSettings, ItemDatabase))
	{
		ItemInfoById.Empty();
		Initialize();
	}
}
#endif

void USquirreltemSettings::PostInitProperties()
{
	Super::PostInitProperties();
	Initialize();
}

void USquirreltemSettings::Initialize()
{
	ItemInfoById.Empty();

	const UDataTable* LoadedDataTable = ItemDatabase.LoadSynchronous();
	if (!LoadedDataTable)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("ItemDatabase가 설정되지 않았거나 로드할 수 없습니다. 프로젝트 세팅에서 Squirreltem ItemDatabase를 설정해주세요."));
		return;
	}

	uint16 NextFallbackItemId = 1;
	for (const TPair<FName, uint8*>& RowEntry : LoadedDataTable->GetRowMap())
	{
		const FItemInfoData* InfoItem = reinterpret_cast<const FItemInfoData*>(RowEntry.Value);
		if (!InfoItem)
		{
			continue;
		}

		FItemInfoData ResolvedItemInfo = *InfoItem;
		uint16 ResolvedItemId = ResolvedItemInfo.GetItemId();
		if (ResolvedItemId == 0)
		{
			const FString RowNameAsString = RowEntry.Key.ToString();
			if (RowNameAsString.IsNumeric())
			{
				const int32 ParsedItemId = FCString::Atoi(*RowNameAsString);
				if (ParsedItemId > 0 && ParsedItemId <= MAX_uint16)
				{
					ResolvedItemId = static_cast<uint16>(ParsedItemId);
				}
			}
		}

		if (ResolvedItemId == 0)
		{
			while (ItemInfoById.Contains(NextFallbackItemId))
			{
				++NextFallbackItemId;
			}

			ResolvedItemId = NextFallbackItemId;
			UE_LOG(LogTemp, Warning,
				TEXT("[Squirreltem] Item row '%s' has no valid ItemId. Falling back to generated id %d."),
				*RowEntry.Key.ToString(),
				ResolvedItemId);
		}

		if (ItemInfoById.Contains(ResolvedItemId))
		{
			UE_LOG(LogTemp, Warning,
				TEXT("[Squirreltem] Duplicate ItemId %d detected for row '%s'. The row will be ignored."),
				ResolvedItemId,
				*RowEntry.Key.ToString());
			continue;
		}

		ResolvedItemInfo.SetItemId(ResolvedItemId);
		ItemInfoById.Add(ResolvedItemId, MoveTemp(ResolvedItemInfo));
		NextFallbackItemId = FMath::Max<uint16>(NextFallbackItemId, ResolvedItemId + 1);
	}
}

const FItemInfoData* USquirreltemSettings::FindItemInfoById(const uint16 Id) const
{
	return ItemInfoById.Find(Id);
}

bool USquirreltemSettings::HasItemInfo(const uint16 Id) const
{
	return ItemInfoById.Contains(Id);
}

const FItemInfoData& USquirreltemSettings::GetItemInfoById(const uint16 Id) const
{
	if (const FItemInfoData* FoundItemInfo = FindItemInfoById(Id))
	{
		return *FoundItemInfo;
	}

	UE_LOG(LogTemp, Warning, TEXT("[Squirreltem] Requested unknown ItemId %d."), Id);
	return EmptyItemInfo;
}

FItemMetaInfo USquirreltemSettings::GetInitialItemMetaDataById(const uint16 Id) const
{
	const FItemInfoData* InitialData = FindItemInfoById(Id);
	if (!InitialData)
	{
		return FItemMetaInfo();
	}

	FItemMetaInfo NewMetaInfo;
	NewMetaInfo.SetId(Id);
	NewMetaInfo.SetCurrentCount(1);
	NewMetaInfo.SetMetaData(InitialData->GetMetaData());
	return NewMetaInfo;
}

const FString* USquirreltemSettings::FindConstDataValue(const uint16 Id, const EConstDataKey Key) const
{
	const FItemInfoData* ItemInfoData = FindItemInfoById(Id);
	if (!ItemInfoData)
	{
		return nullptr;
	}

	return ItemInfoData->GetConstData().Find(Key);
}

FString USquirreltemSettings::GetItemUsingType(const uint16 Id) const
{
	if (const FString* FindData = FindConstDataValue(Id, EConstDataKey::ItemUseType))
	{
		return *FindData;
	}

	return FString();
}

FString USquirreltemSettings::GetItemCategoryTextById(const uint16 Id) const
{
	const FItemInfoData* ItemInfoData = FindItemInfoById(Id);
	if (!ItemInfoData)
	{
		return TEXT("없음");
	}

	switch (ItemInfoData->GetItemType())
	{
	case EItemType::Undefined:
		return TEXT("없음");
	case EItemType::Build:
		return TEXT("건축");
	case EItemType::Consume:
		return TEXT("소비");
	case EItemType::Equipment:
		return TEXT("장비");
	case EItemType::Ingredient:
		return TEXT("재료");
	case EItemType::Interactive:
		return TEXT("상호작용");
	default:
		return TEXT("없음");
	}
}

bool USquirreltemSettings::IsItemCanHousing(const uint16 Id) const
{
	const FString* FindData = FindConstDataValue(Id, EConstDataKey::ItemUseType);
	return FindData && FindData->Equals(TEXT("Housing"));
}

bool USquirreltemSettings::IsItemCanInteraction(const uint16 Id) const
{
	const FItemInfoData* ItemInfoData = FindItemInfoById(Id);
	return ItemInfoData && ItemInfoData->GetItemType() == EItemType::Interactive;
}

bool USquirreltemSettings::IsInfiniteDurability(const uint16 Id) const
{
	const FString* FindData = FindConstDataValue(Id, EConstDataKey::MaxDurability);
	if (!FindData || FindData->IsEmpty() || !FindData->IsNumeric())
	{
		return false;
	}

	return FCString::Atoi(**FindData) < 0;
}

uint16 USquirreltemSettings::GetGeneratedOtherItemIdById(const uint16 Id) const
{
	const FString* FindData = FindConstDataValue(Id, EConstDataKey::GeneratedItemId);
	if (!FindData || FindData->IsEmpty() || !FindData->IsNumeric())
	{
		return 0;
	}

	return static_cast<uint16>(FCString::Atoi(**FindData));
}

uint16 USquirreltemSettings::GetChanceBasedSpawnItemIdById(const uint16 Id) const
{
	const FString* FindData = FindConstDataValue(Id, EConstDataKey::ChanceBasedSpawnItemId);
	if (!FindData || FindData->IsEmpty() || !FindData->IsNumeric())
	{
		return 0;
	}

	return static_cast<uint16>(FCString::Atoi(**FindData));
}

FName USquirreltemSettings::GetSocketNameById(const uint16 Id) const
{
	const FString* FindData = FindConstDataValue(Id, EConstDataKey::SocketName);
	if (!FindData || FindData->IsEmpty())
	{
		return NAME_None;
	}

	return FName(**FindData);
}
