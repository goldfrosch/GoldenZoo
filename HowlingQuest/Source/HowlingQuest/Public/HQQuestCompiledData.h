#pragma once

#include "CoreMinimal.h"
#include "HQQuestPayload.h"
#include "HQQuestRow.h"
#include "HQQuestTypes.h"
#include "HQQuestCompiledData.generated.h"

/**
 * 시작 시점에 DataTable Row와 Definition Asset을 병합한 정적 퀘스트 데이터입니다.
 */
USTRUCT(BlueprintType)
struct HOWLINGQUEST_API FHQQuestCompiledData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HowlingQuest")
	FName QuestRowName = NAME_None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HowlingQuest")
	FHQQuestStoryRow StoryRow;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HowlingQuest")
	TArray<FHQQuestTaggedPayload> Requirements;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HowlingQuest")
	TArray<FHQQuestTaggedPayload> Rewards;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HowlingQuest")
	TArray<FHQQuestTaggedPayload> Extensions;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HowlingQuest")
	TArray<FHQQuestMetaField> MetaFields;
};
