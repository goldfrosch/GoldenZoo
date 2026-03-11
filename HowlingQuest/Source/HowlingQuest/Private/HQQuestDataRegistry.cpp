#include "HQQuestDataRegistry.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/DataTable.h"
#include "HQQuestDefinitionAsset.h"
#include "HQQuestDeveloperSettings.h"
#include "HQQuestHandlerRegistry.h"
#include "Misc/PackageName.h"
#include "Modules/ModuleManager.h"

namespace
{
	TMap<FName, FHQQuestCompiledData> GCompiledQuestData;
	UHQQuestHandlerRegistry* GHandlerRegistry = nullptr;

	FString NormalizeDirectoryToPackagePath(const FString& Directory)
	{
		const FString TrimmedDirectory = Directory.TrimStartAndEnd();
		if (TrimmedDirectory.IsEmpty())
		{
			return FString();
		}

		if (FPackageName::IsValidLongPackageName(TrimmedDirectory))
		{
			return TrimmedDirectory;
		}

		if (!TrimmedDirectory.StartsWith(TEXT("/")) && !TrimmedDirectory.Contains(TEXT(":")))
		{
			return FString::Printf(TEXT("/Game/%s"), *TrimmedDirectory.TrimChar(TEXT('/')));
		}

		FString PackagePath;
		if (FPackageName::TryConvertFilenameToLongPackageName(TrimmedDirectory, PackagePath))
		{
			return PackagePath;
		}

		return FString();
	}
}

void FHQQuestDataRegistry::ReloadFromSettings()
{
	Shutdown();

	const UHQQuestDeveloperSettings* Settings = GetDefault<UHQQuestDeveloperSettings>();
	if (!Settings)
	{
		UE_LOG(LogTemp, Warning, TEXT("[HowlingQuest] Failed to load developer settings."));
		return;
	}

	UDataTable* QuestDataTable = Settings->QuestDataTable.LoadSynchronous();
	if (!QuestDataTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("[HowlingQuest] QuestDataTable is not configured."));
		return;
	}

	GHandlerRegistry = Settings->HandlerRegistry.LoadSynchronous();
	if (GHandlerRegistry)
	{
		GHandlerRegistry->AddToRoot();
	}

	TMap<FName, TObjectPtr<UHQQuestDefinitionAsset>> DefinitionAssetsByRowName;
	if (Settings->QuestDefinitionDirectories.Num() > 0)
	{
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
		IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

		for (const FString& ConfiguredDirectory : Settings->QuestDefinitionDirectories)
		{
			const FString PackagePath = NormalizeDirectoryToPackagePath(ConfiguredDirectory);
			if (PackagePath.IsEmpty())
			{
				UE_LOG(LogTemp, Warning, TEXT("[HowlingQuest] Invalid quest definition directory '%s'."), *ConfiguredDirectory);
				continue;
			}

			FARFilter Filter;
			Filter.PackagePaths.Add(*PackagePath);
			Filter.ClassPaths.Add(UHQQuestDefinitionAsset::StaticClass()->GetClassPathName());
			Filter.bRecursivePaths = true;

			TArray<FAssetData> AssetDataList;
			AssetRegistry.GetAssets(Filter, AssetDataList);

			for (const FAssetData& AssetData : AssetDataList)
			{
				const FName AssetRowName = AssetData.AssetName;
				if (DefinitionAssetsByRowName.Contains(AssetRowName))
				{
					UE_LOG(LogTemp, Error, TEXT("[HowlingQuest] Duplicate quest definition asset binding for RowName '%s'."), *AssetRowName.ToString());
					continue;
				}

				UHQQuestDefinitionAsset* DefinitionAsset = Cast<UHQQuestDefinitionAsset>(AssetData.GetAsset());
				if (!DefinitionAsset)
				{
					UE_LOG(LogTemp, Error, TEXT("[HowlingQuest] Failed to load quest definition asset '%s'."), *AssetData.GetObjectPathString());
					continue;
				}

				DefinitionAssetsByRowName.Add(AssetRowName, DefinitionAsset);
			}
		}
	}

	TSet<FName> TableRowNames;
	for (const TPair<FName, uint8*>& RowEntry : QuestDataTable->GetRowMap())
	{
		const FName QuestRowName = RowEntry.Key;
		TableRowNames.Add(QuestRowName);
	}

	for (const TPair<FName, uint8*>& RowEntry : QuestDataTable->GetRowMap())
	{
		const FName QuestRowName = RowEntry.Key;
		const FHQQuestStoryRow* StoryRow = reinterpret_cast<const FHQQuestStoryRow*>(RowEntry.Value);
		if (!StoryRow)
		{
			continue;
		}

		const TObjectPtr<UHQQuestDefinitionAsset>* DefinitionAssetPtr = DefinitionAssetsByRowName.Find(QuestRowName);
		if (!DefinitionAssetPtr || !DefinitionAssetPtr->Get())
		{
			UE_LOG(LogTemp, Error, TEXT("[HowlingQuest] Missing quest definition asset for RowName '%s'."), *QuestRowName.ToString());
			continue;
		}

		for (const FName& RequiredQuestRowName : StoryRow->RequireQuestRowNames)
		{
			if (!TableRowNames.Contains(RequiredQuestRowName))
			{
				UE_LOG(LogTemp, Error, TEXT("[HowlingQuest] Quest '%s' references missing prerequisite RowName '%s'."),
					*QuestRowName.ToString(),
					*RequiredQuestRowName.ToString());
			}
		}

		const UHQQuestDefinitionAsset* DefinitionAsset = DefinitionAssetPtr->Get();

		FHQQuestCompiledData CompiledData;
		CompiledData.QuestRowName = QuestRowName;
		CompiledData.StoryRow = *StoryRow;
		CompiledData.Requirements = DefinitionAsset->Requirements;
		CompiledData.Rewards = DefinitionAsset->Rewards;
		CompiledData.Extensions = DefinitionAsset->Extensions;
		CompiledData.MetaFields = DefinitionAsset->MetaFields;

		GCompiledQuestData.Add(QuestRowName, MoveTemp(CompiledData));
	}

	for (const TPair<FName, TObjectPtr<UHQQuestDefinitionAsset>>& DefinitionEntry : DefinitionAssetsByRowName)
	{
		if (!TableRowNames.Contains(DefinitionEntry.Key))
		{
			UE_LOG(LogTemp, Warning, TEXT("[HowlingQuest] Quest definition asset '%s' has no matching DataTable row."), *DefinitionEntry.Key.ToString());
		}
	}
}

void FHQQuestDataRegistry::Shutdown()
{
	GCompiledQuestData.Reset();

	if (GHandlerRegistry)
	{
		GHandlerRegistry->RemoveFromRoot();
		GHandlerRegistry = nullptr;
	}
}

const TMap<FName, FHQQuestCompiledData>& FHQQuestDataRegistry::GetCompiledQuestData()
{
	return GCompiledQuestData;
}

UHQQuestHandlerRegistry* FHQQuestDataRegistry::GetHandlerRegistry()
{
	return GHandlerRegistry;
}
