#pragma once

#include "scripts/Scripts.h"
#include <engine/DLL.h>


struct Alien;

struct AIModule
{
	using Procedure = void (*)(AlienScriptId scriptId, Alien& alien, const ScriptArgs& args);

	Procedure procedure = nullptr;
	DLL  dll;
};


bool InitAIModule(AIModule& module, const char* dllFileName);
bool ReloadScriptModule(AIModule& module);
