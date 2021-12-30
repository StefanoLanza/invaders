#include "AIModule.h"

namespace
{

bool GetProcedures(AIModule& module)
{
	module.procedure = nullptr;
	if (DLLError err = GetDLLProcedure(&module.procedure, module.dll, "ExecuteAlienScript"); err != DLLError::ok)
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
