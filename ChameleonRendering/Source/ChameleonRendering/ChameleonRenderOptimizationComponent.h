#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ChameleonRenderOptimizationComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CHAMELEONRENDERING_API UChameleonRenderOptimizationComponent
	: public UActorComponent
{
	GENERATED_BODY()

public:
	UChameleonRenderOptimizationComponent();

protected:
	virtual void BeginPlay() override;
};
