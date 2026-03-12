#include "ChameleonRenderOptimizationSubsystem.h"

#include "CollisionQueryParams.h"
#include "CollisionShape.h"
#include "ChameleonRendering/ChameleonRenderOptimizationComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

void UChameleonRenderOptimizationSubsystem::InitPosition(
	const FVector& CurrentPos)
{
	PrevLocation = CurrentPos;
	CurLocation = CurrentPos;
}

void UChameleonRenderOptimizationSubsystem::CheckAndRenderOptimization(
	const FVector& CurrentPos)
{
	// 현재 위치 정보를 저장
	CurLocation = CurrentPos;
	RenderOptimization();
	// 이전 위치 정보에 계산하고 끝난 현재 위치 정보를 저장
	PrevLocation = CurrentPos;
}

void UChameleonRenderOptimizationSubsystem::RenderOptimization()
{
	TraceObject(CurLocation);
}

void UChameleonRenderOptimizationSubsystem::TraceObject(const FVector& CurrentPos)
{
	RenderObjects.Reset();

	const UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

	TArray<FHitResult> HitResults;
	const FCollisionShape SphereShape = FCollisionShape::MakeSphere(RenderMeter);
	const FCollisionObjectQueryParams ObjectQueryParams = FCollisionObjectQueryParams::AllObjects;
	const FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(LocalPlayerRenderOptimizationTrace), false);

	const bool bHasHit = World->SweepMultiByObjectType(
		HitResults,
		CurrentPos,
		CurrentPos,
		FQuat::Identity,
		ObjectQueryParams,
		SphereShape,
		QueryParams);

	if (!bHasHit)
	{
		return;
	}

	for (const FHitResult& HitResult : HitResults)
	{
		const AActor* HitActor = HitResult.GetActor();
		if (!HitActor->IsValidLowLevel())
		{
			continue;
		}
		
		UChameleonRenderOptimizationComponent* RenderOptimizationComponent = 
			HitActor->FindComponentByClass<UChameleonRenderOptimizationComponent>();
		if (!RenderOptimizationComponent->IsValidLowLevel())
		{
			return;
		}
		
		const FVector ChunkVector = GetChunkVector(HitActor->GetActorLocation());
		if (!RenderObjects.Contains(ChunkVector))
		{
			RenderObjects.Add(ChunkVector, TArray<FRenderObject>());
		}
		
		RenderObjects[ChunkVector].Add(FRenderObject(RenderOptimizationComponent));
	}
}

FVector UChameleonRenderOptimizationSubsystem::GetChunkVector(
	const FVector& Pos) const
{
	return FVector(
		GetNearestChunkAxis(Pos.X),
		GetNearestChunkAxis(Pos.Y),
		GetNearestChunkAxis(Pos.Z));
}

double UChameleonRenderOptimizationSubsystem::GetNearestChunkAxis(
	const double AxisValue) const
{
	const uint16 ChunkDiameter = FMath::Max(1, CompareMeter * 2);
	const int32 ChunkIndex = FMath::RoundToInt(static_cast<float>(AxisValue / static_cast<double>(ChunkDiameter)));
	return ChunkIndex * ChunkDiameter;
}
