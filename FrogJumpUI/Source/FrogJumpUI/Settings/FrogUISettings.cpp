#include "FrogUISettings.h"
#include "FrogJumpUI/MVC/BaseUIView.h"

UFrogUISettings::UFrogUISettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool UFrogUISettings::HasUIKey(const FString& Key) const
{
	return UIViewClasses.Contains(Key);
}

TArray<FString> UFrogUISettings::GetAllKeys() const
{
	TArray<FString> Keys;
	UIViewClasses.GetKeys(Keys);

	return Keys;
}

TSubclassOf<UBaseUIView> UFrogUISettings::GetUIViewClassByKey(
	const FString& Key) const
{
	const TSubclassOf<UBaseUIView>* FoundClass = UIViewClasses.Find(Key);
	if (!FoundClass || !*FoundClass)
	{
		return nullptr;
	}

	return *FoundClass;
}