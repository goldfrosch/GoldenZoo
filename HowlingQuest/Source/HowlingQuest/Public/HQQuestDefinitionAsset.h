#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "HQQuestPayload.h"
#include "HQQuestTypes.h"
#include "HQQuestDefinitionAsset.generated.h"

/**
 * 퀘스트별 상세 정의를 담는 DataAsset.
 * 에셋 이름이 DataTable RowName과 동일해야 바인딩됩니다.
 */
UCLASS(BlueprintType)
class HOWLINGQUEST_API UHQQuestDefinitionAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HowlingQuest|Definition")
	TArray<FHQQuestTaggedPayload> Requirements;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HowlingQuest|Definition")
	TArray<FHQQuestTaggedPayload> Rewards;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HowlingQuest|Definition")
	TArray<FHQQuestTaggedPayload> Extensions;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HowlingQuest|Definition")
	TArray<FHQQuestMetaField> MetaFields;
};
