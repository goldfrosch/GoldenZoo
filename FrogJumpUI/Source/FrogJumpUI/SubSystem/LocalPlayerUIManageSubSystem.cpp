#include "LocalPlayerUIManageSubSystem.h"

#include "Blueprint/UserWidget.h"
#include "FrogJumpUI/MVC/BaseUIController.h"
#include "FrogJumpUI/MVC/BaseUIView.h"
#include "FrogJumpUI/UIEnum.h"
#include "FrogJumpUI/Settings/FrogUISettings.h"

void ULocalPlayerUIManageSubSystem::InitializeUI()
{
	if (!IsPlayerInLocalControlled())
	{
		UE_LOG(LogTemp, Warning
				, TEXT("Player Controller가 Local Controller여야 합니다."));
		return;
	}

	const UFrogUISettings* UISettings = GetUISettings();
	if (!UISettings)
	{
		UE_LOG(LogTemp, Warning
				, TEXT("UI Settings를 찾을 수 없습니다. 프로젝트 세팅에서 UI Settings를 확인해주세요."));
		return;
	}

	// 프로젝트 세팅에서 모든 UI 키를 가져와서 레이어를 설정합니다.
	for (const FString& Key : UISettings->GetAllKeys())
	{
		if (Key.StartsWith(TEXT("Gameplay_")))
		{
			WidgetLayers.Add(Key, EUILayer::Gameplay);
		}
		else if (Key.StartsWith(TEXT("Popup_")))
		{
			WidgetLayers.Add(Key, EUILayer::Popup);
		}
		else if (Key.StartsWith(TEXT("Modal_")))
		{
			WidgetLayers.Add(Key, EUILayer::Modal);
		}
		else if (Key.StartsWith(TEXT("Loading_")))
		{
			WidgetLayers.Add(Key, EUILayer::Loading);
		}
	}
}

void ULocalPlayerUIManageSubSystem::PushWidget(const FString& Key)
{
	if (!IsPlayerInLocalControlled() || !WidgetLayers.Contains(Key))
	{
		return;
	}

	APlayerController* PC = Cast<APlayerController>(
		GetLocalPlayer()->GetPlayerController(GetWorld()));

	if (!WidgetStack.IsEmpty())
	{
		const FString& LastKey = WidgetStack.Last();
		if (WidgetLayers[Key] != EUILayer::Modal && ControllerInstances.
			Contains(LastKey))
		{
			ControllerInstances[LastKey]->EndShowUI();
		}
	}

	// 동일한 Key가 들어가 있으면 순서를 앞으로 이동시킨다
	// UI는 반드시 Key 하나당 하나의 UI로 바인딩 된다는 비즈니스
	// 규칙을 명시한다. [25.04.24]
	// if (WidgetStack.Find(Key) != INDEX_NONE)
	// {
	// 	WidgetStack.Remove(Key);
	// }

	WidgetStack.Add(Key);

	if (!ControllerInstances.Contains(Key))
	{
		const TSubclassOf<UBaseUIView> ViewClass = GetUIViewClassFromSettings(Key);
		if (!ViewClass)
		{
			UE_LOG(LogTemp, Error, TEXT("UI 클래스를 찾을 수 없습니다: %s"), *Key);
			return;
		}

		UBaseUIView* NewView = CreateWidget<UBaseUIView>(PC, ViewClass);
		if (!NewView)
		{
			UE_LOG(LogTemp, Error, TEXT("UI 인스턴스를 생성할 수 없습니다: %s"), *Key);
			return;
		}

		NewView->AddToViewport(static_cast<int32>(WidgetLayers[Key]));
		ControllerInstances.Add(Key, NewView->GetController());
	}

	ControllerInstances[Key]->StartShowUI(WidgetLayers[Key]);
	ControllerInstances[Key]->OnPushUI();

	PrintAllWidgetStackToDebug();
}

