#include "HowlingQuest.h"
#include "HQQuestDataRegistry.h"

#define LOCTEXT_NAMESPACE "FHowlingQuestModule"

void FHowlingQuestModule::StartupModule()
{
	FHQQuestDataRegistry::ReloadFromSettings();
}

void FHowlingQuestModule::ShutdownModule()
{
	FHQQuestDataRegistry::Shutdown();
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FHowlingQuestModule, HowlingQuest)

