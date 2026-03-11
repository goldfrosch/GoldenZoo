#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "HQQuestDeveloperSettings.generated.h"

class UDataTable;
class UHQQuestHandlerRegistry;

/**
 * HowlingQuest가 시작 시 로드할 퀘스트 데이터 소스를 설정합니다.
 */
UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "HowlingQuest"))
class HOWLINGQUEST_API UHQQuestDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	/** 퀘스트 기본 정보를 담은 DataTable (RowType: FHQQuestStoryRow). */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "HowlingQuest")
	TSoftObjectPtr<UDataTable> QuestDataTable;

	/** 태그 기반 핸들러 레지스트리 DataAsset. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "HowlingQuest")
	TSoftObjectPtr<UHQQuestHandlerRegistry> HandlerRegistry;

	/**
	 * 검사할 퀘스트 정의 에셋 디렉토리 목록.
	 * '/Game/Quests' 같은 long package path를 권장합니다.
	 */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "HowlingQuest")
	TArray<FString> QuestDefinitionDirectories;
};
