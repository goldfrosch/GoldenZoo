#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SquirreltemData.h"
#include "ItemInventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdateNotified);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryAddNotified, const FName&, ItemId);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SQUIRRELTEM_API UItemInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UItemInventoryComponent();

	uint8 GetInventorySlotCount() const
	{
		return InventorySlotCount;
	}

	const TArray<FItemMetaInfo>& GetInventoryList() const
	{
		return InventoryList;
	}

	virtual FItemMetaInfo GetFirstMetaInfo(const FName& ItemId);
	uint32 GetCurrentCount(const FName& Id);
	uint32 AddItem(const FItemMetaInfo& ItemInfo);
	bool HasItemInInventory(const FName& Id, const uint32 Count);
	bool HasExactItemInInventory(const FItemMetaInfo& ItemMetaInfo);
	bool DropItem(const uint16 Index, const uint32 Count);
	bool RemoveItem(const FName& Id, const uint32 Count);
	bool RemoveExactItem(const FItemMetaInfo& ItemInfo);
	uint32 AddItemToInventory(const uint16 Index, const FItemMetaInfo& ItemInfo);

	TMap<FName, uint32> GetCurrentRemainItemValue() const
	{
		return CurrentRemainItemValue;
	}

	TMap<FItemMetaInfo, uint32> GetCurrentRemainItemMetaValue() const
	{
		return CurrentRemainItemMetaValue;
	}

	FOnInventoryUpdateNotified OnInventoryUpdateNotified;
	FOnInventoryAddNotified OnInventoryAddNotified;

	void InitializeItemData();
	virtual FItemMetaInfo& GetItemMetaInfoByIndex(const uint16 Index);
	virtual void ChangeItem(const FItemMetaInfo& ItemInfo, const uint16 Index);
	virtual void SwapItem(const uint16 Prev, const uint16 Index);

protected:
	virtual void BeginPlay() override;
	virtual void InitializeComponent() override;

private:
	void SwapItemInInventory_Internal(const uint16 Prev, const uint16 Next);
	void UpdateInventory_Internal();
	void UpdateCurrentRemainItemValue();

	TMap<FName, uint32> CurrentRemainItemValue;
	TMap<FItemMetaInfo, uint32> CurrentRemainItemMetaValue;

	UPROPERTY(EditDefaultsOnly, Category = "Options", meta = (AllowPrivateAccess = true))
	uint8 InventorySlotCount = 54;

	UPROPERTY(EditDefaultsOnly, Category = "Option|Test", meta = (AllowPrivateAccess = true))
	TArray<FName> InitialItemIdList;

	UPROPERTY()
	TArray<FItemMetaInfo> InventoryList;
};
