#pragma once

#include "CoreMinimal.h"
#include "HQQuestCompiledData.h"

class UHQQuestHandlerRegistry;

/**
 * 에디터/게임 시작 시 설정 기반으로 퀘스트 정적 데이터를 로드하는 내부 레지스트리입니다.
 */
class FHQQuestDataRegistry
{
public:
	static void ReloadFromSettings();
	static void Shutdown();

	static const TMap<FName, FHQQuestCompiledData>& GetCompiledQuestData();
	static UHQQuestHandlerRegistry* GetHandlerRegistry();
};
