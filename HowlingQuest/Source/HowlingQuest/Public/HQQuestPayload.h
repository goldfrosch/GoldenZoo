#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "StructUtils/InstancedStruct.h"
#include "HQQuestPayload.generated.h"

/**
 * 퀘스트 정의에서 사용하는 "타입 태그 + Payload" 컨테이너.
 * - TypeTag : 예) HowlingQuest.Require.HasItem
 * - Payload : 해당 타입의 설정 데이터(UStruct 1개 인스턴스)
 */
USTRUCT(BlueprintType)
struct HOWLINGQUEST_API FHQQuestTaggedPayload
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HowlingQuest")
	FGameplayTag TypeTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HowlingQuest")
	FInstancedStruct Payload;
};

