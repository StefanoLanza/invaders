#include "ScriptModule.h"
#include "DLL.h"

namespace
{

bool GetProcedures(ScriptModule& module)
{
	module.ai = nullptr;
	if (DLLError err = GetDLLProcedure(&module.ai, module.dll, "ExecuteAlienScript"); err != DLLError::ok)
	{
		return false;
	}
	return true;
}

}

bool InitScriptModule(ScriptModule& module, const char* dllFileName)
{
	if (DLLError err = LoadDLL(module.dll, dllFileName); err != DLLError::ok)
	{
		return false;
	}
	return GetProcedures(module);
}

bool ReloadScriptModule(ScriptModule& module)
{
	// FIXME Release current DLL on success only ?
	if (ReloadDLL(module.dll) == DLLError::ok)
	{
		return GetProcedures(module);
	}
	return false;
}
