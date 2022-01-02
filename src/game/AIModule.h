#pragma once

#include "scripts/Scripts.h"
#include <engine/DLL.h>


struct Alien;
struct AlienWave;

struct AIModule
{
	using AlienScript = void (*)(AlienScriptId scriptId, Alien& alien, const ScriptArgs& args);
	using AlienWaveScript = void (*)(AlienWave* wave, Alien* aliens, const ScriptArgs& args);

	AlienScript alienScript = nullptr;
	AlienWaveScript alienWaveScript = nullptr;
	DLL  dll;
};


bool InitAIModule(AIModule& module, const char* dllFileName);
bool ReloadScriptModule(AIModule& module);
