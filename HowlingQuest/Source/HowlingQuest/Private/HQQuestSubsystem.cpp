#include "HQQuestSubsystem.h"

#include "HQQuestDataRegistry.h"
#include "HQQuestHandlerInterfaces.h"
#include "Subsystems/SubsystemCollection.h"

void UHQQuestSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	FHQQuestDataRegistry::ReloadFromSettings();

	QuestDataByRowName = FHQQuestDataRegistry::GetCompiledQuestData();
	LoadedHandlerRegistry = FHQQuestDataRegistry::GetHandlerRegistry();
}

void UHQQuestSubsystem::Deinitialize()
{
	QuestDataByRowName.Reset();
	LoadedHandlerRegistry = nullptr;

	Super::Deinitialize();
}

const FHQQuestCompiledData* UHQQuestSubsystem::FindQuestData(const FName& QuestRowName) const
{
	return QuestDataByRowName.Find(QuestRowName);
}

UObject* UHQQuestSubsystem::FindRequirementHandler(FGameplayTag TypeTag) const
{
	if (!LoadedHandlerRegistry)
	{
		return nullptr;
	}

	for (const FHQQuestHandlerBinding& Binding : LoadedHandlerRegistry->RequirementHandlers)
	{
		if (Binding.TypeTag != TypeTag)
		{
			continue;
		}

		UClass* HandlerClass = Binding.HandlerClass.LoadSynchronous();
		if (HandlerClass)
		{
			return HandlerClass->GetDefaultObject();
		}
	}

	return nullptr;
}

UObject* UHQQuestSubsystem::FindRewardHandler(FGameplayTag TypeTag) const
{
	if (!LoadedHandlerRegistry)
	{
		return nullptr;
	}

	for (const FHQQuestHandlerBinding& Binding : LoadedHandlerRegistry->RewardHandlers)
	{
		if (Binding.TypeTag != TypeTag)
		{
			continue;
		}

		UClass* HandlerClass = Binding.HandlerClass.LoadSynchronous();
		if (HandlerClass)
		{
			return HandlerClass->GetDefaultObject();
		}
	}

	return nullptr;
}

UObject* UHQQuestSubsystem::FindObjectiveHandler(FGameplayTag TypeTag) const
{
	if (!LoadedHandlerRegistry)
	{
		return nullptr;
	}

	for (const FHQQuestHandlerBinding& Binding : LoadedHandlerRegistry->ObjectiveHandlers)
	{
		if (Binding.TypeTag != TypeTag)
		{
			continue;
		}

		UClass* HandlerClass = Binding.HandlerClass.LoadSynchronous();
		if (HandlerClass)
		{
			return HandlerClass->GetDefaultObject();
		}
	}

	return nullptr;
}

void UHQQuestSubsystem::StartQuest(FName QuestRowName)
{
	const FHQQuestCompiledData* QuestData = FindQuestData(QuestRowName);
	if (!QuestData)
	{
		return;
	}

	for (const FHQQuestTaggedPayload& Requirement : QuestData->Requirements)
	{
		UObject* RequirementHandler = FindRequirementHandler(Requirement.TypeTag);
		if (!RequirementHandler)
		{
			continue;
		}

		if (RequirementHandler->GetClass()->ImplementsInterface(UHQQuestRequirementHandler::StaticClass()) &&
			!IHQQuestRequirementHandler::Execute_IsSatisfied(RequirementHandler, Requirement))
		{
			return;
		}
	}

	// TODO: 활성 퀘스트 목록에 추가하고 Objective 상태를 초기화하는 로직을 구현합니다.
}

bool UHQQuestSubsystem::IsQuestComplete(FName QuestRowName) const
{
	const FHQQuestCompiledData* QuestData = FindQuestData(QuestRowName);
	if (!QuestData)
	{
		return false;
	}

	// TODO: 활성 퀘스트 상태와 ObjectiveHandler 기반으로 완료 여부를 판단하도록 확장합니다.
	return false;
}

void UHQQuestSubsystem::NotifyQuestEvent(const FHQQuestEventPayloadBase& EventPayload)
{
	// TODO:
	// - 활성 퀘스트 목록을 순회
	// - 각 퀘스트의 Objectives(FHQQuestTaggedPayload)를 순회
	// - ObjectiveHandler를 찾아 OnEvent를 호출
}
