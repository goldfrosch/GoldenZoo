#pragma once

#include "CoreMinimal.h"
#include "HQQuestEventPayload.generated.h"

/**
 * 모든 퀘스트 이벤트가 상속하는 공통 베이스.
 * 필요 시 Instigator, Timestamp 등 공통 필드를 추가할 수 있습니다.
 */
USTRUCT(BlueprintType)
struct HOWLINGQUEST_API FHQQuestEventPayloadBase
{
	GENERATED_BODY()
};

/**
 * 아이템 획득 이벤트 예시.
 * 게임 로직에서 행동 발생 시 이 Struct를 만들어 Subsystem에 전달합니다.
 */
USTRUCT(BlueprintType)
struct HOWLINGQUEST_API FHQQuestEventItemAcquiredPayload : public FHQQuestEventPayloadBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "HowlingQuest")
	int32 ItemId = 0;

	UPROPERTY(BlueprintReadOnly, Category = "HowlingQuest")
	int32 Count = 1;
};

