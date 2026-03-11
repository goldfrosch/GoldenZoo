#pragma once

#include "Blueprint/UserWidget.h"

#include "BaseUIView.generated.h"

class UBaseUIModel;
class UBaseUIController;

UCLASS(Abstract, Blueprintable)
class FROGJUMPUI_API UBaseUIView : public UUserWidget
{
	GENERATED_BODY()

public:
	TObjectPtr<UWidgetAnimation> GetDefaultTickAnimation() const { return DefaultTickAnimation; }
	TObjectPtr<UWidgetAnimation> GetDefaultStartAnimation() const { return DefaultStartAnimation; }
	TObjectPtr<UWidgetAnimation> GetDefaultEndAnimation() const { return DefaultEndAnimation; }
	
	template<typename T = UBaseUIController>
	TObjectPtr<T> GetController() const { return Controller ? Cast<T>(Controller) : nullptr; }

	FWidgetAnimationDynamicEvent TickAnimationEndNotified;
	FWidgetAnimationDynamicEvent StartAnimationFinishNotified;
	FWidgetAnimationDynamicEvent EndAnimationFinishNotified;

protected:
	virtual void NativeConstruct() override;

	template<typename T = UBaseUIModel>
	TObjectPtr<T> GetModel() const { return Model ? Cast<T>(Model) : nullptr; }

private:
	UPROPERTY(Transient, meta=(BindWidgetAnimOptional))
	TObjectPtr<UWidgetAnimation> DefaultTickAnimation;

	UPROPERTY(Transient, meta=(BindWidgetAnimOptional))
	TObjectPtr<UWidgetAnimation> DefaultStartAnimation;

	UPROPERTY(Transient, meta=(BindWidgetAnimOptional))
	TObjectPtr<UWidgetAnimation> DefaultEndAnimation;

	UPROPERTY(EditDefaultsOnly, Category = "Options"
		, meta=(AllowPrivateAccess = true))
	TSubclassOf<UBaseUIModel> ModelClass;

	UPROPERTY(EditDefaultsOnly, Category = "Options"
		, meta=(AllowPrivateAccess = true))
	TSubclassOf<UBaseUIController> ControllerClass;

	UPROPERTY()
	TObjectPtr<UBaseUIModel> Model;

	UPROPERTY()
	TObjectPtr<UBaseUIController> Controller;
};
