#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "LocalPlayerUIManageSubSystem.generated.h"

enum class EUILayer : uint8;
class UBaseUIView;
class UBaseUIController;
class UFrogUISettings;

UCLASS()
class FROGJUMPUI_API ULocalPlayerUIManageSubSystem
	: public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	/** UI 서브시스템 초기화 (기본 Initialize와 구분) */
	UFUNCTION(BlueprintCallable)
	void InitializeUI();
	
	const TArray<FString>& GetWidgetStack() const { return WidgetStack; }

	UFUNCTION(BlueprintCallable)
	void PushWidget(const FString& Key);
	UFUNCTION(BlueprintCallable)
	void PopWidget();
	FString GetTopStackUI() const;
	static TSubclassOf<UBaseUIView> GetViewClasses(const FString& Key);

	bool HasViewUI(const FString& Key) const;

	void ResetWidget();

	/**
	 * Key를 사용하여 새로운 UI 인스턴스를 생성합니다.
	 * 프로젝트 세팅에서 UI 클래스를 찾아 인스턴스를 생성합니다.
	 * @param Key UI를 식별하는 키
	 * @return 생성된 UI 인스턴스 (유효하지 않은 경우 nullptr)
	 */
	UBaseUIView* CreateUIInstance(const FString& Key) const;

	UPROPERTY()
	TMap<FString, TObjectPtr<UBaseUIController>> ControllerInstances;

private:
	UPROPERTY()
	TMap<FString, EUILayer> WidgetLayers;

	UPROPERTY()
	TArray<FString> WidgetStack;

	/** 프로젝트 세팅에서 UI 클래스를 가져옵니다. */
	static const UFrogUISettings* GetUISettings();

	/** 프로젝트 세팅에서 UI 클래스를 가져옵니다. */
	static TSubclassOf<UBaseUIView> GetUIViewClassFromSettings(const FString& Key);

	bool IsPlayerInLocalControlled() const;

	void PrintAllWidgetStackToDebug();
};
