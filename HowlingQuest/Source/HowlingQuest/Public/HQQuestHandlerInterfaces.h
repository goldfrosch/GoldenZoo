#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"
#include "HQQuestPayload.h"
#include "HQQuestEventPayload.h"
#include "HQQuestHandlerInterfaces.generated.h"

/**
 * 요구 조건 핸들러 인터페이스.
 * TypeTag 및 DefinitionPayload를 기반으로 조건 충족 여부를 판단합니다.
 */
UINTERFACE(BlueprintType)
class HOWLINGQUEST_API UHQQuestRequirementHandler : public UInterface
{
	GENERATED_BODY()
};

class HOWLINGQUEST_API IHQQuestRequirementHandler
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HowlingQuest")
	bool CanHandle(FGameplayTag TypeTag) const;

	/** 요구 조건이 현재 충족되었는지 판단. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HowlingQuest")
	bool IsSatisfied(const FHQQuestTaggedPayload& DefinitionPayload) const;
};

/**
 * 보상 핸들러 인터페이스.
 * DefinitionPayload를 기반으로 실제 보상을 지급합니다.
 */
UINTERFACE(BlueprintType)
class HOWLINGQUEST_API UHQQuestRewardHandler : public UInterface
{
	GENERATED_BODY()
};

class HOWLINGQUEST_API IHQQuestRewardHandler
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HowlingQuest")
	bool CanHandle(FGameplayTag TypeTag) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HowlingQuest")
	void GiveReward(const FHQQuestTaggedPayload& DefinitionPayload) const;
};

/**
 * Objective(목표) 핸들러 인터페이스.
 * DefinitionPayload와 런타임 이벤트를 기반으로 진행도 업데이트/완료 여부를 관리합니다.
 */
UINTERFACE(BlueprintType)
class HOWLINGQUEST_API UHQQuestObjectiveHandler : public UInterface
{
	GENERATED_BODY()
};

class HOWLINGQUEST_API IHQQuestObjectiveHandler
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HowlingQuest")
	bool CanHandle(FGameplayTag TypeTag) const;

	/** 런타임 이벤트 발생 시 호출되어 Objective 진행도를 업데이트합니다. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HowlingQuest")
	void OnEvent(const FHQQuestTaggedPayload& DefinitionPayload,
	             const FHQQuestEventPayloadBase& EventPayload);

	/** Objective가 완료 상태인지 판단합니다. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HowlingQuest")
	bool IsComplete(const FHQQuestTaggedPayload& DefinitionPayload) const;
};

