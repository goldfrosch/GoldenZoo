#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "UObject/SoftObjectPtr.h"

#include "BCraftingEditorSettings.generated.h"

class UDataTable;

UCLASS(Config = EditorPerProjectUserSettings, DefaultConfig, meta = (DisplayName = "BeaverCrafting"))
class BEAVERCRAFTINGEDITOR_API UBCraftingEditorSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "BeaverCrafting")
	TSoftObjectPtr<UDataTable> RecipeTable;
};
