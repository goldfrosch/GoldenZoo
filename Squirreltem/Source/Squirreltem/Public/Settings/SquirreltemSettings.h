#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Engine/DeveloperSettings.h"
#include "SquirreltemData.h"
#include "GameplayTagContainer.h"
#include "SquirreltemSettings.generated.h"

UCLASS(Config = ProjectSettings, DefaultConfig, meta = (DisplayName = "Squirreltem Settings"))
class SQUIRRELTEM_API USquirreltemSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	USquirreltemSettings(const FObjectInitializer& ObjectInitializer);

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	const FItemInfoData* FindItemInfoById(const FName& Id) const;
	const FItemInfoData* FindItemInfoById(uint16 Id) const;

	bool HasItemInfo(const FName& Id) const;
	bool HasItemInfo(uint16 Id) const;

	const FItemInfoData& GetItemInfoById(const FName& Id) const;
	const FItemInfoData& GetItemInfoById(uint16 Id) const;

	FItemMetaInfo GetInitialItemMetaDataById(const FName& Id) const;
	FItemMetaInfo GetInitialItemMetaDataById(uint16 Id) const;

	FString GetItemUsingType(const FName& Id) const;
	FString GetItemUsingType(uint16 Id) const;

	FString GetItemCategoryTextById(const FName& Id) const;
	FString GetItemCategoryTextById(uint16 Id) const;

	bool IsItemCanHousing(const FName& Id) const;
	bool IsItemCanHousing(uint16 Id) const;

	bool IsItemCanInteraction(const FName& Id) const;
	bool IsItemCanInteraction(uint16 Id) const;

	bool IsInfiniteDurability(const FName& Id) const;
	bool IsInfiniteDurability(uint16 Id) const;

	uint16 GetGeneratedOtherItemIdById(const FName& Id) const;
	uint16 GetGeneratedOtherItemIdById(uint16 Id) const;

	uint16 GetChanceBasedSpawnItemIdById(const FName& Id) const;
	uint16 GetChanceBasedSpawnItemIdById(uint16 Id) const;

	FName GetSocketNameById(const FName& Id) const;
	FName GetSocketNameById(uint16 Id) const;

	UPROPERTY(Config, EditAnywhere, Category = "Item Database")
	TSoftObjectPtr<UDataTable> ItemDatabase;

protected:
	virtual void PostInitProperties() override;

private:
	static FName ConvertItemIdToName(uint16 Id);

	const FString* FindConstDataValue(const FName& Id, const FGameplayTag& Key) const;

	TMap<FName, FItemInfoData> ItemInfoById;
	FItemInfoData EmptyItemInfo;

	void Initialize();
};
