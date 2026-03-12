#include "Components/ItemInventoryComponent.h"

#include "Kismet/KismetMathLibrary.h"
#include "Settings/SquirreltemSettings.h"

UItemInventoryComponent::UItemInventoryComponent()
{
	bWantsInitializeComponent = true;
}

void UItemInventoryComponent::InitializeComponent()
{
	Super::InitializeComponent();
	InitializeItemData();
}

void UItemInventoryComponent::InitializeItemData()
{
	InventoryList.SetNum(InventorySlotCount);

#if WITH_EDITOR
	for (const uint32 ItemId : InitialItemIdList)
	{
		FItemMetaInfo InitItemInfo;
		InitItemInfo.SetId(static_cast<uint16>(ItemId));
		InitItemInfo.SetCurrentCount(1);
		AddItem(InitItemInfo);
	}
#endif
}

FItemMetaInfo& UItemInventoryComponent::GetItemMetaInfoByIndex(const uint16 Index)
{
	return InventoryList[Index];
}

void UItemInventoryComponent::ChangeItem(const FItemMetaInfo& ItemInfo, const uint16 Index)
{
	InventoryList[Index] = ItemInfo;
	UpdateInventory_Internal();
}

void UItemInventoryComponent::SwapItem(const uint16 Prev, const uint16 Next)
{
	SwapItemInInventory_Internal(Prev, Next);
	UpdateInventory_Internal();
}

void UItemInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
}

bool UItemInventoryComponent::HasItemInInventory(const uint32 Id, const uint32 Count)
{
	if (CurrentRemainItemValue.Find(Id))
	{
		return CurrentRemainItemValue[Id] >= Count;
	}

	return false;
}

bool UItemInventoryComponent::HasExactItemInInventory(const FItemMetaInfo& ItemMetaInfo)
{
	if (CurrentRemainItemMetaValue.Find(ItemMetaInfo))
	{
		return CurrentRemainItemMetaValue[ItemMetaInfo] >= ItemMetaInfo.GetCurrentCount();
	}

	return false;
}

void UItemInventoryComponent::UpdateCurrentRemainItemValue()
{
	CurrentRemainItemValue.Empty();
	CurrentRemainItemMetaValue.Empty();

	for (int32 Index = 0; Index < InventoryList.Num(); ++Index)
	{
		if (!InventoryList[Index].IsValid() || InventoryList[Index].GetCurrentCount() == 0)
		{
			continue;
		}

		if (CurrentRemainItemValue.Find(InventoryList[Index].GetId()))
		{
			CurrentRemainItemValue[InventoryList[Index].GetId()] += InventoryList[Index].GetCurrentCount();
		}
		else
		{
			CurrentRemainItemValue.Add(InventoryList[Index].GetId(), InventoryList[Index].GetCurrentCount());
		}

		if (CurrentRemainItemMetaValue.Find(InventoryList[Index]))
		{
			CurrentRemainItemMetaValue[InventoryList[Index]] += InventoryList[Index].GetCurrentCount();
		}
		else
		{
			CurrentRemainItemMetaValue.Add(InventoryList[Index], InventoryList[Index].GetCurrentCount());
		}
	}
}

void UItemInventoryComponent::SwapItemInInventory_Internal(const uint16 Prev, const uint16 Next)
{
	const FItemMetaInfo Temp = InventoryList[Prev];
	InventoryList[Prev] = InventoryList[Next];
	InventoryList[Next] = Temp;
}

