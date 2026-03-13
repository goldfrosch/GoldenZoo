#pragma once

#include "CoreMinimal.h"
#include "Flow/BCraftingFlowStep.h"
#include "GameplayTagContainer.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "Tickable.h"

#include "BCraftingSubsystem.generated.h"

USTRUCT()
struct FBCraftingFlowStepPool
{
	GENERATED_BODY()

	UPROPERTY(Transient)
	TArray<TObjectPtr<UBCraftingFlowStep>> Steps;
};

UCLASS(Config = Game, DefaultConfig)
class BEAVERCRAFTING_API UBCraftingSubsystem : public ULocalPlayerSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override;
	virtual TStatId GetStatId() const override;

	UBCraftingFlowStep* EnqueueStep(TSubclassOf<UBCraftingFlowStep> StepClass, FName StepName = NAME_None);
	bool EnqueueStepInstance(UBCraftingFlowStep* StepInstance);
	UBCraftingFlowStep* EnqueuePooledStep(const FGameplayTag& StepTag, int32 StepIndex = 0);

	template <typename TStep>
	TStep* EnqueueStep(FName StepName = NAME_None)
	{
		static_assert(TIsDerivedFrom<TStep, UBCraftingFlowStep>::IsDerived, "TStep must derive from UBCraftingFlowStep.");
		return Cast<TStep>(EnqueueStep(TStep::StaticClass(), StepName));
	}

	bool StartNextStep();
	bool CompleteStep(UBCraftingFlowStep& Step, bool bStartNextStep);
	bool CompleteActiveStep(bool bStartNextStep = true);

	void RequestQueuedStepInitialization();
	void InitializeQueuedSteps();
	void ClearSteps();
	void RebuildStepPool();

	UBCraftingFlowStep* GetActiveStep() const;
	int32 GetQueuedStepCount() const { return QueuedSteps.Num(); }
	int32 GetPooledStepCount(const FGameplayTag& StepTag) const;

private:
	bool EnqueueManagedStep(UBCraftingFlowStep* Step);
	bool TryStartStep(UBCraftingFlowStep& Step);
	const FBCraftingFlowStepPool* FindStepPool(const FGameplayTag& StepTag) const;

	UPROPERTY(Config, EditAnywhere, Category = "Crafting", meta = (ClampMin = "0.0"))
	float QueuedInitializationInterval = 1.0f;

	UPROPERTY(Config, EditAnywhere, Category = "Crafting")
	bool bInitializeStepWhenEnqueued = true;

	UPROPERTY(Config, EditAnywhere, Category = "Crafting")
	bool bAutoStartNextStep = false;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UBCraftingFlowStep>> QueuedSteps;

	UPROPERTY(Transient)
	TObjectPtr<UBCraftingFlowStep> ActiveStep;

	UPROPERTY(Transient)
	TMap<FGameplayTag, FBCraftingFlowStepPool> StepPools;

	float TimeSinceLastQueuedInitialization = 0.0f;
	bool bQueuedInitializationRequested = false;
};
