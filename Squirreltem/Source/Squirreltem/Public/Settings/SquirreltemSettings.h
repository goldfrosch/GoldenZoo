#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Engine/DeveloperSettings.h"
#include "SquirreltemData.h"
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

	const FItemInfoData* FindItemInfoById(const uint16 Id) const;

	bool HasItemInfo(const uint16 Id) const;

	const FItemInfoData& GetItemInfoById(const uint16 Id) const;

	FItemMetaInfo GetInitialItemMetaDataById(const uint16 Id) const;

	FString GetItemUsingType(const uint16 Id) const;

	FString GetItemCategoryTextById(const uint16 Id) const;

	bool IsItemCanHousing(const uint16 Id) const;

	bool IsItemCanInteraction(const uint16 Id) const;

	bool IsInfiniteDurability(const uint16 Id) const;

	uint16 GetGeneratedOtherItemIdById(const uint16 Id) const;

	uint16 GetChanceBasedSpawnItemIdById(const uint16 Id) const;

	FName GetSocketNameById(const uint16 Id) const;

	UPROPERTY(Config, EditAnywhere, Category = "Item Database")
	TSoftObjectPtr<UDataTable> ItemDatabase;

protected:
	virtual void PostInitProperties() override;

private:
	const FString* FindConstDataValue(const uint16 Id, const EConstDataKey Key) const;

	TMap<uint16, FItemInfoData> ItemInfoById;
	FItemInfoData EmptyItemInfo;

	void Initialize();
};
