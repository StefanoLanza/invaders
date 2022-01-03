#pragma once

#include "scripts/Scripts.h"
#include <engine/DLL.h>


struct Alien;
struct AlienWave;

struct AIModule
{
	using AlienScript = void (*)(Alien& alien, const ScriptArgs& args);

	AlienScript alienScript = nullptr;
	DLL  dll;
};


bool InitAIModule(AIModule& module, const char* dllFileName);
bool ReloadScriptModule(AIModule& module);
