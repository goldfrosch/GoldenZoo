#include "Flow/BCraftingSubsystem.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "Flow/BCraftingFlowStep.h"
#include "Misc/ConfigCacheIni.h"
#include "Modules/ModuleManager.h"

namespace
{
	const TCHAR* CraftingEditorSettingsSection = TEXT("/Script/BeaverCraftingEditor.BCraftingEditorSettings");
	const TCHAR* CraftingFlowSearchRootKey = TEXT("CraftingFlowSearchRoot");

	FString GetConfiguredCraftingFlowSearchRoot()
	{
#if WITH_EDITOR
		const FString ConfigFilename = GEditorPerProjectIni;
#else
		const FString ConfigFilename = GGameIni;
#endif

		FString SearchRoot;
		GConfig->GetString(CraftingEditorSettingsSection, CraftingFlowSearchRootKey, SearchRoot, ConfigFilename);
		return SearchRoot;
	}
}

void UBCraftingSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	QueuedSteps.Reset();
	ActiveStep = nullptr;
	StepPools.Reset();
	TimeSinceLastQueuedInitialization = 0.0f;
	bQueuedInitializationRequested = true;
	RebuildStepPool();
}

void UBCraftingSubsystem::Deinitialize()
{
	ClearSteps();
	StepPools.Reset();
	Super::Deinitialize();
}

void UBCraftingSubsystem::Tick(const float DeltaTime)
{
	if (bQueuedInitializationRequested)
	{
		InitializeQueuedSteps();
		TimeSinceLastQueuedInitialization = 0.0f;
		bQueuedInitializationRequested = false;
		return;
	}

	if (QueuedInitializationInterval <= 0.0f)
	{
		return;
	}

	TimeSinceLastQueuedInitialization += DeltaTime;
	if (TimeSinceLastQueuedInitialization < QueuedInitializationInterval)
	{
		return;
	}

	InitializeQueuedSteps();
	TimeSinceLastQueuedInitialization = 0.0f;
}

bool UBCraftingSubsystem::IsTickable() const
{
	return !HasAnyFlags(RF_ClassDefaultObject);
}

TStatId UBCraftingSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UBCraftingSubsystem, STATGROUP_Tickables);
}

UBCraftingFlowStep* UBCraftingSubsystem::EnqueueStep(TSubclassOf<UBCraftingFlowStep> StepClass, const FName StepName)
{
	if (!StepClass)
	{
		return nullptr;
	}

	const FName ObjectName = StepName.IsNone() ? MakeUniqueObjectName(this, StepClass) : StepName;
	UBCraftingFlowStep* Step = NewObject<UBCraftingFlowStep>(this, StepClass, ObjectName);
	if (!EnqueueManagedStep(Step))
	{
		return nullptr;
	}

	return Step;
}

UBCraftingFlowStep* UBCraftingSubsystem::EnqueuePooledStep(const FGameplayTag& StepTag, const int32 StepIndex)
{
	const FBCraftingFlowStepPool* StepPool = FindStepPool(StepTag);
	if (!StepPool || !StepPool->Steps.IsValidIndex(StepIndex))
	{
		return nullptr;
	}

	UBCraftingFlowStep* TemplateStep = StepPool->Steps[StepIndex];
	if (!TemplateStep)
	{
		return nullptr;
	}

	const FName ObjectName = MakeUniqueObjectName(this, TemplateStep->GetClass(), TemplateStep->GetFName());
	UBCraftingFlowStep* InstanceStep = DuplicateObject<UBCraftingFlowStep>(TemplateStep, this, ObjectName);
	if (!EnqueueManagedStep(InstanceStep))
	{
		return nullptr;
	}

	return InstanceStep;
}

bool UBCraftingSubsystem::EnqueueStepInstance(UBCraftingFlowStep* StepInstance)
{
	if (!StepInstance)
	{
		return false;
	}

	UBCraftingFlowStep* ManagedStep = StepInstance;
	if (StepInstance->GetOuter() != this)
	{
		ManagedStep = DuplicateObject<UBCraftingFlowStep>(StepInstance, this, StepInstance->GetFName());
	}

	return EnqueueManagedStep(ManagedStep);
}

