#include "Settings/SquirreltemSettings.h"

#include "Engine/DataTable.h"
#include "SquirreltemTags.h"

USquirreltemSettings::USquirreltemSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FName USquirreltemSettings::ConvertItemIdToName(const uint16 Id)
{
	return FName(*LexToString(Id));
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

	for (const TPair<FName, uint8*>& RowEntry : LoadedDataTable->GetRowMap())
	{
		const FItemInfoData* InfoItem = reinterpret_cast<const FItemInfoData*>(RowEntry.Value);
		if (!InfoItem)
		{
			continue;
		}

		FItemInfoData ResolvedItemInfo = *InfoItem;
		FName ResolvedItemId = RowEntry.Key;
		
		ItemInfoById.Add(ResolvedItemId, MoveTemp(ResolvedItemInfo));
	}
}

const FItemInfoData* USquirreltemSettings::FindItemInfoById(const FName& Id) const
{
	return ItemInfoById.Find(Id);
}

const FItemInfoData* USquirreltemSettings::FindItemInfoById(const uint16 Id) const
{
	return FindItemInfoById(ConvertItemIdToName(Id));
}

bool USquirreltemSettings::HasItemInfo(const FName& Id) const
{
	return ItemInfoById.Contains(Id);
}

bool USquirreltemSettings::HasItemInfo(const uint16 Id) const
{
	return HasItemInfo(ConvertItemIdToName(Id));
}

const FItemInfoData& USquirreltemSettings::GetItemInfoById(const FName& Id) const
{
	if (const FItemInfoData* FoundItemInfo = FindItemInfoById(Id))
	{
		return *FoundItemInfo;
	}

	UE_LOG(LogTemp, Warning, TEXT("[Squirreltem] Requested unknown ItemId %s."), *Id.ToString());
	return EmptyItemInfo;
}

const FItemInfoData& USquirreltemSettings::GetItemInfoById(const uint16 Id) const
{
	return GetItemInfoById(ConvertItemIdToName(Id));
}

FItemMetaInfo USquirreltemSettings::GetInitialItemMetaDataById(const FName& Id) const
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

FItemMetaInfo USquirreltemSettings::GetInitialItemMetaDataById(const uint16 Id) const
{
	return GetInitialItemMetaDataById(ConvertItemIdToName(Id));
}

const FString* USquirreltemSettings::FindConstDataValue(const FName& Id, const FGameplayTag& Key) const
{
	const FItemInfoData* ItemInfoData = FindItemInfoById(Id);
	if (!ItemInfoData)
	{
		return nullptr;
	}

	return ItemInfoData->GetConstData().Find(Key);
}

FString USquirreltemSettings::GetItemUsingType(const FName& Id) const
{
	if (const FString* FindData = FindConstDataValue(Id, SquirreltemGameplayTags::ConstData_ItemUseType))
	{
		return *FindData;
	}

	return FString();
}

FString USquirreltemSettings::GetItemUsingType(const uint16 Id) const
{
	return GetItemUsingType(ConvertItemIdToName(Id));
}

FString USquirreltemSettings::GetItemCategoryTextById(const FName& Id) const
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

FString USquirreltemSettings::GetItemCategoryTextById(const uint16 Id) const
{
	return GetItemCategoryTextById(ConvertItemIdToName(Id));
}

bool USquirreltemSettings::IsItemCanHousing(const FName& Id) const
{
	const FString* FindData = FindConstDataValue(Id, SquirreltemGameplayTags::ConstData_ItemUseType);
	return FindData && FindData->Equals(TEXT("Housing"));
}

bool USquirreltemSettings::IsItemCanHousing(const uint16 Id) const
{
	return IsItemCanHousing(ConvertItemIdToName(Id));
}

bool USquirreltemSettings::IsItemCanInteraction(const FName& Id) const
{
	const FItemInfoData* ItemInfoData = FindItemInfoById(Id);
	return ItemInfoData && ItemInfoData->GetItemType() == EItemType::Interactive;
}

bool USquirreltemSettings::IsItemCanInteraction(const uint16 Id) const
{
	return IsItemCanInteraction(ConvertItemIdToName(Id));
}

bool USquirreltemSettings::IsInfiniteDurability(const FName& Id) const
{
	const FString* FindData = FindConstDataValue(Id, SquirreltemGameplayTags::ConstData_MaxDurability);
	if (!FindData || FindData->IsEmpty() || !FindData->IsNumeric())
	{
		return false;
	}

	return FCString::Atoi(**FindData) < 0;
}

bool USquirreltemSettings::IsInfiniteDurability(const uint16 Id) const
{
	return IsInfiniteDurability(ConvertItemIdToName(Id));
}

uint16 USquirreltemSettings::GetGeneratedOtherItemIdById(const FName& Id) const
{
	const FString* FindData = FindConstDataValue(Id, SquirreltemGameplayTags::ConstData_GeneratedItemId);
	if (!FindData || FindData->IsEmpty() || !FindData->IsNumeric())
	{
		return 0;
	}

	return static_cast<uint16>(FCString::Atoi(**FindData));
}

uint16 USquirreltemSettings::GetGeneratedOtherItemIdById(const uint16 Id) const
{
	return GetGeneratedOtherItemIdById(ConvertItemIdToName(Id));
}

uint16 USquirreltemSettings::GetChanceBasedSpawnItemIdById(const FName& Id) const
{
	const FString* FindData = FindConstDataValue(Id, SquirreltemGameplayTags::ConstData_ChanceBasedSpawnItemId);
	if (!FindData || FindData->IsEmpty() || !FindData->IsNumeric())
	{
		return 0;
	}

	return static_cast<uint16>(FCString::Atoi(**FindData));
}

uint16 USquirreltemSettings::GetChanceBasedSpawnItemIdById(const uint16 Id) const
{
	return GetChanceBasedSpawnItemIdById(ConvertItemIdToName(Id));
}

FName USquirreltemSettings::GetSocketNameById(const FName& Id) const
{
	const FString* FindData = FindConstDataValue(Id, SquirreltemGameplayTags::ConstData_SocketName);
	if (!FindData || FindData->IsEmpty())
	{
		return NAME_None;
	}

	return FName(**FindData);
}

FName USquirreltemSettings::GetSocketNameById(const uint16 Id) const
{
	return GetSocketNameById(ConvertItemIdToName(Id));
}