uint32 UItemInventoryComponent::AddItemToInventory(const uint16 Index, const FItemMetaInfo& ItemInfo)
{
	const USquirreltemSettings* ItemSettings = GetDefault<USquirreltemSettings>();
	const FItemInfoData* ItemInfoById = ItemSettings ? ItemSettings->FindItemInfoById(ItemInfo.GetId()) : nullptr;
	if (!ItemInfoById || ItemInfoById->GetMaxItemCount() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Squirreltem] Cannot add unknown ItemId %d to inventory."), ItemInfo.GetId());
		return ItemInfo.GetCurrentCount();
	}

	const uint16 CurrentItemCount = InventoryList[Index].GetCurrentCount() + ItemInfo.GetCurrentCount();
	const uint16 NextSetMainItemCount = UKismetMathLibrary::Min(CurrentItemCount, ItemInfoById->GetMaxItemCount());

	if (InventoryList[Index].GetId() == 0)
	{
		InventoryList[Index].SetId(ItemInfo.GetId());
		InventoryList[Index].SetMetaData(ItemInfo.GetMetaData());
	}
	InventoryList[Index].SetCurrentCount(NextSetMainItemCount);

	int32 RemainCount = CurrentItemCount - ItemInfoById->GetMaxItemCount();
	if (RemainCount > 0)
	{
		for (int32 SlotIndex = 0; SlotIndex < GetInventorySlotCount(); ++SlotIndex)
		{
			if (RemainCount == 0)
			{
				break;
			}

			if (InventoryList[SlotIndex].GetId() == 0)
			{
				FItemMetaInfo NewItemInfo;
				NewItemInfo.SetId(ItemInfo.GetId());
				NewItemInfo.SetMetaData(ItemInfo.GetMetaData());
				NewItemInfo.SetCurrentCount(UKismetMathLibrary::Min(RemainCount, ItemInfoById->GetMaxItemCount()));

				InventoryList[SlotIndex] = NewItemInfo;
				RemainCount -= UKismetMathLibrary::Min(RemainCount, ItemInfoById->GetMaxItemCount());
			}
		}
	}

	OnInventoryAddNotified.Broadcast(ItemInfo.GetId());
	UpdateInventory_Internal();
	return RemainCount > 0 ? RemainCount : 0;
}

bool UItemInventoryComponent::DropItem(const uint16 Index, const uint32 Count)
{
	if (Count == static_cast<uint32>(-1))
	{
		const FItemMetaInfo ClearItemMeta;
		InventoryList[Index] = ClearItemMeta;
		UpdateInventory_Internal();
		return true;
	}

	if (InventoryList[Index].GetCurrentCount() < Count)
	{
		return false;
	}

	if (InventoryList[Index].GetCurrentCount() - Count == 0)
	{
		const FItemMetaInfo ClearItemMeta;
		InventoryList[Index] = ClearItemMeta;
	}
	else
	{
		InventoryList[Index].SetCurrentCount(InventoryList[Index].GetCurrentCount() - Count);
	}

	UpdateInventory_Internal();
	return true;
}

FItemMetaInfo UItemInventoryComponent::GetFirstMetaInfo(const uint16 ItemId)
{
	for (const FItemMetaInfo& MetaInfo : InventoryList)
	{
		if (MetaInfo.GetId() == ItemId)
		{
			return MetaInfo;
		}
	}

	return FItemMetaInfo();
}

uint32 UItemInventoryComponent::GetCurrentCount(const uint16 Id)
{
	uint32 Result = 0;
	if (Id == 0)
	{
		return Result;
	}

	for (int32 Index = 0, ListCount = GetInventorySlotCount(); Index < ListCount; ++Index)
	{
		if (InventoryList[Index].GetId() == Id)
		{
			Result += InventoryList[Index].GetCurrentCount();
		}
	}

	return Result;
}

