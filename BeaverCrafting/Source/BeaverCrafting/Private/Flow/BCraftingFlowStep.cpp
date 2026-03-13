#include "Flow/BCraftingFlowStep.h"

#include "Flow/BCraftingSubsystem.h"

void UBCraftingFlowStep::Initialize()
{
	HandleInitialize();
	State = EBCraftingFlowStepState::Initialized;
}

bool UBCraftingFlowStep::Prepare()
{
	if (State != EBCraftingFlowStepState::Initialized)
	{
		return false;
	}

	if (!HandlePrepare())
	{
		return false;
	}

	State = EBCraftingFlowStepState::Prepared;
	return true;
}

bool UBCraftingFlowStep::Start()
{
	if (State != EBCraftingFlowStepState::Prepared)
	{
		return false;
	}

	if (!HandleStart())
	{
		return false;
	}

	State = EBCraftingFlowStepState::Running;
	return true;
}

bool UBCraftingFlowStep::Finish()
{
	if (State != EBCraftingFlowStepState::Running)
	{
		return false;
	}

	HandleFinish();
	State = EBCraftingFlowStepState::Finished;
	return true;
}

bool UBCraftingFlowStep::Complete()
{
	if (UBCraftingSubsystem* CraftingSubsystem = GetSubsystem())
	{
		return CraftingSubsystem->CompleteStep(*this, false);
	}

	return false;
}

bool UBCraftingFlowStep::CompleteAndAdvance()
{
	if (UBCraftingSubsystem* CraftingSubsystem = GetSubsystem())
	{
		return CraftingSubsystem->CompleteStep(*this, true);
	}

	return false;
}

UBCraftingSubsystem* UBCraftingFlowStep::GetSubsystem() const
{
	return Cast<UBCraftingSubsystem>(GetOuter());
}

void UBCraftingFlowStep::HandleInitialize()
{
}

bool UBCraftingFlowStep::HandlePrepare()
{
	return true;
}

bool UBCraftingFlowStep::HandleStart()
{
	return true;
}

void UBCraftingFlowStep::HandleFinish()
{
}
