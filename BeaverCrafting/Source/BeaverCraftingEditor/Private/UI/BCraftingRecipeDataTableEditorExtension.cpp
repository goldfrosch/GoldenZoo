#include "UI/BCraftingRecipeDataTableEditorExtension.h"

#include "Data/BCraftingRecipeTypes.h"
#include "DataTableEditorModule.h"
#include "Engine/DataTable.h"
#include "Framework/Commands/UIAction.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Framework/MultiBox/MultiBoxExtender.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Modules/ModuleManager.h"
#include "Search/BCraftingRecipeIndex.h"
#include "Styling/AppStyle.h"
#include "UI/BCraftingRecipeIndexVisualizerTab.h"
#include "Widgets/Notifications/SNotificationList.h"

#define LOCTEXT_NAMESPACE "BeaverCraftingRecipeDataTableEditorExtension"

FBCraftingRecipeDataTableEditorExtension::FBCraftingRecipeDataTableEditorExtension(FBCraftingRecipeIndexVisualizerTab& InVisualizerTab)
	: VisualizerTab(InVisualizerTab)
{
}

void FBCraftingRecipeDataTableEditorExtension::Register()
{
	FDataTableEditorModule& DataTableEditorModule = FModuleManager::LoadModuleChecked<FDataTableEditorModule>("DataTableEditor");

	const FAssetEditorExtender ExtenderDelegate = FAssetEditorExtender::CreateRaw(this, 
		&FBCraftingRecipeDataTableEditorExtension::ExtendRecipeDataTableToolbar);
	DataTableToolbarExtenderHandle = ExtenderDelegate.GetHandle();
	DataTableEditorModule.GetToolBarExtensibilityManager()->GetExtenderDelegates().Add(ExtenderDelegate);
}

void FBCraftingRecipeDataTableEditorExtension::Unregister()
{
	if (!DataTableToolbarExtenderHandle.IsValid())
	{
		return;
	}

	if (FDataTableEditorModule* DataTableEditorModule = FModuleManager::GetModulePtr<FDataTableEditorModule>("DataTableEditor"))
	{
		TArray<FAssetEditorExtender>& ExtenderDelegates = DataTableEditorModule->GetToolBarExtensibilityManager()->GetExtenderDelegates();
		ExtenderDelegates.RemoveAll([this](const FAssetEditorExtender& Delegate)
		{
			return Delegate.GetHandle() == DataTableToolbarExtenderHandle;
		});
	}

	DataTableToolbarExtenderHandle.Reset();
}

TSharedRef<FExtender> FBCraftingRecipeDataTableEditorExtension::ExtendRecipeDataTableToolbar(const TSharedRef<FUICommandList> CommandList, const TArray<UObject*> ContextObjects)
{
	TSharedRef<FExtender> Extender = MakeShared<FExtender>();

	if (UDataTable* RecipeTable = FindRecipeDataTable(ContextObjects))
	{
		Extender->AddToolBarExtension(
			TEXT("DataTableCommands"),
			EExtensionHook::After,
			CommandList,
			FToolBarExtensionDelegate::CreateRaw(this, &FBCraftingRecipeDataTableEditorExtension::AddRecipeDataTableToolbar, TWeakObjectPtr(RecipeTable)));
	}

	return Extender;
}

void FBCraftingRecipeDataTableEditorExtension::AddRecipeDataTableToolbar(FToolBarBuilder& ToolbarBuilder, TWeakObjectPtr<UDataTable> RecipeTable)
{
	ToolbarBuilder.AddSeparator();
	ToolbarBuilder.AddToolBarButton(
		FUIAction(FExecuteAction::CreateRaw(this, &FBCraftingRecipeDataTableEditorExtension::HandleRecipeToolbarButtonClicked, RecipeTable)),
		NAME_None,
		LOCTEXT("RecipeIndexTestLabel", "Refresh Recipe Tree"),
		LOCTEXT("RecipeIndexTestTooltip", 
			"Refresh data and regenerate tree struct"),
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Refresh"));
}

void FBCraftingRecipeDataTableEditorExtension::HandleRecipeToolbarButtonClicked(TWeakObjectPtr<UDataTable> RecipeTable) const
{
	if (!RecipeTable.IsValid())
	{
		FNotificationInfo NotificationInfo(LOCTEXT("MissingRecipeTable", "BeaverCrafting DataTable을 찾지 못했습니다."));
		NotificationInfo.ExpireDuration = 3.0f;
		FSlateNotificationManager::Get().AddNotification(NotificationInfo);
		return;
	}

	FBCraftingRecipeIndex RecipeIndex;
	const bool bBuilt = FBCraftingRecipeIndexBuilder::BuildFromDataTable(RecipeTable.Get(), RecipeIndex);
	VisualizerTab.ShowRecipeIndex(RecipeTable.Get(), RecipeIndex);

	const FText Message = FText::Format(
		LOCTEXT("RecipeIndexBuilt", "Recipe Table: {0}\nRows: {1}, Indexed: {2}"),
		FText::FromName(RecipeTable->GetFName()),
		FText::AsNumber(RecipeTable->GetRowMap().Num()),
		FText::AsNumber(bBuilt ? RecipeIndex.GetRecipeCount() : 0));

	FNotificationInfo NotificationInfo(Message);
	NotificationInfo.ExpireDuration = 4.0f;
	FSlateNotificationManager::Get().AddNotification(NotificationInfo);
}

UDataTable* FBCraftingRecipeDataTableEditorExtension::FindRecipeDataTable(const TArray<UObject*>& ContextObjects)
{
	for (UObject* ContextObject : ContextObjects)
	{
		UDataTable* DataTable = Cast<UDataTable>(ContextObject);
		if (!DataTable)
		{
			continue;
		}
		
		if (DataTable->GetRowStruct() == FBCraftingRecipeRow::StaticStruct())
		{
			return DataTable;
		}
	}

	return nullptr;
}

#undef LOCTEXT_NAMESPACE
