#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "HQQuestRow.generated.h"

/**
 * HowlingQuest 메인 퀘스트 정의 Row.
 * CSV/엑셀 → DataTable 로드용 구조체입니다.
 */
USTRUCT(BlueprintType)
struct HOWLINGQUEST_API FHQQuestStoryRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HowlingQuest")
	FText Title;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HowlingQuest", meta = (MultiLine = true))
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HowlingQuest", meta = (MultiLine = true))
	FText Scenario;

	/** 메인/서브/기타 스토리 타입 (예: HowlingQuest.StoryType.Main). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HowlingQuest")
	FGameplayTag StoryTypeTag;

	/** 시작 전 선행 완료가 필요한 퀘스트 RowName 목록. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HowlingQuest")
	TArray<FName> RequireQuestRowNames;
};

