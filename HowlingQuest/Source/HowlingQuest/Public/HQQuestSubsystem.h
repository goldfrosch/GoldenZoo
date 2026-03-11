#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "HQQuestCompiledData.h"
#include "HQQuestHandlerRegistry.h"
#include "HQQuestEventPayload.h"
#include "HQQuestSubsystem.generated.h"

class FSubsystemCollectionBase;

/**
 * 게임 코드에서 퀘스트와 상호작용하는 진입점 Subsystem.
 * - DataTable RowName + DefinitionAsset 기반 정적 퀘스트 데이터 로드
 * - 런타임 이벤트 전달
 * - (향후) 활성 퀘스트 상태 관리
 */
UCLASS()
class HOWLINGQUEST_API UHQQuestSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/** 특정 RowName의 퀘스트를 시작합니다. */
	UFUNCTION(BlueprintCallable, Category = "HowlingQuest")
	void StartQuest(FName QuestRowName);

	/** 특정 RowName의 퀘스트가 완료되었는지 확인합니다. (임시: 항상 false 반환). */
	UFUNCTION(BlueprintCallable, Category = "HowlingQuest")
	bool IsQuestComplete(FName QuestRowName) const;

	/**
	 * 게임 로직에서 행동 발생 시 호출하는 함수.
	 * - EventPayload는 FHQQuestEventPayloadBase를 상속한 구조체여야 합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "HowlingQuest")
	void NotifyQuestEvent(const FHQQuestEventPayloadBase& EventPayload);

private:
	const FHQQuestCompiledData* FindQuestData(const FName& QuestRowName) const;

	UObject* FindRequirementHandler(FGameplayTag TypeTag) const;
	UObject* FindRewardHandler(FGameplayTag TypeTag) const;
	UObject* FindObjectiveHandler(FGameplayTag TypeTag) const;

	UPROPERTY(Transient)
	TObjectPtr<UHQQuestHandlerRegistry> LoadedHandlerRegistry;

	UPROPERTY(Transient)
	TMap<FName, FHQQuestCompiledData> QuestDataByRowName;
};

