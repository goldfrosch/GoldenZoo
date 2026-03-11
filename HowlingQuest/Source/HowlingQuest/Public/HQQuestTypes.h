#pragma once

#include "CoreMinimal.h"
#include "StructUtils/InstancedStruct.h"
#include "HQQuestTypes.generated.h"

/**
 * 메타데이터 한 항목.
 * Key와 구조체 Value를 조합해 UI/연출/프로젝트별 확장 데이터를 담습니다.
 */
USTRUCT(BlueprintType)
struct HOWLINGQUEST_API FHQQuestMetaField
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HowlingQuest")
	FName Key;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HowlingQuest")
	FInstancedStruct Value;
};

/**
 * 아이템 요구/보상용 기본 Payload.
 */
USTRUCT(BlueprintType)
struct HOWLINGQUEST_API FHQQuestItemPayload
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HowlingQuest")
	int32 ItemId = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HowlingQuest")
	int32 Count = 1;
};

/**
 * 골드 요구/보상용 기본 Payload.
 */
USTRUCT(BlueprintType)
struct HOWLINGQUEST_API FHQQuestGoldPayload
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HowlingQuest")
	int32 GoldAmount = 0;
};

