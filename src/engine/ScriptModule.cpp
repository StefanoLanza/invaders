#include "ScriptModule.h"
#include "DLL.h"

namespace
{

bool GetProcedures(ScriptModule& module)
{
#ifdef WINDOWS
	module.ai = nullptr;
	DLLError err;
	err = GetDLLProcedure(&module.ai, module.dll, "ExecuteAlienScript");
	if (err != DLLError::ok)
	{
		return false;
	}
	return true;
#else
	return false;
#endif
}

}

bool InitScriptModule(ScriptModule& module, const char* dllFileName)
{
#ifdef WINDOWS
	DLLError err = LoadDLL(module.dll, dllFileName);
	if (err != DLLError::ok)
	{
		return false;
	}
	return GetProcedures(module);
#else
	return false;
#endif
}

bool ReloadScriptModule(ScriptModule& module)
{
#ifdef WINDOWS
	// FIXME Release current DLL on success only ?
	bool res = true;
	if (ReloadDLL(module.dll) == DLLError::ok)
	{
		res = GetProcedures(module);
	}
	return res;
#else
	return false;
#endif
}
