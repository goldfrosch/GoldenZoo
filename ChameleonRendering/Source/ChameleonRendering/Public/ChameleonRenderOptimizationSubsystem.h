#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "ChameleonRenderOptimizationSubsystem.generated.h"

class UChameleonRenderOptimizationComponent;
class AActor;

USTRUCT()
struct FRenderObject
{
	GENERATED_BODY()

	FRenderObject() = default;
	explicit FRenderObject(UChameleonRenderOptimizationComponent* InActor)
		: RenderControlComponent(InActor)
	{
		RenderControlComponent = InActor;
	}
	
	TWeakObjectPtr<UChameleonRenderOptimizationComponent> GetRenderControlComponent() const { return RenderControlComponent; }
	
private:
	UPROPERTY()
	TWeakObjectPtr<UChameleonRenderOptimizationComponent> RenderControlComponent;
};

UCLASS()
class CHAMELEONRENDERING_API UChameleonRenderOptimizationSubsystem
	: public ULocalPlayerSubsystem
{
	GENERATED_BODY()
public:
	void InitPosition(const FVector& CurrentPos);
	
	void CheckAndRenderOptimization(const FVector& CurrentPos);
	
protected:
	virtual void RenderOptimization();
	
	virtual void TraceObject(const FVector& CurrentPos);
	
private:
	TMap<FVector, TArray<FRenderObject>> RenderObjects;
	
	UPROPERTY(meta = (UIMin= "0", UIMax="100"))
	uint8 CompareMeter = 5;
	
	UPROPERTY()
	FVector PrevLocation;
	
	UPROPERTY()
	FVector CurLocation;
	
	UPROPERTY(meta = (UIMin= "1", UIMax="10000"))
	uint16 RenderMeter = 500;
	
	FVector GetChunkVector(const FVector& Pos) const;
	
	double GetNearestChunkAxis(const double AxisValue) const;
};
