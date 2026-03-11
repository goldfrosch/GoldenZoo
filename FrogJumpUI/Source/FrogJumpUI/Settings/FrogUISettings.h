#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "FrogUISettings.generated.h"

class UBaseUIView;

UCLASS(Config = ProjectSettings, DefaultConfig
	, meta = (DisplayName = "UI Settings"))
class FROGJUMPUI_API UFrogUISettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	UFrogUISettings(const FObjectInitializer& ObjectInitializer);

	TArray<FString> GetAllKeys() const;

	bool HasUIKey(const FString& Key) const;

	TSubclassOf<UBaseUIView> GetUIViewClassByKey(const FString& Key) const;

protected:
	/** 프로젝트 세팅 카테고리 이름 */
	virtual FName GetCategoryName() const override
	{
		return FName(TEXT("Game"));
	}

	/** 프로젝트 세팅 섹션 이름 */
	virtual FName GetSectionName() const override
	{
		return FName(TEXT("UI Settings"));
	}

private:
	UPROPERTY(Config, EditAnywhere, Category = "UI Classes"
		, meta = (AllowedClasses = "/Script/MSUIModule.BaseUIView"))
	TMap<FString, TSubclassOf<UBaseUIView>> UIViewClasses;
};