void ULocalPlayerUIManageSubSystem::PopWidget()
{
	if (!IsPlayerInLocalControlled() || WidgetStack.IsEmpty())
	{
		return;
	}

	const FString LastKey = WidgetStack.Last();
	if (ControllerInstances.Contains(LastKey))
	{
		ControllerInstances[LastKey]->EndShowUI();
	}

	WidgetStack.Pop();

	// 여기서부터는 제거된 Stack을 기반으로 동작함
	if (WidgetStack.IsEmpty())
	{
		return;
	}

	// 제거된 이후 남은 Key 기반으로 UI 노출 처리
	const FString PrevKey = WidgetStack.Last();
	if (ControllerInstances.Contains(PrevKey))
	{
		ControllerInstances[PrevKey]->StartShowUI(WidgetLayers[PrevKey]);
	}

	APlayerController* PC = Cast<APlayerController>(
		GetLocalPlayer()->GetPlayerController(GetWorld()));
	if (WidgetLayers[LastKey] != EUILayer::Gameplay && WidgetLayers[PrevKey] ==
		EUILayer::Gameplay)
	{
		PC->SetShowMouseCursor(false);
	}

	PrintAllWidgetStackToDebug();
}

bool ULocalPlayerUIManageSubSystem::IsPlayerInLocalControlled() const
{
	const APlayerController* PC = Cast<APlayerController>(
		GetLocalPlayer()->GetPlayerController(GetWorld()));
	return PC && PC->IsLocalController();
}

FString ULocalPlayerUIManageSubSystem::GetTopStackUI() const
{
	return WidgetStack.Last();
}

TSubclassOf<UBaseUIView> ULocalPlayerUIManageSubSystem::GetViewClasses(
	const FString& Key)
{
	return GetUIViewClassFromSettings(Key);
}

bool ULocalPlayerUIManageSubSystem::HasViewUI(const FString& Key) const
{
	return WidgetStack.Find(Key) != INDEX_NONE;
}

void ULocalPlayerUIManageSubSystem::ResetWidget()
{
	while (!WidgetStack.IsEmpty())
	{
		PopWidget();
	}
}

UBaseUIView* ULocalPlayerUIManageSubSystem::CreateUIInstance(const FString& Key) const
{
	APlayerController* PC = Cast<APlayerController>(
		GetLocalPlayer()->GetPlayerController(GetWorld()));

	if (!PC)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerController를 찾을 수 없습니다."));
		return nullptr;
	}

	TSubclassOf<UBaseUIView> ViewClass = GetUIViewClassFromSettings(Key);
	if (!ViewClass)
	{
		UE_LOG(LogTemp, Error, TEXT("UI 클래스를 찾을 수 없습니다: %s"), *Key);
		return nullptr;
	}

	UBaseUIView* NewView = CreateWidget<UBaseUIView>(PC, ViewClass);
	if (!NewView)
	{
		UE_LOG(LogTemp, Error, TEXT("UI 인스턴스를 생성할 수 없습니다: %s"), *Key);
		return nullptr;
	}

	return NewView;
}

const UFrogUISettings* ULocalPlayerUIManageSubSystem::GetUISettings()
{
	UFrogUISettings* const Settings = GetMutableDefault<UFrogUISettings>();
	Settings->LoadConfig();
	return Settings;
}

TSubclassOf<UBaseUIView> ULocalPlayerUIManageSubSystem::GetUIViewClassFromSettings(
	const FString& Key)
{
	const UFrogUISettings* UISettings = GetUISettings();
	if (!UISettings)
	{
		return nullptr;
	}

	return UISettings->GetUIViewClassByKey(Key);
}

void ULocalPlayerUIManageSubSystem::PrintAllWidgetStackToDebug()
{
	FString StackText;
	for (int i = 0; i < WidgetStack.Num(); i++)
	{
		StackText += WidgetStack[i];

		if (i != WidgetStack.Num() - 1)
		{
			StackText += TEXT(",");
		}
	}

	UE_LOG(LogTemp, Display, TEXT("Current Widget Stack: [%s]"), *StackText);
}
