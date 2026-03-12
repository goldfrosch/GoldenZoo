#pragma once

#include "Modules/ModuleManager.h"

class FChameleonRenderingModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