uint32 UItemInventoryComponent::AddItem(const FItemMetaInfo& ItemInfo)
{
	const USquirreltemSettings* ItemSettings = GetDefault<USquirreltemSettings>();
	const FItemInfoData* ItemInfoById = ItemSettings ? ItemSettings->FindItemInfoById(ItemInfo.GetId()) : nullptr;
	if (!ItemInfoById || ItemInfoById->GetMaxItemCount() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Squirreltem] Cannot stack unknown ItemId %d in inventory."), ItemInfo.GetId());
		return ItemInfo.GetCurrentCount();
	}

	bool bHasInventory = false;
	uint32 RemainResult = ItemInfo.GetCurrentCount();

	for (int32 Index = 0; Index < GetInventorySlotCount(); ++Index)
	{
		if (InventoryList[Index] == ItemInfo && InventoryList[Index].GetCurrentCount() < ItemInfoById->GetMaxItemCount())
		{
			bHasInventory = true;
			RemainResult = AddItemToInventory(Index, ItemInfo);
			break;
		}
	}

	if (!bHasInventory && RemainResult > 0)
	{
		for (int32 Index = 0; Index < GetInventorySlotCount(); ++Index)
		{
			if (InventoryList[Index].GetId() == 0)
			{
				RemainResult = AddItemToInventory(Index, ItemInfo);
				break;
			}
		}
	}

	if (RemainResult > 0)
	{
		UE_LOG(LogTemp, Error, TEXT("인벤토리 초과함"));
		return RemainResult;
	}

	return 0;
}

bool UItemInventoryComponent::RemoveItem(const uint16 Id, const uint32 Count)
{
	uint32 RemainNum = Count;
	TArray<uint32> CanRemoveIndexList;

	for (int32 Index = 0; Index < GetInventorySlotCount(); ++Index)
	{
		if (InventoryList[Index].GetId() == Id)
		{
			CanRemoveIndexList.Add(Index);
			RemainNum = UKismetMathLibrary::Max(RemainNum - InventoryList[Index].GetCurrentCount(), 0);
		}

		if (RemainNum <= 0)
		{
			break;
		}
	}

	if (RemainNum > 0)
	{
		return false;
	}

	RemainNum = Count;
	for (const uint32 RemoveIndex : CanRemoveIndexList)
	{
		const uint32 RemoveCount = UKismetMathLibrary::Min(InventoryList[RemoveIndex].GetCurrentCount(), RemainNum);
		InventoryList[RemoveIndex].SetCurrentCount(InventoryList[RemoveIndex].GetCurrentCount() - RemoveCount);
		RemainNum -= RemoveCount;

		if (InventoryList[RemoveIndex].GetCurrentCount() == 0)
		{
			const FItemMetaInfo ClearItemMeta;
			InventoryList[RemoveIndex] = ClearItemMeta;
		}

		if (RemainNum == 0)
		{
			UpdateInventory_Internal();
			return true;
		}
	}

	return false;
}

bool UItemInventoryComponent::RemoveExactItem(const FItemMetaInfo& ItemInfo)
{
	uint32 RemainNum = ItemInfo.GetCurrentCount();
	TArray<uint32> CanRemoveIndexList;

	for (int32 Index = 0; Index < GetInventorySlotCount(); ++Index)
	{
		if (InventoryList[Index] == ItemInfo)
		{
			CanRemoveIndexList.Add(Index);
			RemainNum = UKismetMathLibrary::Max(RemainNum - InventoryList[Index].GetCurrentCount(), 0);
		}

		if (RemainNum <= 0)
		{
			break;
		}
	}

	if (RemainNum > 0)
	{
		return false;
	}

	RemainNum = ItemInfo.GetCurrentCount();
	for (const uint32 RemoveIndex : CanRemoveIndexList)
	{
		const uint32 RemoveCount = UKismetMathLibrary::Min(InventoryList[RemoveIndex].GetCurrentCount(), RemainNum);
		InventoryList[RemoveIndex].SetCurrentCount(InventoryList[RemoveIndex].GetCurrentCount() - RemoveCount);
		RemainNum -= RemoveCount;

		if (InventoryList[RemoveIndex].GetCurrentCount() == 0)
		{
			const FItemMetaInfo ClearItemMeta;
			InventoryList[RemoveIndex] = ClearItemMeta;
		}

		if (RemainNum == 0)
		{
			UpdateInventory_Internal();
			return true;
		}
	}

	return false;
}

void UItemInventoryComponent::UpdateInventory_Internal()
{
	UpdateCurrentRemainItemValue();
	OnInventoryUpdateNotified.Broadcast();
}