bool UBCraftingSubsystem::EnqueueManagedStep(UBCraftingFlowStep* Step)
{
	if (!Step)
	{
		return false;
	}

	if (bInitializeStepWhenEnqueued)
	{
		Step->Initialize();
	}

	QueuedSteps.Add(Step);
	RequestQueuedStepInitialization();

	if (bAutoStartNextStep && !ActiveStep)
	{
		StartNextStep();
	}

	return true;
}

bool UBCraftingSubsystem::StartNextStep()
{
	if (ActiveStep || QueuedSteps.IsEmpty())
	{
		return false;
	}

	UBCraftingFlowStep* NextStep = QueuedSteps[0];
	QueuedSteps.RemoveAt(0);

	if (!NextStep || !TryStartStep(*NextStep))
	{
		QueuedSteps.Insert(NextStep, 0);
		return false;
	}

	ActiveStep = NextStep;
	return true;
}

bool UBCraftingSubsystem::CompleteStep(UBCraftingFlowStep& Step, const bool bStartNextStep)
{
	if (!ActiveStep || ActiveStep != &Step)
	{
		return false;
	}

	return CompleteActiveStep(bStartNextStep);
}

bool UBCraftingSubsystem::CompleteActiveStep(const bool bStartNextStep)
{
	if (!ActiveStep)
	{
		return false;
	}

	if (!ActiveStep->Finish())
	{
		return false;
	}

	ActiveStep = nullptr;

	if (bStartNextStep)
	{
		StartNextStep();
	}

	return true;
}

void UBCraftingSubsystem::RequestQueuedStepInitialization()
{
	bQueuedInitializationRequested = true;
}

void UBCraftingSubsystem::InitializeQueuedSteps()
{
	for (UBCraftingFlowStep* Step : QueuedSteps)
	{
		if (Step)
		{
			Step->Initialize();
		}
	}
}

void UBCraftingSubsystem::ClearSteps()
{
	QueuedSteps.Reset();
	ActiveStep = nullptr;
	TimeSinceLastQueuedInitialization = 0.0f;
	bQueuedInitializationRequested = false;
}

UBCraftingFlowStep* UBCraftingSubsystem::GetActiveStep() const
{
	return ActiveStep;
}

void UBCraftingSubsystem::RebuildStepPool()
{
	StepPools.Reset();

	const FString SearchRoot = GetConfiguredCraftingFlowSearchRoot();
	if (SearchRoot.IsEmpty())
	{
		return;
	}

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

	FARFilter Filter;
	Filter.PackagePaths.Add(*SearchRoot);
	Filter.ClassPaths.Add(UBCraftingFlowStep::StaticClass()->GetClassPathName());
	Filter.bRecursivePaths = true;
	Filter.bRecursiveClasses = true;

	TArray<FAssetData> AssetDataList;
	AssetRegistryModule.Get().GetAssets(Filter, AssetDataList);

	for (const FAssetData& AssetData : AssetDataList)
	{
		UBCraftingFlowStep* StepTemplate = Cast<UBCraftingFlowStep>(AssetData.GetAsset());
		if (!StepTemplate)
		{
			continue;
		}

		const FGameplayTag& StepTag = StepTemplate->GetFlowTag();
		if (!StepTag.IsValid())
		{
			continue;
		}

		FBCraftingFlowStepPool& StepPool = StepPools.FindOrAdd(StepTag);
		StepPool.Steps.Add(StepTemplate);
	}
}

int32 UBCraftingSubsystem::GetPooledStepCount(const FGameplayTag& StepTag) const
{
	const FBCraftingFlowStepPool* StepPool = FindStepPool(StepTag);
	return StepPool ? StepPool->Steps.Num() : 0;
}

const FBCraftingFlowStepPool* UBCraftingSubsystem::FindStepPool(const FGameplayTag& StepTag) const
{
	if (!StepTag.IsValid())
	{
		return nullptr;
	}

	return StepPools.Find(StepTag);
}

bool UBCraftingSubsystem::TryStartStep(UBCraftingFlowStep& Step)
{
	if (Step.GetState() == EBCraftingFlowStepState::Uninitialized)
	{
		Step.Initialize();
	}

	return Step.Prepare() && Step.Start();
}
