#include "AIModule.h"

namespace
{

bool GetProcedures(AIModule& module)
{
	module.alienScript = nullptr;
	module.alienWaveScript = nullptr;
	if (DLLError err = GetDLLProcedure(&module.alienScript, module.dll, "ExecuteAlienScript"); err != DLLError::ok)
	{
		return false;
	}
	if (DLLError err = GetDLLProcedure(&module.alienWaveScript, module.dll, "ExecuteAlienWaveScript"); err != DLLError::ok)
	{
		return false;
	}
	return true;
}

}

bool InitAIModule(AIModule& module, const char* dllFileName)
{
	if (DLLError err = LoadDLL(module.dll, dllFileName); err != DLLError::ok)
	{
		return false;
	}
	return GetProcedures(module);
}

bool ReloadScriptModule(AIModule& module)
{
	if (ReloadDLL(module.dll) == DLLError::ok)
	{
		return GetProcedures(module);
	}
	return false;
}
