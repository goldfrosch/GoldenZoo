#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "HQQuestHandlerRegistry.generated.h"

/**
 * 태그 → 핸들러 클래스 매핑용 구조체.
 */
USTRUCT(BlueprintType)
struct HOWLINGQUEST_API FHQQuestHandlerBinding
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HowlingQuest")
	FGameplayTag TypeTag;

	/** 해당 타입을 처리할 핸들러 클래스 (UObject 기반). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HowlingQuest")
	TSoftClassPtr<UObject> HandlerClass;
};

/**
 * 퀘스트 요구/보상/Objective에 대한 핸들러 레지스트리.
 * 프로젝트나 모듈에서 DataAsset을 생성해 설정합니다.
 */
UCLASS(BlueprintType)
class HOWLINGQUEST_API UHQQuestHandlerRegistry : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HowlingQuest|Handlers")
	TArray<FHQQuestHandlerBinding> RequirementHandlers;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HowlingQuest|Handlers")
	TArray<FHQQuestHandlerBinding> RewardHandlers;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HowlingQuest|Handlers")
	TArray<FHQQuestHandlerBinding> ObjectiveHandlers;
};

