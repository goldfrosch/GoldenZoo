#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"

#include "BCraftingFlowStep.generated.h"

class UBCraftingSubsystem;

enum class EBCraftingFlowStepState : uint8
{
	Uninitialized,
	Initialized,
	Prepared,
	Running,
	Finished
};

UCLASS(Abstract, BlueprintType, Blueprintable)
class BEAVERCRAFTING_API UBCraftingFlowStep : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	void Initialize();
	bool Prepare();
	bool Start();
	bool Finish();

	bool Complete();
	bool CompleteAndAdvance();

	const FGameplayTag& GetFlowTag() const { return FlowTag; }
	EBCraftingFlowStepState GetState() const { return State; }
	FName GetDebugName() const { return GetFName(); }
	UBCraftingSubsystem* GetSubsystem() const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Crafting Flow")
	FGameplayTag FlowTag;

	virtual void HandleInitialize();
	virtual bool HandlePrepare();
	virtual bool HandleStart();
	virtual void HandleFinish();

private:
	EBCraftingFlowStepState State = EBCraftingFlowStepState::Uninitialized;
};
