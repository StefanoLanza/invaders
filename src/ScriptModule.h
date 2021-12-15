#pragma once

#include "Scripts.h"
#include "DLL.h"


struct Alien;

struct ScriptModule
{
	using AI = void (*)(AlienScriptId scriptId, Alien& alien, const ScriptArgs& args);

	AI   ai = nullptr;
	DLL  dll;
};


bool InitScriptModule(ScriptModule& module, const char* dllFileName);
bool ReloadScriptModule(ScriptModule& module);
